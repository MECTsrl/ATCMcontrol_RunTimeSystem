/*
 * Copyright 2011 Mect s.r.l.
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
 * Filename: main.c (ex fcrts/fcMain.c)
 */

/* ----  Local Defines:   ----------------------------------------------------- */

#define __4CFILE__ "fcMain.c"

/* ----  Includes:	 ---------------------------------------------------------- */

#include "inc/stdInc.h"

#include "inc.fc/fcDef.h"

#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <signal.h>
#include <fcntl.h>
#include <sched.h>
#include <unistd.h>
#include <ucontext.h>
#if defined(RTS_CFG_MECT_RETAIN)

/*
 * FIXME: remove old style mect retentives
 * from #include <linux/pwrsignal.h>
 * i.e. imx_mect/ltib/rootfs/usr/src/linux/include/linux/pwrsignal.h
 */
#define SIGPWRFAIL 44
#define PID_FILE "signalconfpid"
#define PID_PATH "/sys/kernel/debug/signalconfpid"

#include <sys/ioctl.h>
#endif
#include <getopt.h>
#include <sys/reboot.h> // and not <linux/reboot.h>

#include "inc.data/dataMain.h" // dataEngineStop()

/* ----  Local Defines:   ----------------------------------------------------- */
#define MSR_EE			(1<<15) 		/* External Interrupt Enable		*/
#define MSR_PR			(1<<14) 		/* Problem State / Privilege Level	*/
#define MSR_FP			(1<<13) 		/* Floating Point enable			*/
#define MSR_ME			(1<<12) 		/* Machine Check Enable 			*/
#define MSR_IR			(1<<5)			/* Instruction Relocate 			*/
#define MSR_DR			(1<<4)			/* Data Relocate					*/

#define LOG_FILE_1		"/local/root/crash_trace"
#define LOG_FILE_2		"/dev/console"

//#define DBG_MAIN
//#define MECT_RETAIN_DEBUG

/* ----  Global Variables:	 -------------------------------------------------- */

volatile sig_atomic_t term_handler_active	= 0;
volatile sig_atomic_t crash_handler_active	= 0;
volatile sig_atomic_t log_ignore_crash		= 0;

static char *app_name = "FarosPLC"; // NULL;

int configfd;

/* ----  Local Functions:	--------------------------------------------------- */

void termination_handler(int signum);
void crash_handler(int signum, siginfo_t *siginfo, void *context);
void pwrfail_handler(int signum, siginfo_t *siginfo, void *context);

static char *get_signal(int signum);

IEC_UINT writepid(void);

void ReleaseResources(void);

/* ----  Implementations:	--------------------------------------------------- */

/* ---------------------------------------------------------------------------- */

/* Long options */
static struct option long_options[] = {
    {"version",  no_argument,        NULL, 'v'},
    {"xx_gpio",  no_argument,        NULL, 'x'},
    {"print",    no_argument,        NULL, 'p'},
    {"overflow", no_argument,        NULL, 'o'},
    {NULL,       no_argument,        NULL,  0}
};

/*
 * Short options.
 * FIXME: KEEP THEIR LETTERS IN SYNC WITH THE RETURN VALUE
 * FROM THE LONG OPTIONS!
 */
static char short_options[] = "vxpo";

static int application_options(int argc, char *argv[])
{
    int option_index = 0;
    int c = 0, n;
    char version[VMM_MAX_IEC_STRLEN];

    if (argc <= 0)
        return 0;

    if (argv == NULL)
        return 1;

    while ((c = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1) {
        switch (c) {
            case 'v':
#if 0
                if (sysGetVersionInfo(version) != OK) {
                    sprintf(version, "(unknown)");
                }
#else
                dataGetVersionInfo(version);
#endif
                printf("%s version: %s\n", argv[0], version);
#if defined(RTS_CFG_DEBUG_GPIO)
                printf("\tXX_GPIO enabled\n");
#endif

                exit(0);

            case 'x':
                fprintf(stderr, "xx_gpio testing:\n");
                XX_GPIO_INIT();
                fprintf(stderr, "    XX_GPIO_CONFIG(");
                for (n = 0; n < XX_GPIO_MAX_TEST; ++n) {
                    fprintf(stderr, "%02d, 1", n);
                    XX_GPIO_ENABLE(n);
                    XX_GPIO_CONFIG(n, 1);
                    fprintf(stderr, ")\n");
                }
                fprintf(stderr, ")\n");
                while (1) {
                    int n;
                    fprintf(stderr, "    XX_GPIO_SET(");
                    for (n = 0; n < XX_GPIO_MAX_TEST; ++n) {
                        fprintf(stderr, "%02d ", n);
                        XX_GPIO_SET(n);
                        XX_GPIO_CLR(n);
                        XX_GPIO_SET(n);
                    }
                    fprintf(stderr, "), XX_GPIO_CLR(");
                    for (n = 0; n < XX_GPIO_MAX_TEST; ++n) {
                        fprintf(stderr, "%02d ", n);
                        XX_GPIO_CLR(n);
                        XX_GPIO_SET(n);
                        XX_GPIO_CLR(n);
                    }
                    fprintf(stderr, ")\n");
                    sleep(1);
                }
                XX_GPIO_CLOSE();
                exit(0);

        case 'p':
            printf("enabling verbose printing\n");
            dataEnableVerbosePrint();
            break;

        case 'o':
            printf("enabling timer overflow simulation\n");
            dataEnableTimerOverflow();
            break;

            default:
                break;
        }
    }

    return 0;
}

/**
 * main
 */
int main(int argc, char *argv[])
{
    IEC_UINT uRes;

#ifdef __XENO__
    printf("Xenomai enabled\n");
    struct rlimit rlimit;
    int retval;
    rlimit.rlim_cur = rlimit.rlim_max = 128 * 1024;
    retval = setrlimit(RLIMIT_STACK, &rlimit);
#else
    struct rlimit rlimit;
    int retval;
    rlimit.rlim_cur = rlimit.rlim_max = 1024 * 1024;
    retval = setrlimit(RLIMIT_STACK, &rlimit);
    if (retval) {
        perror("setrlimit");
        return 1;
    }
#endif
    mlockall(MCL_CURRENT | MCL_FUTURE);

    if (application_options(argc, argv) != 0) {
        fprintf(stderr, "%s: command line option error.\n", __func__);

        return 1;
    }

    // app_name = argv[0];

    /* Enable Core Dumps
     * ------------------------------------------------------------------------
     */
    if (FALSE)
    {
        struct rlimit infinit = {RLIM_INFINITY, RLIM_INFINITY};
        struct rlimit curr;

        if (getrlimit(RLIMIT_CORE, &curr) == -1)
        {
            fprintf(stdout, "getrlimit failed (reason:%d (%s))\r\n", os_errno, OS_STRERROR);
        }

        if (setrlimit(RLIMIT_CORE, &infinit) == -1)
        {
            fprintf(stdout, "setrlimit failed (reason:%d (%s))\r\n", os_errno, OS_STRERROR);
        }
    }

    /* number of  processes  and  threads for  a real user ID */
    struct rlimit limit;

    if (getrlimit(RLIMIT_NPROC, &limit) == -1) {
        fprintf(stdout, "getrlimit failed (reason:%d (%s))\r\n", os_errno, OS_STRERROR);
    } else {
        fprintf(stdout, "getrlimit RLIMIT_NPROC: cur=%lu max=%lu\n", limit.rlim_cur, limit.rlim_max);
    }

    struct sigaction new_action;
    struct sigaction old_action;

    new_action.sa_flags = 0;
    if (sigemptyset(&new_action.sa_mask) == -1)
    {
        fprintf(stdout, "sigemptyset failed (reason:%d (%s))\r\n", os_errno, OS_STRERROR);
    }


    /* Disable SIGHUP
     * ------------------------------------------------------------------------
     * Ignore detaching of a possible connected user's terminal.
     */
    new_action.sa_handler = SIG_IGN;
    if (sigaction(SIGHUP, &new_action, NULL) == -1)
    {
        fprintf(stdout, "sigaction (SIGHUP) failed (reason:%d (%s))\r\n", os_errno, OS_STRERROR);
    }


    /* Install Termination Handler
     * ------------------------------------------------------------------------
     */
    new_action.sa_handler = termination_handler;

    if (sigaction(SIGINT, NULL, &old_action) == -1)
    {
        fprintf(stdout, "sigaction (SIGINT) failed (reason:%d (%s))\r\n", os_errno, OS_STRERROR);
    }
    if (old_action.sa_handler != SIG_IGN)
    {
        if (sigaction(SIGINT, &new_action, NULL) == -1)
        {
            fprintf(stdout, "sigaction (SIGINT) failed (reason:%d (%s))\r\n", os_errno, OS_STRERROR);
        }
    }

    if (sigaction(SIGTERM, NULL, &old_action) == -1)
    {
        fprintf(stdout, "sigaction (SIGTERM) failed (reason:%d (%s))\r\n", os_errno, OS_STRERROR);
    }
    if (old_action.sa_handler != SIG_IGN)
    {
        if (sigaction(SIGTERM, &new_action, NULL) == -1)
        {
            fprintf(stdout, "sigaction (SIGTERM) failed (reason:%d (%s))\r\n", os_errno, OS_STRERROR);
        }
    }


    /* Install Crash Handler
     * ------------------------------------------------------------------------
     */
    new_action.sa_sigaction = crash_handler;

    if (sigemptyset(&new_action.sa_mask) == -1)
    {
        fprintf(stdout, "sigemptyset failed (reason:%d (%s))\r\n", os_errno, OS_STRERROR);
    }

    new_action.sa_flags = SA_SIGINFO | SA_NOMASK;

    if (sigaction(SIGILL,  &new_action, NULL) == -1)
    {
        fprintf(stdout, "sigaction (SIGILL) failed (reason:%d (%s))\r\n", os_errno, OS_STRERROR);
    }

    if (sigaction(SIGFPE,  &new_action, NULL) == -1)
    {
        fprintf(stdout, "sigaction (SIGFPE) failed (reason:%d (%s))\r\n", os_errno, OS_STRERROR);
    }

    if (sigaction(SIGSEGV, &new_action, NULL) == -1)
    {
        fprintf(stdout, "sigaction (SIGSEGV) failed (reason:%d (%s))\r\n", os_errno, OS_STRERROR);
    }

    if (sigaction(SIGBUS,  &new_action, NULL) == -1)
    {
        fprintf(stdout, "sigaction (SIGBUS) failed (reason:%d (%s))\r\n", os_errno, OS_STRERROR);
    }

    if (sigaction(SIGTRAP, &new_action, NULL) == -1)
    {
        fprintf(stdout, "sigaction (SIGTRAP) failed (reason:%d (%s))\r\n", os_errno, OS_STRERROR);
    }
#if defined(RTS_CFG_MECT_RETAIN)
    /* Install Dump Retentive Handler
     * ------------------------------------------------------------------------
     */
    new_action.sa_sigaction = pwrfail_handler;
    if (sigaction(SIGPWRFAIL, &new_action, NULL) == -1)
    {
        fprintf(stdout, "sigaction (SIGPWRFAIL) failed (reason:%d (%s))\r\n", os_errno, OS_STRERROR);
    }

    /* kernel needs to know our pid to be able to send us a signal ->
     * we use debugfs for this -> do not forget to mount the debugfs!
     */
#ifdef __XENO__
    writepid();
#endif

#endif

    /* Set Scheduling Parameters
     * ------------------------------------------------------------------------
     */
  #if ! defined(_POSIX_PRIORITY_SCHEDULING)
    #error Posix scheduling not defined in actual Linux kernel
  #endif
    // osPthreadSetSched(FC_SCHED_VMM, FC_PRIO_VMM) in vmKernel/vmmMain.c

    /* Initialize Jiffies
     * ------------------------------------------------------------------------
     */
  #if defined(_SOF_4CFC_SRC_)
    uRes = fcInitJiffies();
    if (uRes != OK)
    {
        RETURN(uRes);
    }
  #endif


    /* Run Main Loop
     * ------------------------------------------------------------------------
     */
    uRes = osMain(argc, argv);

    exit(uRes == OK ? EXIT_SUCCESS : EXIT_FAILURE);
}


/*------------------------------------------------------------------*/
/**
 * fcSetSystemWatchdog
 *
 * Sets the watchdog to the value of MFP_WATCHDOG_TIME
 *
 * @return		OK if successful else error number
 */
IEC_UINT fcSetSystemWatchdog()
{
  #if defined(_SOF_4CFC_SRC_)

    int hWatchdog = open(FC_WATCHDOG_DEVICE, O_WRONLY, 0);
    if (hWatchdog == -1)
    {
        RETURN(ERR_ERROR);
    }

    IEC_DINT  iWatchdogValue = FC_WATCHDOG_TIME;
    IEC_UDINT dwWrite = write(hWatchdog, (IEC_CHAR *)&iWatchdogValue, sizeof(iWatchdogValue));

    close(hWatchdog);

    if (dwWrite != sizeof(iWatchdogValue))
    {
        RETURN(ERR_ERROR);
    }
  #endif

    RETURN(OK);
}


/*------------------------------------------------------------------*/
/**
 * ByeBye
 *
 */
void ByeBye(void)
{
    char *sz;
    srand(osGetTime32());

    switch (rand() % 14)
    {
    case 0: sz = "Tschuess!";		break;
    case 1: sz = "Bye-Bye!";		break;
    case 2: sz = "Cheerio!";		break;
    case 3: sz = "Cheers!"; 		break;
    case 4: sz = "Auf Wiedersehen!";break;
    case 5: sz = "See You!";		break;
    case 6: sz = "Ade!";			break;
    case 7: sz = "Salut!";			break;
    case 8: sz = "Au Revoir!";		break;
    case 9: sz = "Servus!"; 		break;
    case 10:sz = "Ciao!";			break;
    case 11:sz = "Adieu!";			break;
    case 12:sz = "Pfiat Di!";		break;
    case 13:sz = "Gruezi!"; 		break;
    default:sz = "Servus!"; 		break;
    }

    printf("\r\n%s\r\n", sz);
}


/* ---------------------------------------------------------------------------- */
/**
 * termination_handler
 *
 */
void termination_handler(int signum)
{
    struct sigaction new_action;

#ifdef DBG_MAIN
    printf("%s!\n", __func__);
#endif
    /* Avoid recursive call of this handler
     */
    if (term_handler_active == 0) {
        term_handler_active = 1;

         ReleaseResources();

        //ByeBye();
    }

    /* Forward the signal
     */
    new_action.sa_flags = 0;
    sigemptyset (&new_action.sa_mask);
    new_action.sa_handler = SIG_DFL;

    sigaction(signum, &new_action, NULL);
    raise(signum);
}


/* ---------------------------------------------------------------------------- */
/**
 * log_ignore_crash_handler
 *
 */
void log_ignore_crash_handler(int signum, siginfo_t *siginfo, void *context)
{
    if (log_ignore_crash == 1)
    {
        log_ignore_crash = 0;

      #if defined(_SOF_4CFC_SRC_)
        ucontext_t *uc = (ucontext_t *)context;
        struct pt_regs *regs = uc->uc_mcontext.regs;
        regs->nip += 4;
      #else
        /* TODO fuer PC & DC
         */

      #endif
    }
    else
    {
        crash_handler(signum, siginfo, context);
    }
}


/* ---------------------------------------------------------------------------- */
/**
 * crash_handler
 *
 */
void crash_handler(int signum, siginfo_t *siginfo, void *context)
{
  #if defined(_SOF_4CFC_SRC_)
    ucontext_t *uc = (ucontext_t *)context;
    struct pt_regs *regs = uc->uc_mcontext.regs;
    int i;
    unsigned long *sp;
    unsigned long frame;
    struct sigaction sa;
  #endif

    char buf[256];
    int fd,len;

    struct timeval	tv;
    struct tm		tm;

    (void)context;

    fd = open(LOG_FILE_1, O_WRONLY|O_APPEND|O_CREAT, S_IRUSR|S_IWUSR);
    if (fd < 0)
    {
        fd = open(LOG_FILE_2, O_WRONLY|O_NOCTTY);
    }

    gettimeofday(&tv, NULL);
    localtime_r(&tv.tv_sec, &tm);

    len = sprintf(buf, "%04d/%02d/%02d %02d:%02d:%02d.%03d: ",
                       tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday,
                       tm.tm_hour,tm.tm_min,tm.tm_sec,(int)(tv.tv_usec/1000));

    len += sprintf(buf+len, "User Mode Exception - Signal: %s\n", get_signal(signum));
    if (write(fd,buf,len)) {}

  #if defined (RTS_CFG_SYSLOAD)

    IEC_UINT uTask	= 0xffffu;
    IEC_UINT uRes	= ldFindTask(getpid(), &uTask);

    char szTask[100];

    if (uRes == OK && uTask != 0xffffu)
    {
        uRes = ldGetTaskName(uTask, szTask);
    }

    if (uRes == OK && uTask != 0xffffu)
    {
        len = sprintf(buf, "APP: %s, TASK: %d(%s), ERRNO: %d, CODE: %08X\n",
                           app_name, getpid(), szTask, siginfo->si_errno, siginfo->si_code);
    }
    else
    {
        len = sprintf(buf, "APP: %s, TASK: %d, ERRNO: %d, CODE: %08X\n",
                           app_name, getpid(), siginfo->si_errno, siginfo->si_code);
    }
  #else
    len = sprintf(buf, "APP: %s, TASK: %d, ERRNO: %d, CODE: %08X\n",
                       app_name, getpid(), siginfo->si_errno, siginfo->si_code);
  #endif
    if (write(fd,buf,len)) {}

    len = sprintf(buf, "--------------------------------------------------------------------------\n");
    if (write(fd,buf,len)) {}

  #if defined(_SOF_4CFC_SRC_)
    len = sprintf(buf, "NIP: %08lX CTR: %08lX LR: %08lX SP: %08lX REGS: %08lX TRAP: %04lX\n",
                  regs->nip, regs->ctr, regs->link, regs->gpr[1],
                  (unsigned long)regs, regs->trap);
    write(fd,buf,len);

    len = sprintf(buf, "CCR: %08lX XER: %08lX MSR: %08lX EE: %01x PR: %01x FP: %01x ME: %01x IR/DR: %01x%01x\n",
                  regs->ccr, regs->xer, regs->msr,
                  regs->msr&MSR_EE ? 1 : 0, regs->msr&MSR_PR ? 1 : 0,
                  regs->msr&MSR_FP ? 1 : 0, regs->msr&MSR_ME ? 1 : 0,
                  regs->msr&MSR_IR ? 1 : 0, regs->msr&MSR_DR ? 1 : 0);
    write(fd,buf,len);

    if (regs->trap == 0x300 || regs->trap == 0x600)
    {
        len = sprintf(buf, "DAR: %08lX, DSISR: %08lX\n", regs->dar, regs->dsisr);
        write(fd,buf,len);
    }

    for (i = 0;  i < 32;  i++)
    {
        if ((i % 8) == 0)
        {
            len = sprintf(buf, "GPR%02d: ", i);
        }

        len += sprintf(buf+len, "%08lX ", regs->gpr[i]);
        if ((i % 8) == 7)
        {
            len += sprintf(buf+len, "\n");
            write(fd,buf,len);
        }
    }

    len = sprintf(buf, "Stack: ");
    write(fd,buf,len);

    sp = (unsigned long*)regs->gpr[1];
    i = 0;
    len = 0;

    log_ignore_crash = 0;
    sa.sa_sigaction = log_ignore_crash_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGSEGV, &sa, NULL);

    int bFirst = 1;

    while (sp)
    {
        log_ignore_crash = 1;
        frame = sp[1];
        if (--log_ignore_crash)
            break;

        if (i++ % 7 == 0 && bFirst == 0)
        {
            len += sprintf(buf+len, "\n");
            write(fd,buf,len);
            len = 0;
        }

        bFirst = 0;

        len += sprintf(buf+len, "%08lX ", frame);
        if (i > 32)
            break;

        log_ignore_crash = 1;
        sp = *(unsigned long**)sp;
        if (--log_ignore_crash)
            break;
    }

  #endif	/* _SOF_4CFC_SRC_ */

    len += sprintf(buf+len, "\n\n");
    if (write(fd,buf,len)) {}
    close(fd);
    sync();

    /* Forward the signal
     */

    if (crash_handler_active != 0)
    {
        raise(signum);
    }

    crash_handler_active = 1;

    struct sigaction new_action;

    new_action.sa_flags = 0;
    sigemptyset (&new_action.sa_mask);
    new_action.sa_handler = SIG_DFL;

    sigaction(signum, &new_action, NULL);
    raise(signum);
}

#if defined(RTS_CFG_MECT_RETAIN)
/**
 * power fail handler
 *
 */

void pwrfail_handler(int signum, siginfo_t *siginfo, void *context)
{
    int n;
    (void)signum;
    (void)siginfo;
    (void)context;

    // immediately block the engine and sync the retentive file
    dataEnginePwrFailStop();

    // in case of power hole we will have the chance of rebooting
    for (n = 0; n < 500000000; ++n)
        ;
    reboot(RB_POWER_OFF); // actually a reset on i.MX28, maybe also RB_AUTOBOOT but not RB_HALT_SYSTEM

    // unreachable code
    fputs("pwrfail_handler: ... reboot() returned ????\n", stderr);
    while (1)
        ;
}

/* kernel needs to know our pid to be able to send us a signal ->
 * we use debugfs for this -> do not forget to mount the debugfs!
 */
IEC_UINT writepid(void)
{
    char buf[10];

    configfd = open(PID_PATH, O_CREAT | O_WRONLY, S_IRUSR|S_IWUSR);
    if (configfd < 0) {
        fprintf(stderr, "open '%s' failed (reason:%d (%s))\r\n", PID_FILE, os_errno, OS_STRERROR);
        RETURN(ERR_ERROR);
    }
    sprintf(buf, "%i", getpid());
    if (write(configfd, buf, strlen(buf) + 1) < 0) {
        fprintf(stderr, "write '%s' failed (reason:%d (%s))\r\n", buf, os_errno, OS_STRERROR);
        RETURN(ERR_ERROR);
    }
    RETURN(OK);
}
#endif

/* ---------------------------------------------------------------------------- */
/**
 * get_signal
 *
 */
static char *get_signal(int signum)
{
    switch (signum)
    {
        case SIGILL:  return "SIGILL";
        case SIGFPE:  return "SIGFPE";
        case SIGSEGV: return "SIGSEGV";
        case SIGBUS:  return "SIGBUS";
        case SIGTRAP: return "SIGTRAP";
#if defined(RTS_CFG_MECT_RETAIN)
        case SIGPWRFAIL: return "SIGPWRFAIL";
#endif
        case SIGINT:  return "SIGINT";
        default :
        {
            static char szDummy[100];
            sprintf(szDummy, "%d", signum);
            return szDummy;
        }
    }
}

void ReleaseResources(void)
{
#ifdef DBG_MAIN
    fprintf(stdout, "[%s] - Release resources...\n", __func__);
#endif

    dataEngineStop();

#ifdef DBG_MAIN
    fprintf(stdout, "[%s] - done.\n", __func__);
#endif
}
/* ---------------------------------------------------------------------------- */

