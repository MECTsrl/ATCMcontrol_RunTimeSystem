/*
 * Copyright 2011 Mect s.r.l
 *
 * This file is part of FarosPLC.
 *
 * FarosPLC is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * FarosPLC is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * FarosPLC. If not, see http://www.gnu.org/licenses/.
*/

/*
 * Filename: dataEngine.c
 */

#include "dataImpl.h"

#define __4CFILE__	"dataEngine.c"

/* ---------------------------------------------------------------------------- */

enum EngineStatus engineStatus = enIdle;
pthread_mutex_t theCrosstableClientMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t theAlarmsEventsCondvar;
sem_t newOperations[MAX_DEVICES];

#if defined(RTS_CFG_MECT_RETAIN)
u_int32_t *retentive = NULL;
int do_flush_retentives = FALSE;
#endif

struct ServerStruct theServers[MAX_SERVERS];
u_int16_t theServersNumber = 0;

struct ClientStruct theDevices[MAX_DEVICES];
u_int16_t theDevicesNumber = 0;
u_int16_t theTcpDevicesNumber = 0;

struct NodeStruct theNodes[MAX_NODES];
u_int16_t theNodesNumber = 0;

/* ---------------------------------------------------------------------------- */

static pthread_t theDataSyncThread_id = WRONG_THREAD;
static enum threadStatus theDataSyncThreadStatus = NOT_STARTED;

static STaskInfoVMM *pVMM = NULL;

/* ---------------------------------------------------------------------------- */

#define OPER_GREATER    41
#define OPER_GREATER_EQ 42
#define OPER_SMALLER    43
#define OPER_SMALLER_EQ 44
#define OPER_EQUAL      45
#define OPER_NOT_EQUAL  46
#define OPER_RISING     47
#define OPER_FALLING    48

#define COMP_UNSIGNED   77
#define COMP_SIGNED16   78
#define COMP_SIGNED32   79
#define COMP_FLOATING   80

static int system_ini_ok;

static u_int16_t lastAlarmEvent = 0;

static const char *fieldbusName[] = {"PLC", "RTU", "TCP", "TCPRTU", "CANOPEN", "MECT", "RTU_SRV", "TCP_SRV", "TCPRTU_SRV" };

static int LoadXTable(void);
static u_int16_t tagAddr(char *tag);
static int checkServersDevicesAndNodes();
static void initNodeDiagnostic(u_int16_t n);

static int newAlarmEvent(int isAlarm, u_int16_t addr, char *expr, size_t len);
static inline void setAlarmEvent(int i);
static inline void clearAlarmEvent(int i);
static inline void checkAlarmEvent(int i, int condition);
static void AlarmMngr(void);

static unsigned plc_product_id();
static unsigned plc_serial_number();

/* ---------------------------------------------------------------------------- */

void engineInit()
{
    // initialize
    theDataSyncThread_id = WRONG_THREAD;
    theDataSyncThreadStatus = NOT_STARTED;
    int s, d, n;

    // read the configuration file
    if (app_config_load(&system_ini)) {
        fprintf(stderr, "[%s]: Error loading config file.\n", __func__);
        system_ini_ok = FALSE;
    } else {
        system_ini_ok = TRUE;
    }
    // non null default values
    for (n = 0; n < MAX_SERIAL_PORT; ++n) {
        if (system_ini.serial_port[n].max_block_size <= 0 || system_ini.serial_port[n].max_block_size > MAX_VALUES)
            system_ini.serial_port[n].max_block_size = MAX_VALUES;
    }
    if (system_ini.tcp_ip_port.max_block_size <= 0 || system_ini.tcp_ip_port.max_block_size > MAX_VALUES)
        system_ini.tcp_ip_port.max_block_size = MAX_VALUES;
    for (n = 0; n < MAX_CANOPEN; ++n) {
        if (system_ini.canopen[n].max_block_size <= 0 || system_ini.canopen[n].max_block_size> MAX_VALUES)
            system_ini.canopen[n].max_block_size = MAX_VALUES;
    }

    if (verbose_print_enabled) {
        app_config_dump(&system_ini);
    }

    // cleanup variables
    resetHmiPlcBlocks(&hmiBlock, &plcBlock);

    // default values
    VAR_VALUE(PLC_Version) = REVISION_HI * 1000 + REVISION_LO;
    VAR_VALUE(PLC_BEEP_VOLUME) = 0x00000064; // duty=100%
    VAR_VALUE(PLC_TOUCH_VOLUME) = 0x00000064; // duty=100%
    VAR_VALUE(PLC_ALARM_VOLUME) = 0x00000064; // duty=100%

    // P/N and S/N
    VAR_VALUE(PLC_PRODUCT_ID) = plc_product_id();
    VAR_VALUE(PLC_SERIAL_NUMBER) = plc_serial_number();

    // retentive variables
#if defined(RTS_CFG_MECT_RETAIN)
    if (ptRetentive == MAP_FAILED) {
        retentive = NULL;
        fprintf(stderr, "Missing retentive file.\n");
    } else {
        retentive = (u_int32_t *)ptRetentive;
        if (lenRetentive == LAST_RETENTIVE * 4) {
            OS_MEMCPY(&VAR_VALUE(1), retentive, LAST_RETENTIVE * 4);
        } else {
            fprintf(stderr, "Wrong retentive file size: got %ld expecting %u.\n", lenRetentive, LAST_RETENTIVE * 4);
        }
    }
#endif

    // initialize data array
    pthread_mutex_init(&theCrosstableClientMutex, NULL);
    {
        pthread_condattr_t attr;

        pthread_condattr_init(&attr);
        pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
        pthread_cond_init(&theAlarmsEventsCondvar, &attr);
    }
    for (s = 0; s < MAX_SERVERS; ++s) {
        theServers[s].thread_id = WRONG_THREAD;
    }
    for (d = 0; d < MAX_DEVICES; ++d) {
        theDevices[d].thread_id = WRONG_THREAD;
    }
}

void *engineThread(void *statusAdr)
{
    enum threadStatus *threadStatusPtr = (enum threadStatus *)statusAdr;
    int allOK = FALSE;
    pVMM = get_pVMM();

    // thread init
    osPthreadSetSched(FC_SCHED_VMM, FC_PRIO_VMM); // engineThread
    setEngineStatus(enInitialized);

    pthread_mutex_lock(&theCrosstableClientMutex);
    {
        int s, d, n;

        // XX_GPIO_SET(1);
        // load configuration
        if (LoadXTable()) {
            goto exit_initialization;
        }
        if (!system_ini_ok) {
            goto exit_initialization;
        }
        if (checkServersDevicesAndNodes()) {
            goto exit_initialization;
        }

        if (verbose_print_enabled) {
            fprintf(stderr, "engineThread(): %u servers, %u devices, %u nodes\n", theServersNumber, theDevicesNumber, theNodesNumber);
            for (s = 0; s < theServersNumber; ++s) {
                fprintf(stderr, "\t0x%02x: %s\n", s, theServers[s].name);
            }
            for (d = 0; d < theDevicesNumber; ++d) {
                fprintf(stderr, "\t0x%02x: %s\n", d, theDevices[d].name);
                for (n = 0; n < theNodesNumber; ++n) {
                    if (theNodes[n].device == d) {
                        fprintf(stderr, "\t\tnode#%2d NodeID=%u\n", n+1, theNodes[n].NodeID);
                    }
                }
            }
        }

        // i.MX28 workaround
        xx_pwm3_set(0);
        xx_pwm3_enable();

        // create servers
        for (s = 0; s < theServersNumber; ++s) {
            theServers[s].thread_status = NOT_STARTED;
            if (osPthreadCreate(&theServers[s].thread_id, NULL, &serverThread, &theServers[s], theServers[s].name, 0) == 0) {
                do {
                    osSleep(THE_CONFIG_DELAY_ms); // not sched_yield();
                } while (theServers[s].thread_status != RUNNING);
            } else {
                fprintf(stderr, "[%s] ERROR creating server thread %s: %s.\n", __func__, theServers[s].name, strerror(errno));
            }
        }
        // create clients
        for (d = 0; d < theDevicesNumber; ++d) {
            theDevices[d].thread_status = NOT_STARTED;
            if (osPthreadCreate(&theDevices[d].thread_id, NULL, &clientThread, &theDevices[d], theDevices[d].name, 0) == 0) {
                do {
                    osSleep(THE_CONFIG_DELAY_ms); // not sched_yield();
                } while (theDevices[d].thread_status != RUNNING);
            } else {
                fprintf(stderr, "[%s] ERROR creating device thread %s: %s.\n", __func__, theDevices[d].name, strerror(errno));
            }
        }
        // create udp server
        if (osPthreadCreate(&theDataSyncThread_id, NULL, &datasyncThread, &theDataSyncThreadStatus, "datasync", 0) == 0) {
            do {
                osSleep(THE_CONFIG_DELAY_ms); // not sched_yield();
            } while (theDataSyncThreadStatus != RUNNING);
        }
        // ok, all done
        allOK = TRUE;

    exit_initialization:
         // XX_GPIO_CLR(1);
        buzzer_periods = 0;
        buzzer_tic = 0;
        xx_pwm3_disable();
    }
    pthread_mutex_unlock(&theCrosstableClientMutex);

    if (allOK) {
        fprintf(stderr, "PLC engine is running\n");
        setEngineStatus(enRunning);
    } else {
        fprintf(stderr, "**********************************************************\n");
        fprintf(stderr, "* PLC engine is in error status: application won't work! *\n");
        fprintf(stderr, "**********************************************************\n");
        setEngineStatus(enError);
    }
    // run
    struct timespec abstime;
    clock_gettime(CLOCK_MONOTONIC, &abstime); // pthread_cond_timedwait + pthread_condattr_setclock

    // NO default Fast I/O config PLC_FastIO_Dir

    // XX_GPIO_SET(1);
    int tic = 0;
    RTIME tic_ns;
    struct timespec tic_ts;

    if (timer_overflow_enabled) {
        clock_gettime_overflow(CLOCK_MONOTONIC, &tic_ts);
        tic_ns = tic_ts.tv_sec * UN_MILIARDO_ULL + tic_ts.tv_nsec;
    } else {
        tic_ns = rt_timer_read();
    }
    VAR_VALUE(PLC_UPTIME_cs) = tic_ns / DIECI_MILIONI_UL;
    VAR_VALUE(PLC_UPTIME_s) = VAR_VALUE(PLC_UPTIME_cs) / 100UL;

    pthread_mutex_lock(&theCrosstableClientMutex);
    *threadStatusPtr = RUNNING;
    while (engineStatus != enExiting) {

        // trivial scenario
        if (engineStatus != enRunning) {
            // XX_GPIO_CLR(1);
            pthread_mutex_unlock(&theCrosstableClientMutex);
            osSleep(THE_ENGINE_DELAY_ms);
            pthread_mutex_lock(&theCrosstableClientMutex);
            // XX_GPIO_SET(1);
            continue;
        }

        // ldiv_t x = ldiv(THE_ENGINE_DELAY_ms, 1000);
        // abstime.tv_sec += x.quot;
        // abstime.tv_sec += x.rem * 1E6;
        abstime.tv_nsec = abstime.tv_nsec + (THE_ENGINE_DELAY_ms * 1E6);
        if (abstime.tv_nsec >= 1E9) {
            abstime.tv_sec += 1;
            abstime.tv_nsec -= 1E9;
        }
        while (TRUE) {
            int e;
            // XX_GPIO_CLR(1);
            e = pthread_cond_timedwait(&theAlarmsEventsCondvar, &theCrosstableClientMutex, &abstime);

            if (timer_overflow_enabled) {
                clock_gettime_overflow(CLOCK_MONOTONIC, &tic_ts);
                tic_ns = tic_ts.tv_sec * UN_MILIARDO_ULL + tic_ts.tv_nsec;
            } else {
                tic_ns = rt_timer_read();
            }
            VAR_VALUE(PLC_UPTIME_cs) = tic_ns / DIECI_MILIONI_UL;
            VAR_VALUE(PLC_UPTIME_s) = VAR_VALUE(PLC_UPTIME_cs) / 100UL;

            // XX_GPIO_SET(1);
            if (e == ETIMEDOUT) {
                break;
            }
            AlarmMngr();
        }

        tic = (tic + 1) % 10;
        if (tic == 1) {
            // datetime   NB no writeQdataRegisters();
            struct timespec tv;
            struct tm datetime;
#ifdef __XENO__
            clock_gettime_overflow(CLOCK_HOST_REALTIME, &tv);
#else
            clock_gettime_overflow(CLOCK_REALTIME, &tv);
#endif
            if (localtime_r(&tv.tv_sec, &datetime)) {
                VAR_VALUE(PLC_Seconds) = datetime.tm_sec;
                VAR_VALUE(PLC_Minutes) = datetime.tm_min;
                VAR_VALUE(PLC_Hours) = datetime.tm_hour;
                VAR_VALUE(PLC_Day) = datetime.tm_mday;
                VAR_VALUE(PLC_Month) = datetime.tm_mon + 1;
                VAR_VALUE(PLC_Year) = 1900 + datetime.tm_year;
            }
        }

        if (tic == 1 || tic == 6) {
            // TICtimer  NB no writeQdataRegisters();

            float plc_time, plc_timeMin, plc_timeMax, plc_timeWin;
            RTIME tic_ms = tic_ns / UN_MILIONE_UL;

            tic_ms = tic_ms % (86400 * 1000); // 1 day overflow
            plc_time = tic_ms / 1000.0;
            memcpy(&plc_timeWin, &VAR_VALUE(PLC_timeWin), sizeof(u_int32_t));

            if (plc_timeWin < 5.0) {
                plc_timeWin = 5.0;
            }
            if (plc_time <= plc_timeWin) {
                plc_timeMin = 0;
                plc_timeMax = plc_timeWin;
            } else {
                plc_timeMin = plc_time - plc_timeWin;
                plc_timeMax = plc_time;
            }
            memcpy(&VAR_VALUE(PLC_time), &plc_time, sizeof(u_int32_t));
            memcpy(&VAR_VALUE(PLC_timeMin), &plc_timeMin, sizeof(u_int32_t));
            memcpy(&VAR_VALUE(PLC_timeMax), &plc_timeMax, sizeof(u_int32_t));
            memcpy(&VAR_VALUE(PLC_timeWin), &plc_timeWin, sizeof(u_int32_t));
        }

        if (VAR_VALUE(PLC_ResetValues)) {
            u_int16_t addr;
            for (addr = 5000; addr < 5160; addr += 10) {
                VAR_VALUE(addr + 3) = 0; // READS
                VAR_VALUE(addr + 4) = 0; // WRITES
                VAR_VALUE(addr + 5) = 0; // TIMEOUTS
                VAR_VALUE(addr + 6) = 0; // COMM_ERRORS
                VAR_VALUE(addr + 7) = 0; // LAST_ERROR
            }
            VAR_VALUE(PLC_ResetValues) = 0;
        }

        // PLC_FastIO, both In and Out
        unsigned addr;
        unsigned value;

        for (addr = PLC_FastIO_1; addr < (PLC_FastIO_1 + XX_GPIO_MAX); ++addr) {
            value = xx_gpio_get(addr - PLC_FastIO_1);

            if (value != VAR_VALUE(addr)) {
                writeQdataRegisters(addr, value, DATA_OK);
            }
        }

        addr = PLC_WATCHDOG_ms;
        value = xx_watchdog_get();
        if (value != VAR_VALUE(addr)) {
            VAR_VALUE(addr) = value; // NB no writeQdataRegisters();
        }

        // BUZZER
        if (buzzer_periods > 0) {
            ++buzzer_tic;

            if (buzzer_tic > buzzer_period_tics) {
                ++buzzer_periods;

                if (buzzer_periods > buzzer_replies) {
                    xx_pwm3_disable();
                    buzzer_tic = 0;
                    buzzer_periods = 0;
                } else {
                    xx_pwm3_enable();
                    buzzer_tic = 1;
                }
            } else if (buzzer_tic == buzzer_on_cs) {
                xx_pwm3_disable();
            }
        }

        if (do_flush_retentives) {
            do_flush_retentives = FALSE;
            // the mutex was locked by pthread_cond_timedwait()
            pthread_mutex_unlock(&theCrosstableClientMutex);
            {
                // syncing the retentive file but without holding the mutex
                msync((void *)retentive, lenRetentive, MS_SYNC);
            }
            pthread_mutex_lock(&theCrosstableClientMutex);
        }

    }
    pthread_mutex_unlock(&theCrosstableClientMutex);

    // thread clean
    {
        void *retval;
        unsigned n;

        for (n = 0; n < theDevicesNumber; ++n) {
            if (theDevices[n].thread_id != WRONG_THREAD) {
                pthread_join(theDevices[n].thread_id, &retval);
                theDevices[n].thread_id = WRONG_THREAD;
                fprintf(stderr, "joined dev(%d)\n", n);
            }
        }
        for (n = 0; n < theServersNumber; ++n) {
            if (theServers[n].thread_id != WRONG_THREAD) {
                pthread_join(theServers[n].thread_id, &retval);
                theServers[n].thread_id = WRONG_THREAD;
            fprintf(stderr, "joined srv(%d)\n", n);
            }
        }
        if (theDataSyncThread_id != WRONG_THREAD) {
            pthread_join(theDataSyncThread_id, &retval);
            theDataSyncThread_id = WRONG_THREAD;
            fprintf(stderr, "joined datasync\n");
        }
    }

    // exit
    // XX_GPIO_CLR(1);
    fprintf(stderr, "EXITING: engineThread\n");
    *threadStatusPtr = EXITING;
    return NULL;
}

/* ---------------------------------------------------------------------------- */

static int LoadXTable(void)
{
    u_int32_t addr, indx;
    int ERR = FALSE;
    FILE *xtable = NULL;

    // init tables
    for (addr = 1; addr <= DimCrossTable; ++addr) {
        CrossTable[addr].Enable = 0;
        CrossTable[addr].Plc = FALSE;
        CrossTable[addr].Tag[0] = UNKNOWN;
        CrossTable[addr].Types = 0;
        CrossTable[addr].Decimal = 0;
        CrossTable[addr].Protocol = PLC;
        CrossTable[addr].IPAddress = 0x00000000;
        CrossTable[addr].Port = 0;
        CrossTable[addr].NodeId = 0;
        CrossTable[addr].Offset = 0;
        CrossTable[addr].Block = 0;
        CrossTable[addr].BlockSize = 0;
        CrossTable[addr].Output = FALSE;
        CrossTable[addr].OldVal = 0;
        CrossTable[addr].device = 0xffff;
        CrossTable[addr].node = 0xffff;
        VAR_STATE(addr) = DATA_ERROR; // in error until we actually read it
    }
    lastAlarmEvent = 0;
    for (addr = 0; addr <= DimAlarmsCT; ++addr) {
        ALCrossTable[addr].ALType = FALSE;
        ALCrossTable[addr].ALSource[0] = '\0';
        ALCrossTable[addr].ALCompareVar[0] = '\0';
        ALCrossTable[addr].TagAddr = 0;
        ALCrossTable[addr].SourceAddr = 0;
        ALCrossTable[addr].CompareAddr = 0;
        ALCrossTable[addr].ALCompareVal.u32 = 0;
        ALCrossTable[addr].ALOperator = 0;
        ALCrossTable[addr].ALFilterTime = 0;
        ALCrossTable[addr].ALFilterCount = 0;
        ALCrossTable[addr].comparison = 0;
    }

    // open file
    fprintf(stderr, "loading '%s' ...", CROSSTABLE_CSV);
    xtable = fopen(CROSSTABLE_CSV, "r");
    if (xtable == NULL)  {
        ERR = TRUE;
        goto exit_function;
    }

    // read loop
    for (addr = 1; addr <= DimCrossTable; ++ addr) {
        char row[1024], *p, *r;

        if (fgets(row, 1024, xtable) == NULL) {
            // no ERR = TRUE;
            continue;
        }

        // Enable {0,1,2,3}
        p = strtok_csv(row, ";", &r);
        if (p == NULL) {
            ERR = TRUE;
            break;
        }
        CrossTable[addr].Enable = atoi(p);
        // skip empty or disabled variables
        if (CrossTable[addr].Enable == 0) {
            continue;
        }

        // Plc {H,P,S,F}
        p = strtok_csv(NULL, ";", &r);
        if (p == NULL) {
            ERR = TRUE;
            break;
        }
        switch (p[0]) {
        case 'H':
            CrossTable[addr].Plc = Htype;
            break;
        case 'P':
            CrossTable[addr].Plc = Ptype;
            break;
        case 'S':
            CrossTable[addr].Plc = Stype;
            break;
        case 'F':
            CrossTable[addr].Plc = Ftype;
            break;
        case 'V':
            CrossTable[addr].Plc = Vtype;
            break;
        case 'X':
            CrossTable[addr].Plc = Xtype;
            break;
        default:
            ERR = TRUE;
        }
        if (ERR) {
            break;
        }

        // Tag {identifier}
        p = strtok_csv(NULL, ";", &r);
        if (p == NULL) {
            ERR = TRUE;
            break;
        }
        strncpy(CrossTable[addr].Tag, p, MAX_IDNAME_LEN);

        // Types {UINT, UDINT, DINT, FDCBA, ...}
        p = strtok_csv(NULL, ";", &r);
        if (p == NULL) {
            ERR = TRUE;
            break;
        }
        if (strcmp(p, "BIT") == 0) {
            CrossTable[addr].Types = BIT;
        } else if (strcmp(p, "BYTE") == 0) {
            CrossTable[addr].Types = UINT8;
        } else if (strcmp(p, "BYTE_BIT") == 0) {
            CrossTable[addr].Types = BYTE_BIT;
        } else if (strcmp(p, "WORD_BIT") == 0) {
            CrossTable[addr].Types = WORD_BIT;
        } else if (strcmp(p, "DWORD_BIT") == 0) {
            CrossTable[addr].Types = DWORD_BIT;
        } else if (strcmp(p, "UINT") == 0) {
            CrossTable[addr].Types = UINT16;
        } else if (strcmp(p, "UINTBA") == 0) {
            CrossTable[addr].Types = UINT16BA;
        } else if (strcmp(p, "INT") == 0) {
            CrossTable[addr].Types = INT16;
        } else if (strcmp(p, "INTBA") == 0) {
            CrossTable[addr].Types = INT16BA;
        } else if (strcmp(p, "UDINT") == 0) {
            CrossTable[addr].Types = UDINT;
        } else if (strcmp(p, "UDINTDCBA") == 0) {
            CrossTable[addr].Types = UDINTDCBA;
        } else if (strcmp(p, "UDINTCDAB") == 0) {
            CrossTable[addr].Types = UDINTCDAB;
        } else if (strcmp(p, "UDINTBADC") == 0) {
            CrossTable[addr].Types = UDINTBADC;
        } else if (strcmp(p, "DINT") == 0) {
            CrossTable[addr].Types = DINT;
        } else if (strcmp(p, "DINTDCBA") == 0) {
            CrossTable[addr].Types = DINTDCBA;
        } else if (strcmp(p, "DINTCDAB") == 0) {
            CrossTable[addr].Types = DINTCDAB;
        } else if (strcmp(p, "DINTBADC") == 0) {
            CrossTable[addr].Types = DINTBADC;
        } else if (strcmp(p, "REAL") == 0) {
            CrossTable[addr].Types = REAL;
        } else if (strcmp(p, "REALDCBA") == 0) {
            CrossTable[addr].Types = REALDCBA;
        } else if (strcmp(p, "REALCDAB") == 0) {
            CrossTable[addr].Types = REALCDAB;
        } else if (strcmp(p, "REALBADC") == 0) {
            CrossTable[addr].Types = REALBADC;

        } else if (strcmp(p, "UINTAB") == 0) {
            CrossTable[addr].Types = UINT16; // backward compatibility
        } else if (strcmp(p, "INTAB") == 0) {
            CrossTable[addr].Types = INT16; // backward compatibility
        } else if (strcmp(p, "UDINTABCD") == 0) {
            CrossTable[addr].Types = UDINT; // backward compatibility
        } else if (strcmp(p, "DINTABCD") == 0) {
            CrossTable[addr].Types = DINT; // backward compatibility
        } else if (strcmp(p, "FDCBA") == 0) {
            CrossTable[addr].Types = REALDCBA; // backward compatibility
        } else if (strcmp(p, "FCDAB") == 0) {
            CrossTable[addr].Types = REALCDAB; // backward compatibility
        } else if (strcmp(p, "FABCD") == 0) {
            CrossTable[addr].Types = REAL; // backward compatibility
        } else if (strcmp(p, "FBADC") == 0) {
            CrossTable[addr].Types = REALBADC; // backward compatibility

        } else {
            if (CrossTable[addr].Enable > 0) {
                CrossTable[addr].Types = UNKNOWN;
                ERR = TRUE;
                break;
            }
        }

        // Decimal {0, 1, 2, 3, 4, ...}
        p = strtok_csv(NULL, ";", &r);
        if (p == NULL) {
            ERR = TRUE;
            break;
        }
        CrossTable[addr].Decimal = atoi(p);

        // Protocol {"", RTU, TCP, TCPRTU}
        p = strtok_csv(NULL, ";", &r);
        if (p == NULL) {
            ERR = TRUE;
            break;
        }
        if (strcmp(p, "PLC") == 0) {
            CrossTable[addr].Protocol = PLC;
            VAR_STATE(addr) = DATA_OK; // PLC variables are already ok at startup
        } else if (strcmp(p, "RTU") == 0) {
            CrossTable[addr].Protocol = RTU;
        } else if (strcmp(p, "TCP") == 0) {
            CrossTable[addr].Protocol = TCP;
        } else if (strcmp(p, "TCPRTU") == 0) {
            CrossTable[addr].Protocol = TCPRTU;
        } else if (strcmp(p, "CANOPEN") == 0) {
            CrossTable[addr].Protocol = CANOPEN;
        } else if (strcmp(p, "MECT") == 0) {
            CrossTable[addr].Protocol = MECT;
        } else if (strcmp(p, "RTU_SRV") == 0) {
            CrossTable[addr].Protocol = RTU_SRV;
        } else if (strcmp(p, "TCP_SRV") == 0) {
            CrossTable[addr].Protocol = TCP_SRV;
        } else if (strcmp(p, "TCPRTU_SRV") == 0) {
            CrossTable[addr].Protocol = TCPRTU_SRV;
        } else {
            CrossTable[addr].Protocol = PLC;
            ERR = TRUE;
            break;
        }

        // IPAddress {identifier}
        p = strtok_csv(NULL, ";", &r);
        if (p == NULL) {
            ERR = TRUE;
            break;
        }
        CrossTable[addr].IPAddress = str2ipaddr(p);

        // Port {number}
        p = strtok_csv(NULL, ";", &r);
        if (p == NULL) {
            ERR = TRUE;
            break;
        }
        CrossTable[addr].Port = atoi(p);

        // NodeId {number}
        p = strtok_csv(NULL, ";", &r);
        if (p == NULL) {
            ERR = TRUE;
            break;
        }
        CrossTable[addr].NodeId = atoi(p);

        // Address {number}
        p = strtok_csv(NULL, ";", &r);
        if (p == NULL) {
            ERR = TRUE;
            break;
        }
        CrossTable[addr].Offset = atoi(p);

        // Block {number}
        p = strtok_csv(NULL, ";", &r);
        if (p == NULL) {
            ERR = TRUE;
            break;
        }
        CrossTable[addr].Block = atoi(p);

        // NReg {number}
        p = strtok_csv(NULL, ";", &r);
        if (p == NULL) {
            ERR = TRUE;
            break;
        }
        CrossTable[addr].BlockSize = atoi(p);

        // Handle ([RO]||[RW]|[AL|EV SOURCE OP COND]){text}
        p = strtok_csv(NULL, ";", &r);
        if (p == NULL) {
            ERR = TRUE;
            break;
        }
        if (strncmp(p, "[RW]", 4) == 0) {
            CrossTable[addr].Output = TRUE;
        } else if (strncmp(p, "[RO]", 4) == 0) {
            CrossTable[addr].Output = FALSE;
        } else if (strncmp(p, "[AL ", 4) == 0) {
            CrossTable[addr].Output = FALSE;
            if (strlen(p) < 10 || newAlarmEvent(1, addr, &(p[3]), strlen(p) - 3)) {
                ERR = TRUE;
                break;
            }
        } else if (strncmp(p, "[EV ", 4) == 0) {
            CrossTable[addr].Output = FALSE;
            if (strlen(p) < 10 || newAlarmEvent(0, addr, &p[3], strlen(p) - 3)) {
                ERR = TRUE;
                break;
            }
        }
    }
    if (ERR) {
        goto exit_function;
    }

    // check alarms and events
    fprintf(stderr, "\nalarms/events:\n");
    for (indx = 1; indx <= lastAlarmEvent; ++indx) {
        // retrieve the source variable address
        addr = tagAddr(ALCrossTable[indx].ALSource);
        if (addr == 0) {
            ERR = TRUE;
            break;
        }
        ALCrossTable[indx].SourceAddr = addr;
        CrossTable[addr].usedInAlarmsEvents = TRUE;
        fprintf(stderr, "\t%2d: %s", indx, CrossTable[ALCrossTable[indx].TagAddr].Tag);
        fprintf(stderr, " = %s", CrossTable[ALCrossTable[indx].SourceAddr].Tag);

        // which comparison?
        switch (CrossTable[addr].Types) {
        case BIT:
        case BYTE_BIT:
        case WORD_BIT:
        case DWORD_BIT:
            ALCrossTable[indx].comparison = COMP_UNSIGNED;
            fprintf(stderr, " (u)");
            break;
        case INT16:
        case INT16BA:
            ALCrossTable[indx].comparison = COMP_SIGNED16;
            fprintf(stderr, " (s16)");
            break;
        case DINT:
        case DINTDCBA:
        case DINTCDAB:
        case DINTBADC:
            ALCrossTable[indx].comparison = COMP_SIGNED32;
            fprintf(stderr, " (s32)");
            break;
        case UINT8:
        case UINT16:
        case UINT16BA:
        case UDINT:
        case UDINTDCBA:
        case UDINTCDAB:
        case UDINTBADC:
            ALCrossTable[indx].comparison = COMP_UNSIGNED;
            fprintf(stderr, " (u)");
            break;
        case REAL:
        case REALDCBA:
        case REALCDAB:
        case REALBADC:
            ALCrossTable[indx].comparison = COMP_FLOATING;
            fprintf(stderr, " (f)");
            break;
        default:
            ; // FIXME: assert
        }

        switch (ALCrossTable[indx].ALOperator)  {
        case OPER_RISING    : fprintf(stderr, " RISING"); break;
        case OPER_FALLING   : fprintf(stderr, " FALLING"); break;
        case OPER_EQUAL     : fprintf(stderr, " =="); break;
        case OPER_NOT_EQUAL : fprintf(stderr, " !="); break;
        case OPER_GREATER   : fprintf(stderr, " >" ); break;
        case OPER_GREATER_EQ: fprintf(stderr, " >="); break;
        case OPER_SMALLER   : fprintf(stderr, " <" ); break;
        case OPER_SMALLER_EQ: fprintf(stderr, " <="); break;
        default             : ;
        }

        if (ALCrossTable[lastAlarmEvent].ALOperator != OPER_FALLING
         && ALCrossTable[lastAlarmEvent].ALOperator != OPER_RISING) {

            // if the comparison is with a variable
            if (ALCrossTable[indx].ALCompareVar[0] != 0) {
                int compatible = TRUE;

                // then retrieve the compare variable address
                addr = tagAddr(ALCrossTable[indx].ALCompareVar);
                if (addr == 0) {
                    ERR = TRUE;
                    break;
                }
                ALCrossTable[indx].CompareAddr = addr;
                CrossTable[addr].usedInAlarmsEvents = TRUE;
                fprintf(stderr, " %s", CrossTable[addr].Tag);

                // check for incompatibles types
                switch (CrossTable[ALCrossTable[indx].SourceAddr].Types) {

                case BIT:
                case BYTE_BIT:
                case WORD_BIT:
                case DWORD_BIT:
                    switch (CrossTable[ALCrossTable[indx].CompareAddr].Types) {
                    case BIT:
                    case BYTE_BIT:
                    case WORD_BIT:
                    case DWORD_BIT:
                        compatible = TRUE;
                        break;
                    case INT16:
                    case INT16BA:
                    case DINT:
                    case DINTDCBA:
                    case DINTCDAB:
                    case DINTBADC:
                        compatible = FALSE; // only == 0 and != 0
                        break;
                    case UINT8:
                    case UINT16:
                    case UINT16BA:
                    case UDINT:
                    case UDINTDCBA:
                    case UDINTCDAB:
                    case UDINTBADC:
                        compatible = FALSE; // only == 0 and != 0
                        break;
                    case REAL:
                    case REALDCBA:
                    case REALCDAB:
                    case REALBADC:
                        compatible = FALSE; // only == 0 and != 0
                        break;
                    default:
                        ; // FIXME: assert
                    }
                    break;

                case INT16:
                case INT16BA:
                case DINT:
                case DINTDCBA:
                case DINTCDAB:
                case DINTBADC:
                    switch (CrossTable[ALCrossTable[indx].CompareAddr].Types) {
                    case BIT:
                    case BYTE_BIT:
                    case WORD_BIT:
                    case DWORD_BIT:
                        compatible = FALSE;
                        break;
                    case INT16:
                    case INT16BA:
                    case DINT:
                    case DINTDCBA:
                    case DINTCDAB:
                    case DINTBADC:
                        compatible = (CrossTable[ALCrossTable[indx].SourceAddr].Decimal == CrossTable[ALCrossTable[indx].CompareAddr].Decimal);
                        break;
                    case UINT8:
                    case UINT16:
                    case UINT16BA:
                    case UDINT:
                    case UDINTDCBA:
                    case UDINTCDAB:
                    case UDINTBADC:
                        // compatible = (CrossTable[ALCrossTable[indx].SourceAddr].Decimal == CrossTable[ALCrossTable[indx].CompareAddr].Decimal);
                        compatible = FALSE;
                        break;
                    case REAL:
                    case REALDCBA:
                    case REALCDAB:
                    case REALBADC:
                        compatible = FALSE;
                        break;
                    default:
                        ; // FIXME: assert
                    }
                    break;

                case UINT8:
                case UINT16:
                case UINT16BA:
                case UDINT:
                case UDINTDCBA:
                case UDINTCDAB:
                case UDINTBADC:
                    switch (CrossTable[ALCrossTable[indx].CompareAddr].Types) {
                    case BIT:
                    case BYTE_BIT:
                    case WORD_BIT:
                    case DWORD_BIT:
                        compatible = FALSE;
                        break;
                    case INT16:
                    case INT16BA:
                    case DINT:
                    case DINTDCBA:
                    case DINTCDAB:
                    case DINTBADC:
                        // compatible = (CrossTable[ALCrossTable[indx].SourceAddr].Decimal == CrossTable[ALCrossTable[indx].CompareAddr].Decimal);
                        compatible = FALSE;
                        break;
                    case UINT8:
                    case UINT16:
                    case UINT16BA:
                    case UDINT:
                    case UDINTDCBA:
                    case UDINTCDAB:
                    case UDINTBADC:
                        compatible = (CrossTable[ALCrossTable[indx].SourceAddr].Decimal == CrossTable[ALCrossTable[indx].CompareAddr].Decimal);
                        break;
                    case REAL:
                    case REALDCBA:
                    case REALCDAB:
                    case REALBADC:
                        compatible = FALSE;
                        break;
                    default:
                        ; // FIXME: assert
                    }
                    break;

                case REAL:
                case REALDCBA:
                case REALCDAB:
                case REALBADC:
                    switch (CrossTable[ALCrossTable[indx].CompareAddr].Types) {
                    case BIT:
                    case BYTE_BIT:
                    case WORD_BIT:
                    case DWORD_BIT:
                        compatible = FALSE;
                        break;
                    case INT16:
                    case INT16BA:
                    case DINT:
                    case DINTDCBA:
                    case DINTCDAB:
                    case DINTBADC:
                        compatible = FALSE;
                        break;
                    case UINT8:
                    case UINT16:
                    case UINT16BA:
                    case UDINT:
                    case UDINTDCBA:
                    case UDINTCDAB:
                    case UDINTBADC:
                        compatible = FALSE;
                        break;
                    case REAL:
                    case REALDCBA:
                    case REALCDAB:
                    case REALBADC:
                        compatible = TRUE; // no decimal test
                        break;
                    default:
                        ; // FIXME: assert
                    }
                    break;

                default:
                    ; // FIXME: assert
                }
                if (! compatible) {
                    fprintf(stderr, " [WARNING: comparison between incompatible types]");
                }

            } else {
                // the comparison is with a fixed value, now check for the vartype
                // since we saved the value as float before
                float fvalue = ALCrossTable[indx].ALCompareVal.f;
                int n;

                switch (CrossTable[addr].Types) {
                case BIT:
                case BYTE_BIT:
                case WORD_BIT:
                case DWORD_BIT:
                    if (fvalue <= 0.0) {
                        ALCrossTable[indx].ALCompareVal.u32 = 0;
                    } else if (fvalue <= 1.0) {
                        ALCrossTable[indx].ALCompareVal.u32 = 1;
                    } else {
                        ALCrossTable[indx].ALCompareVal.u32 = 2;
                    }
                    break;
                case INT16:
                case INT16BA:
                    for (n = 0; n < CrossTable[addr].Decimal; ++n) {
                        fvalue *= 10;
                    }
                    ALCrossTable[indx].ALCompareVal.i16 = fvalue;
                    break;
                case DINT:
                case DINTDCBA:
                case DINTCDAB:
                case DINTBADC:
                    for (n = 0; n < CrossTable[addr].Decimal; ++n) {
                        fvalue *= 10;
                    }
                    ALCrossTable[indx].ALCompareVal.i32 = fvalue;
                    break;
                case UINT8:
                case UINT16:
                case UINT16BA:
                case UDINT:
                case UDINTDCBA:
                case UDINTCDAB:
                case UDINTBADC:
                    if (fvalue <= 0.0) {
                        fvalue = 0.0; // why check unsigned with a negative value?
                    } else {
                        for (n = 0; n < CrossTable[addr].Decimal; ++n) {
                            fvalue *= 10;
                        }
                    }
                    // NB this may overflow
                    ALCrossTable[indx].ALCompareVal.u32 = fvalue;
                    break;
                case REAL:
                case REALDCBA:
                case REALCDAB:
                case REALBADC:
                    // the value is already stored as a float, comparisons will be ok
                    break;
                default:
                    ; // FIXME: assert
                }

                switch (ALCrossTable[indx].comparison)
                {
                case COMP_UNSIGNED:
                    fprintf(stderr, " %u", ALCrossTable[indx].ALCompareVal.u32);
                    break;
                case COMP_SIGNED16:
                    fprintf(stderr, " %d", ALCrossTable[indx].ALCompareVal.i16);
                    break;
                case COMP_SIGNED32:
                    fprintf(stderr, " %d", ALCrossTable[indx].ALCompareVal.i32);
                    break;
                case COMP_FLOATING:
                    fprintf(stderr, " %f", ALCrossTable[indx].ALCompareVal.f);
                    break;
                default:
                    ;
                }
            }
        }
        fprintf(stderr, "\n");
    }

    // close file
exit_function:
    if (xtable) {
        fclose(xtable);
    }
    fprintf(stderr, " %s\n", (ERR) ? "ERROR" : "OK");
    return ERR;
}

static u_int16_t tagAddr(char *tag)
{
    u_int16_t addr;

    for (addr = 1; addr <= DimCrossTable; ++addr) {
        if (strncmp(tag, CrossTable[addr].Tag, MAX_IDNAME_LEN) == 0) {
            return addr;
        }
    }
    return 0;
}

static int checkServersDevicesAndNodes()
{
    int retval = 0;
    int disable_all_nodes = FALSE;
    FILE *hmi_ini;

    // init tables
    theDevicesNumber = 0;
    theNodesNumber = 0;
    bzero(&theDevices[0], sizeof(theDevices));
    bzero(&theNodes[0], sizeof(theNodes));

    // check hmi.ini: search for "disable_all_nodes=true"
    hmi_ini = fopen(HMI_INI, "r");
    if (hmi_ini) {
        char *disable_all_nodes_true = "disable_all_nodes=true";
        char row[1024];

        while (fgets(row, 1024, hmi_ini)) {
            char *p = &row[0];
            unsigned i;

            // trim
            for (i = 0; i < strlen(row); ++i) {
                if (isblank(row[i])) {
                    continue;
                } else {
                    p = &row[i];
                    break;
                }
            }
            // check
            if (strncmp(p, disable_all_nodes_true, strlen(disable_all_nodes_true)) == 0) {
                disable_all_nodes = TRUE;
                fprintf(stderr, "%s() disabling all nodes as requested\n", __func__);
                break;
            }
        }
        fclose(hmi_ini);
    }

    // for each enabled variable
    u_int16_t i, base, block;
    fprintf(stderr, "%s()\n", __func__);
    for (i = 1, base = 1, block = 0; i <= DimCrossTable; ++i) {

        // find block base addresses
        if (CrossTable[i].Block != block) {
            base = i;
            block = CrossTable[i].Block;
        }
        CrossTable[i].BlockBase = base;

        if (CrossTable[i].Enable > 0) {
            u_int16_t s = MAX_SERVERS;
            u_int16_t d;
            u_int16_t n;
            u_int16_t p;

            // server variables =---> enable the server thread
            switch (CrossTable[i].Protocol) {
            case PLC:
                // no plc server
                break;
            case RTU:
            case TCP:
            case TCPRTU:
            case CANOPEN:
            case MECT:
                // nothing to do for server
                break;
            case RTU_SRV:
                // add unique variable's server
                for (s = 0; s < theServersNumber; ++s) {
                    if (RTU_SRV == theServers[s].protocol && CrossTable[i].Port == theServers[s].port) {
                        // already present
                        if (theServers[s].IPaddress != CrossTable[i].IPAddress) {
                            char str1[42];
                            char str2[42];
                            fprintf(stderr,
                                "%s() WARNING in variable #%u wrong 'IP Address' %s (should be %s)\n",
                                __func__, i, ipaddr2str(CrossTable[i].IPAddress, str2), ipaddr2str(theServers[s].IPaddress, str1));
                        }
                        if (theServers[s].NodeId != CrossTable[i].NodeId) {
                            fprintf(stderr,
                                "%s() WARNING in variable #%u wrong 'Node ID' %u (should be %u)\n",
                                __func__, i, CrossTable[i].NodeId, theServers[s].NodeId);
                        }
                        break;
                    }
                }
                goto add_server;
                // no break;
            case TCP_SRV:
            case TCPRTU_SRV:
                // add unique variable's server
                for (s = 0; s < theServersNumber; ++s) {
                    if (CrossTable[i].Protocol == theServers[s].protocol) {
                        // already present
                        if (theServers[s].IPaddress != CrossTable[i].IPAddress) {
                            char str1[42];
                            char str2[42];
                            fprintf(stderr,
                                "%s() WARNING in variable #%u wrong 'IP Address' %s (should be %s)\n",
                                __func__, i, ipaddr2str(CrossTable[i].IPAddress, str2), ipaddr2str(theServers[s].IPaddress, str1));
                        }
                        if (theServers[s].port != CrossTable[i].Port) {
                            fprintf(stderr,
                                "%s() WARNING in variable #%u wrong 'Port' %d (should be %u)\n",
                                __func__, i, CrossTable[i].Port, theServers[s].port);
                        }
                        if (theServers[s].NodeId != CrossTable[i].NodeId) {
                            fprintf(stderr,
                                "%s() WARNING in variable #%u wrong 'Node ID' %u (should be %u)\n",
                                __func__, i, CrossTable[i].NodeId, theServers[s].NodeId);
                        }
                        break;
                    }
                }
            add_server:
                if (s < theServersNumber) {
                    // ok already present
                } else if (theServersNumber >= MAX_SERVERS) {
                    fprintf(stderr, "%s() too many servers (max=%d)\n", __func__, MAX_SERVERS);
                    retval = -1;
                } else {
                    // new server entry
                    ++theServersNumber;
                    theServers[s].protocol = CrossTable[i].Protocol;
                    theServers[s].IPaddress = CrossTable[i].IPAddress;
                    theServers[s].port = CrossTable[i].Port;
                    switch (theServers[s].protocol) {
                    case PLC:
                    case RTU:
                    case TCP:
                    case TCPRTU:
                    case CANOPEN:
                    case MECT:
                        // FIXME: assert
                        break;
                    case RTU_SRV: {
                        u_int16_t port = CrossTable[i].Port;
                        switch (port) {
                        case 0:
                        case 1:
                        case 2:
                        case 3:
                            if (system_ini.serial_port[port].baudrate == 0) {
                                fprintf(stderr, "%s: missing port %u in system.ini for RTU_SRV variable #%u\n", __func__, port, i);
                                retval = -1;
                            } else {
                                theServers[s].u.serial.port = port;
                                theServers[s].u.serial.baudrate = system_ini.serial_port[port].baudrate;
                                theServers[s].u.serial.parity = system_ini.serial_port[port].parity;
                                theServers[s].u.serial.databits = system_ini.serial_port[port].databits;
                                theServers[s].u.serial.stopbits = system_ini.serial_port[port].stopbits;
                                theServers[s].silence_ms = system_ini.serial_port[port].silence_ms;
                                theServers[s].timeout_ms = system_ini.serial_port[port].timeout_ms;
                            }
                            break;
                        default:
                            fprintf(stderr, "%s: bad RTU_SRV port %u for variable #%u\n", __func__, port, i);
                            retval = -1;
                        }
                        theServers[s].ctx = NULL;
                    }   break;
                    case TCP_SRV:
                        theServers[s].u.tcp_ip.IPaddr = CrossTable[i].IPAddress;
                        theServers[s].u.tcp_ip.port = CrossTable[i].Port;
                        theServers[s].silence_ms = system_ini.tcp_ip_port.silence_ms;
                        theServers[s].timeout_ms = system_ini.tcp_ip_port.timeout_ms;
                        theServers[s].ctx = NULL;
                        break;
                    case TCPRTU_SRV:
                        theServers[s].u.tcp_ip.IPaddr = CrossTable[i].IPAddress;
                        theServers[s].u.tcp_ip.port = CrossTable[i].Port;
                        theServers[s].silence_ms = system_ini.tcp_ip_port.silence_ms;
                        theServers[s].timeout_ms = system_ini.tcp_ip_port.timeout_ms;
                        theServers[s].ctx = NULL;
                        break;
                    default:
                        ;
                    }
                    theServers[s].NodeId = CrossTable[i].NodeId;
                    theServers[s].thread_id = WRONG_THREAD;
                    theServers[s].status = SRV_RUNNING0;
                    theServers[s].thread_status = NOT_STARTED;
                    // theServers[s].serverMutex = PTHREAD_MUTEX_INITIALIZER;
                    snprintf(theServers[s].name, MAX_THREADNAME_LEN, "srv[%d]%s_0x%08x_%d", s, fieldbusName[theServers[s].protocol], theServers[s].IPaddress, theServers[s].port);
                    initServerDiagnostic(s);
                    theServers[s].idle_time_ns = 0;
                    theServers[s].busy_time_ns = 0;
                    theServers[s].last_time_ns = 0;
                    theServers[s].last_update_ns = 0;
                }
                break;
            default:
                break;
            }

            // client variables =---> link to the server and add unique devices and nodes
            switch (CrossTable[i].Protocol) {
            case PLC:
                // no plc client
                CrossTable[i].device = 0xffff;
                CrossTable[i].node = 0xffff;
                break;
            case RTU:
            case TCP:
            case TCPRTU:
            case CANOPEN:
            case MECT:
                // add unique variable's device (Protocol, IPAddress, Port, --)
                for (d = 0; d < theDevicesNumber; ++d) {
                    if (CrossTable[i].Protocol == theDevices[d].protocol
                     && CrossTable[i].IPAddress == theDevices[d].IPaddress
                     && CrossTable[i].Port == theDevices[d].port) {
                        // already present
                        break;
                    }
                }
                goto add_device;
                // no break
            case RTU_SRV:
                // add unique variable's device (Protocol, --, Port, --)
                for (d = 0; d < theDevicesNumber; ++d) {
                    if (CrossTable[i].Protocol == theDevices[d].protocol
                     && CrossTable[i].Port == theDevices[d].port) {
                        // already present
                        break;
                    }
                }
                goto add_device;
                // no break
            case TCP_SRV:
            case TCPRTU_SRV:
                // add unique variable's device (Protocol, --, --, --)
                for (d = 0; d < theDevicesNumber; ++d) {
                    if (CrossTable[i].Protocol == theDevices[d].protocol) {
                        // already present
                        break;
                    }
                }
            add_device:
                if (d < theDevicesNumber) {
                    CrossTable[i].device = d; // found
                    theDevices[d].var_num += 1; // this one, also Htype
                } else if (theDevicesNumber >= MAX_DEVICES) {
                    CrossTable[i].device = 0xffff; // FIXME: error
                    fprintf(stderr, "%s() too many devices (max=%d)\n", __func__, MAX_DEVICES);
                    retval = -1;
                } else {
                    // new device entry
                    CrossTable[i].device = theDevicesNumber;
                    ++theDevicesNumber;
                    theDevices[d].protocol = CrossTable[i].Protocol;
                    theDevices[d].IPaddress = CrossTable[i].IPAddress;
                    theDevices[d].port = p = CrossTable[i].Port;
                    theDevices[d].var_num = 1; // this one, also Htype
                    theDevices[d].device_vars = NULL; // calloc later on
                    theDevices[d].server = 0xffff;
                    switch (theDevices[d].protocol) {
                    case PLC:
                        // FIXME: assert
                        break;
                    case RTU:
                    case MECT:
                        switch (p) {
                        case 0:
                        case 1:
                        case 2:
                        case 3:
                            if (system_ini.serial_port[p].baudrate == 0) {
                                fprintf(stderr, "%s: missing port %u in system.ini for RTU variable #%u\n", __func__, p, i);
                                retval = -1;
                            } else {
                                theDevices[d].u.serial.port = p;
                                theDevices[d].u.serial.baudrate = system_ini.serial_port[p].baudrate;
                                theDevices[d].u.serial.parity = system_ini.serial_port[p].parity;
                                theDevices[d].u.serial.databits = system_ini.serial_port[p].databits;
                                theDevices[d].u.serial.stopbits = system_ini.serial_port[p].stopbits;
                                theDevices[d].silence_ms = system_ini.serial_port[p].silence_ms;
                                theDevices[d].timeout_ms = system_ini.serial_port[p].timeout_ms;
                                theDevices[d].max_block_size = system_ini.serial_port[p].max_block_size;
                            }
                            break;
                        default:
                            fprintf(stderr, "%s: bad %s port %u for variable #%u\n", __func__,
                                    (theDevices[d].protocol == RTU ? "RTU" : "MECT"), p, i);
                            retval = -1;
                        }
                        break;
                    case TCP:
                        ++theTcpDevicesNumber;
                        theDevices[d].u.tcp_ip.IPaddr = CrossTable[i].IPAddress;
                        theDevices[d].u.tcp_ip.port = CrossTable[i].Port;
                        theDevices[d].silence_ms = system_ini.tcp_ip_port.silence_ms;
                        theDevices[d].timeout_ms = system_ini.tcp_ip_port.timeout_ms;
                        theDevices[d].max_block_size = system_ini.tcp_ip_port.max_block_size;
                        break;
                    case TCPRTU:
                        theDevices[d].u.tcp_ip.IPaddr = CrossTable[i].IPAddress;
                        theDevices[d].u.tcp_ip.port = CrossTable[i].Port;
                        theDevices[d].silence_ms = system_ini.tcp_ip_port.silence_ms;
                        theDevices[d].timeout_ms = system_ini.tcp_ip_port.timeout_ms;
                        theDevices[d].max_block_size = system_ini.tcp_ip_port.max_block_size;
                        break;
                    case CANOPEN:
                        switch (p) {
                        case 0:
                        case 1:
                            theDevices[d].u.can.bus = p;
                            theDevices[d].u.can.baudrate = system_ini.canopen[p].baudrate;
                            theDevices[d].silence_ms = 0;
                            theDevices[d].timeout_ms = 0;
                            theDevices[d].max_block_size = system_ini.canopen[p].max_block_size;
                            break;
                        default:
                            fprintf(stderr, "%s: bad CANOPEN port %u for variable #%u", __func__, p, i);
                            retval = -1;
                        }
                        break;
                    case RTU_SRV:
                        theDevices[d].server = s; // searched before
                        theDevices[d].silence_ms = system_ini.serial_port[p].silence_ms;
                        theDevices[d].timeout_ms = system_ini.serial_port[p].timeout_ms;
                        theDevices[d].max_block_size = system_ini.serial_port[p].max_block_size;
                        break;
                    case TCP_SRV:
                    case TCPRTU_SRV:
                        theDevices[d].server = s; // searched before
                        theDevices[d].silence_ms = system_ini.tcp_ip_port.silence_ms;
                        theDevices[d].timeout_ms = system_ini.tcp_ip_port.timeout_ms;
                        theDevices[d].max_block_size = system_ini.tcp_ip_port.max_block_size;
                        break;
                    default:
                        ;
                    }
                    snprintf(theDevices[d].name, MAX_THREADNAME_LEN, "dev(%d)%s_0x%08x_%u", d, fieldbusName[theDevices[d].protocol], theDevices[d].IPaddress, theDevices[d].port);
                    if (theDevices[d].timeout_ms == 0 && theDevices[d].protocol == RTU) {
                        theDevices[d].timeout_ms = 300;
                        fprintf(stderr, "%s: TimeOut of device '%s' forced to %u ms\n", __func__, theDevices[d].name, theDevices[d].timeout_ms);
                    }
                    if (i == base && CrossTable[i].BlockSize > theDevices[d].max_block_size) {
                        fprintf(stderr, "%s: warning: variable #%u block #%u size %u, exceeding max_block_size %u (%s)\n",
                                __func__, i, block, CrossTable[i].BlockSize, theDevices[d].max_block_size, theDevices[d].name);
                    }
                    theDevices[d].elapsed_time_ns = 0;
                    theDevices[d].idle_time_ns = 0;
                    theDevices[d].busy_time_ns = 0;
                    theDevices[d].last_time_ns = 0;
                    theDevices[d].last_update_ns = 0;
                    theDevices[d].status = ZERO;
                    // theDevices[d].thread_id = 0;
                    theDevices[d].thread_status = NOT_STARTED;
                    sem_init(&newOperations[d], 0, 0);
                    theDevices[d].PLCwriteRequestNumber = 0;
                    theDevices[d].PLCwriteRequestGet = 0;
                    theDevices[d].PLCwriteRequestPut = 0;
                    // theDevices[d].modbus_ctx .last_good_ms, PLCwriteRequests, PLCwriteRequestNumber, PLCwriteRequestGet, PLCwriteRequestPut
                    initDeviceDiagnostic(d);
                }
                // add unique variable's node
                for (n = 0; n < theNodesNumber; ++n) {
                     if (CrossTable[i].device == theNodes[n].device) {
                         if (CrossTable[i].Protocol == RTU_SRV || CrossTable[i].Protocol == TCP_SRV  || CrossTable[i].Protocol == TCPRTU_SRV) {
                             // already present (any NodeId)
                             break;
                         } else if (CrossTable[i].NodeId == theNodes[n].NodeID) {
                             // already present
                             break;
                         }
                    }
                }
            //add_node:
                if (n < theNodesNumber) {
                    CrossTable[i].node = n; // found
                } else if (theNodesNumber >= MAX_NODES) {
                    CrossTable[i].node = 0xffff;
                    fprintf(stderr, "%s() too many nodes (max=%d)\n", __func__, MAX_NODES);
                    retval = -1;
                } else {
                    // new node entry
                    CrossTable[i].node = theNodesNumber;
                    ++theNodesNumber;
                    theNodes[n].device = CrossTable[i].device;
                    theNodes[n].NodeID = CrossTable[i].NodeId;
                    if (disable_all_nodes) {
                        theNodes[n].status = NODE_DISABLED;
                    } else {
                        theNodes[n].status = NODE_OK;
                    }
                    // theNodes[n].RetryCounter .JumpRead
                    initNodeDiagnostic(n);
                }
                break;
            default:
                break;
            }
        }
    }

  {
    u_int16_t d;
    u_int16_t var_max[MAX_DEVICES];

    for (i = 1; i <= DimCrossTable; ++i) {
        if (CrossTable[i].Enable > 0) {

            // client variables =---> create and fill the variables addresses array
            switch (CrossTable[i].Protocol) {
            case PLC:
                // no plc client
                break;
            case RTU:
            case TCP:
            case TCPRTU:
            case CANOPEN:
            case MECT:
            case RTU_SRV:
            case TCP_SRV:
            case TCPRTU_SRV:
                d = CrossTable[i].device;
                if (d != 0xffff) {
                    if (theDevices[d].device_vars == NULL) {
                        theDevices[d].device_vars = calloc(theDevices[d].var_num, sizeof(struct device_var));
                        var_max[d] = 0;
                    }
                    if (theDevices[d].device_vars == NULL) {
                        fprintf(stderr, "%s() memory full\n", __func__);
                        retval = -1;
                        break;
                    }
                    if (var_max[d] < theDevices[d].var_num) {
                        theDevices[d].device_vars[var_max[d]].addr = i;
                        if (CrossTable[i].Plc > Htype) {
                            theDevices[d].device_vars[var_max[d]].active = 1;
                        }
                        ++var_max[d];
                    }
                }
                break;
            default:
                break;
            }
        }
    }
  }
    return retval;
}

static void initNodeDiagnostic(u_int16_t n)
{
    u_int16_t addr = 0;
    u_int32_t value;

    addr = 5172 + 2 * n;
    theNodes[n].diagnosticAddr = addr;
    value = (theNodes[n].device << 16) + theNodes[n].NodeID;
#if 0
    writeQdataRegisters(addr + DIAGNOSTIC_DEV_NODE, value, DATA_OK);
    writeQdataRegisters(addr + DIAGNOSTIC_NODE_STATUS, theNodes[n].status, DATA_OK);
#else
    VAR_VALUE(addr + DIAGNOSTIC_DEV_NODE) = value;
    VAR_STATE(addr + DIAGNOSTIC_DEV_NODE) = DATA_OK;
    VAR_VALUE(addr + DIAGNOSTIC_NODE_STATUS) = theNodes[n].status;
    VAR_STATE(addr + DIAGNOSTIC_NODE_STATUS) = DATA_OK;
#endif
}

/* ---------------------------------------------------------------------------- */

static int newAlarmEvent(int isAlarm, u_int16_t addr, char *expr, size_t len)
{
    char *p, *r;
    (void)len;

    if (lastAlarmEvent >= DimAlarmsCT) {
        return -1;
    }
    ++lastAlarmEvent;
    ALCrossTable[lastAlarmEvent].ALType = (isAlarm ? Alarm : Event);
    ALCrossTable[lastAlarmEvent].TagAddr = addr;

    p = strtok_r(expr, " ]", &r);
    if (p == NULL) {
        goto exit_error;
    }
    strncpy(ALCrossTable[lastAlarmEvent].ALSource, p, MAX_IDNAME_LEN);

    p = strtok_r(NULL, " ]", &r);
    if (p == NULL) {
        goto exit_error;
    }
    if (strncmp(p, ">=", 2) == 0) { // before ">" !!!
        ALCrossTable[lastAlarmEvent].ALOperator = OPER_GREATER_EQ;
    } else if (strncmp(p, ">", 1) == 0) {
        ALCrossTable[lastAlarmEvent].ALOperator = OPER_GREATER;
    } else if (strncmp(p, "<=", 2) == 0) { // before "<" !!!
        ALCrossTable[lastAlarmEvent].ALOperator = OPER_SMALLER_EQ;
    } else if (strncmp(p, "<", 1) == 0) {
        ALCrossTable[lastAlarmEvent].ALOperator = OPER_SMALLER;
    } else if (strncmp(p, "==", 2) == 0) {
        ALCrossTable[lastAlarmEvent].ALOperator = OPER_EQUAL;
    } else if (strncmp(p, "!=", 2) == 0) {
        ALCrossTable[lastAlarmEvent].ALOperator = OPER_NOT_EQUAL;
    } else if (strncmp(p, "RISING", 6) == 0) {
        ALCrossTable[lastAlarmEvent].ALOperator = OPER_RISING;
    } else if (strncmp(p, "FALLING", 7) == 0) {
        ALCrossTable[lastAlarmEvent].ALOperator = OPER_FALLING;
    } else {
        goto exit_error;
    }

    if (ALCrossTable[lastAlarmEvent].ALOperator != OPER_FALLING && ALCrossTable[lastAlarmEvent].ALOperator != OPER_RISING) {
        char *s;
        float f;

        p = strtok_r(NULL, "]", &r);
        if (p == NULL) {
            goto exit_error;
        }
        f = strtof(p, &s);
        if (s == p) {
            // identifier (check later on)
            strncpy(ALCrossTable[lastAlarmEvent].ALCompareVar, p, MAX_IDNAME_LEN);
        } else {
            // number
            ALCrossTable[lastAlarmEvent].ALCompareVar[0] = 0;
            ALCrossTable[lastAlarmEvent].ALCompareVal.f = f;
        }
    }
    return 0;

exit_error:
    --lastAlarmEvent;
    return -1;
}

static inline void setAlarmEvent(int i)
{
    u_int16_t addr = ALCrossTable[i].TagAddr;

    if (VAR_VALUE(addr) == 0) {
        // set alarm and call tasks only if currently clear
        writeQdataRegisters(addr, 1, DATA_OK);
        if (ALCrossTable[i].ALType == Alarm) {
            vmmSetEvent(pVMM, EVT_RESERVED_11); // 27u --> EVENT:='28'
        } else {
            vmmSetEvent(pVMM, EVT_RESERVED_10); // 26u --> EVENT:='27'
        }
    }
}

static inline void clearAlarmEvent(int i)
{
    writeQdataRegisters(ALCrossTable[i].TagAddr, 0, DATA_OK);
    ALCrossTable[i].ALFilterCount = ALCrossTable[i].ALFilterTime;
}

static inline void checkAlarmEvent(int i, int condition)
{
    if (condition) {
        if (ALCrossTable[i].ALFilterCount == 0) {
            // setting alarm/event
            setAlarmEvent(i);
        } else {
            // filtering alarm/event
            ALCrossTable[i].ALFilterCount = ALCrossTable[i].ALFilterCount - 1;
        }
    } else {
        // clearing alarm/event
        clearAlarmEvent(i);
    }
}

static void AlarmMngr(void)
{
    register u_int16_t i;

    // already in pthread_mutex_lock(&theCrosstableClientMutex)
    for (i = 1; i <= lastAlarmEvent; ++i) {

        register u_int16_t SourceAddr = ALCrossTable[i].SourceAddr;
        register u_int16_t Operator = ALCrossTable[i].ALOperator;

        if (VAR_STATE(SourceAddr) != DATA_OK) {
            // unreliable values
            continue;
        }

        if (Operator == OPER_RISING || Operator == OPER_FALLING) {
            // checking against old value
            register int32_t SourceValue = VAR_VALUE(SourceAddr);
            register int32_t CompareVal = CrossTable[i].OldVal;

            if (Operator == OPER_RISING) {
                if (CompareVal == 0) {
                    // checking rising edge if currently low
                    checkAlarmEvent(i, SourceValue != 0);
                } else if (SourceValue == 0){
                    // clearing alarm/event only at falling edge
                    clearAlarmEvent(i);
                }
            } else if (Operator == OPER_FALLING)  {
                if (CompareVal != 0) {
                    // checking falling edge if currently high
                    checkAlarmEvent(i, SourceValue == 0);
                } else if (SourceValue != 0){
                    // clearing alarm/event only at rising edge
                    clearAlarmEvent(i);
                }
            }
            // saving the new "old value" :)
            CrossTable[i].OldVal = SourceValue;

        } else {
            register u_int16_t CompareAddr = ALCrossTable[i].CompareAddr;
            varUnion SourceValue, CompareVal;

            SourceValue = plcBlock.values[SourceAddr]; // VAR_VALUE(SourceAddr);

            // checking either against fixed value or against variable value
            if (CompareAddr == 0) {
                // fixed value
                CompareVal = ALCrossTable[i].ALCompareVal;
            } else if (VAR_STATE(CompareAddr) != DATA_OK) {
                // unreliable values
                continue;
            } else {
                CompareVal = plcBlock.values[CompareAddr]; // VAR_VALUE(CompareAddr);
                // FIXME: align decimals and types
            }

            // comparison types
            switch (ALCrossTable[i].comparison) {
            case COMP_UNSIGNED:
                switch (Operator) {
                case OPER_EQUAL     : checkAlarmEvent(i, SourceValue.u32 == CompareVal.u32); break;
                case OPER_NOT_EQUAL : checkAlarmEvent(i, SourceValue.u32 != CompareVal.u32); break;
                case OPER_GREATER   : checkAlarmEvent(i, SourceValue.u32 >  CompareVal.u32); break;
                case OPER_GREATER_EQ: checkAlarmEvent(i, SourceValue.u32 >= CompareVal.u32); break;
                case OPER_SMALLER   : checkAlarmEvent(i, SourceValue.u32 <  CompareVal.u32); break;
                case OPER_SMALLER_EQ: checkAlarmEvent(i, SourceValue.u32 <= CompareVal.u32); break;
                default             : ;
                }
                break;
            case COMP_SIGNED16:
                switch (Operator) {
                case OPER_EQUAL     : checkAlarmEvent(i, SourceValue.i16 == CompareVal.i16); break;
                case OPER_NOT_EQUAL : checkAlarmEvent(i, SourceValue.i16 != CompareVal.i16); break;
                case OPER_GREATER   : checkAlarmEvent(i, SourceValue.i16 >  CompareVal.i16); break;
                case OPER_GREATER_EQ: checkAlarmEvent(i, SourceValue.i16 >= CompareVal.i16); break;
                case OPER_SMALLER   : checkAlarmEvent(i, SourceValue.i16 <  CompareVal.i16); break;
                case OPER_SMALLER_EQ: checkAlarmEvent(i, SourceValue.i16 <= CompareVal.i16); break;
                default             : ;
                }
                break;
            case COMP_SIGNED32:
                switch (Operator) {
                case OPER_EQUAL     : checkAlarmEvent(i, SourceValue.i32 == CompareVal.i32); break;
                case OPER_NOT_EQUAL : checkAlarmEvent(i, SourceValue.i32 != CompareVal.i32); break;
                case OPER_GREATER   : checkAlarmEvent(i, SourceValue.i32 >  CompareVal.i32); break;
                case OPER_GREATER_EQ: checkAlarmEvent(i, SourceValue.i32 >= CompareVal.i32); break;
                case OPER_SMALLER   : checkAlarmEvent(i, SourceValue.i32 <  CompareVal.i32); break;
                case OPER_SMALLER_EQ: checkAlarmEvent(i, SourceValue.i32 <= CompareVal.i32); break;
                default             : ;
                }
                break;
            case COMP_FLOATING:
                switch (Operator) {
                case OPER_EQUAL     : checkAlarmEvent(i, SourceValue.f == CompareVal.f); break;
                case OPER_NOT_EQUAL : checkAlarmEvent(i, SourceValue.f != CompareVal.f); break;
                case OPER_GREATER   : checkAlarmEvent(i, SourceValue.f >  CompareVal.f); break;
                case OPER_GREATER_EQ: checkAlarmEvent(i, SourceValue.f >= CompareVal.f); break;
                case OPER_SMALLER   : checkAlarmEvent(i, SourceValue.f <  CompareVal.f); break;
                case OPER_SMALLER_EQ: checkAlarmEvent(i, SourceValue.f <= CompareVal.f); break;
                default             : ;
                }
                break;
            default:
                ;
            }
        }
    }
}

/* ---------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------------- */

static unsigned plc_product_id()
{
    unsigned retval = 0xFFFFffff;
    FILE *f;

    f = fopen(ROOTFS_VERSION, "r");
    if (f) {
        char buf[42];
        char str[42];
        unsigned x = 0, y = 0, z = 0;

        if (fgets(buf, 42, f) == NULL)
            goto close_file;
        if (sscanf(buf, "Release: %5s", str) != 1)
            goto close_file;
        if (fgets(buf, 42, f) == NULL)
            goto close_file;

        // the order of following tests is important

        // TP1043_01_A TP1043_01_B TP1043_02_A TP1043_02_B
        // TP1057_01_A TP1057_01_B
        // TP1070_01_A TP1070_01_B TP1070_01_C
        // TP1070_02_E
        if (sscanf(buf, "Target: TP%x_%x_%x", &x, &y, &z) == 3)
            retval = ((x & 0xFFFF) << 16) + ((y & 0xFF) << 8) + (z & 0xFF);

        // TPX1070_03_D TPX1070_03_E
        else if (sscanf(buf, "Target: TPX%x_%x_%x", &x, &y, &z) == 3)
            retval = ((x & 0xFFFF) << 16) + ((y & 0xFF) << 8) + (z & 0xFF);

        // TPAC1007_04_AA TPAC1007_04_AB TPAC1007_04_AC TPAC1007_04_AD TPAC1007_04_AE
        // TPAC1008_02_AA TPAC1008_02_AB TPAC1008_02_AD TPAC1008_02_AE TPAC1008_02_AF
        // TPAC1008_03_AC TPAC1008_03_AD
        else if (sscanf(buf, "Target: TPAC%x_%x_%x", &x, &y, &z) == 3)
            retval = ((x & 0xFFFF) << 16) + ((y & 0xFF) << 8) + (z & 0xFF);

        // TPAC1007_03 TPAC1008_01
        else if (sscanf(buf, "Target: TPAC%x_%x", &x, &y) == 2)
            retval = ((x & 0xFFFF) << 16) + ((y & 0xFF) << 8);

        // TPAC1007_LV
        else if (strcmp(buf, "Target: TPAC1007_LV") == 0)
            retval = 0x10075500;

        // TPAC1005 TPAC1006
        else if (sscanf(buf, "Target: TPAC%x", &x) == 1)
            retval = ((x & 0xFFFF) << 16);

        // TPLC050_01_AA TPLC100_01_AA TPLC100_01_AB
        else if (sscanf(buf, "Target: TPLC%x_%x_%x", &x, &y, &z) == 3)
            retval = ((x & 0xFFFF) << 16) + ((y & 0xFF) << 8) + (z & 0xFF);

    close_file:
        fclose(f);
    }

    return retval;
}

static unsigned plc_serial_number()
{
    unsigned retval = 0xFFFFffff;
    FILE *f;

    f = fopen(SERIAL_CONF, "r");
    if (f) {
        char buf[80]; // YYYYMM1234

        if (fgets(buf, (4+2+4+1), f)) {
            retval = strtoul(buf, NULL, 10);
        }
        fclose(f);
    }

    return retval;
}
