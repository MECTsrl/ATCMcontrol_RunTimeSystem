TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

DEFINES += _SOF_4CPC_SRC_

#DEFINES += TP1000
#DEFINES += TP2000
DEFINES += TP3000


#QMAKE_CFLAGS_DEBUG -= -O2
#QMAKE_CFLAGS_DEBUG += -O0

#QMAKE_CFLAGS_RELEASE -= -O2
#QMAKE_CFLAGS_RELEASE += -O3

LIBS += -pthread -lrt

TARGET = fcrts
target.path = /local/root # path on device
INSTALLS += target

SOURCES += \
        CANopen/CANopen.c \
        ioData/crosstable.c \
        ioData/dataClients.c \
        ioData/dataDiagnostics.c \
        ioData/dataEngine.c \
        ioData/dataFbMect.c \
        ioData/dataHmiPlc.c \
        ioData/dataImpl.c \
        ioData/dataMain.c \
        ioData/dataRetentives.c \
        ioData/dataServers.c \
        ioData/dataUtils.c \
        ioData/hmi_plc.c \
        ioData/system_ini.c \
        ioTest/tstImpl.c \
        ioTest/tstMain.c \
        ioTest/tstSimu.c \
        main.c \
        osKernel/fcAdapt.c \
        osKernel/mectAdapt.c \
        osKernel/osFieldB.c \
        osKernel/osFlash.c \
        osKernel/osLib.c \
        osKernel/osMain.c \
        osKernel/osTask.c \
        osShared/osLoad.c \
        osShared/osMsg.c \
        osShared/osUtil.c \
        vmKernel/actDFull.c \
        vmKernel/actDIncr.c \
        vmKernel/actDebug.c \
        vmKernel/actDown.c \
        vmKernel/actEtc.c \
        vmKernel/actFile.c \
        vmKernel/dbiMain.c \
        vmKernel/fileMain.c \
        vmKernel/intArith.c \
        vmKernel/intConv.c \
        vmKernel/intInter.c \
        vmKernel/intLevel.c \
        vmKernel/intMain.c \
        vmKernel/intWid16.c \
        vmKernel/intWid32.c \
        vmKernel/intWid64.c \
        vmKernel/intWid8.c \
        vmKernel/intWide.c \
        vmKernel/ioMain.c \
        vmKernel/md5.c \
        vmKernel/vmMain.c \
        vmKernel/vmPrc.c \
        vmKernel/vmTimer.c \
        vmKernel/vmmAct.c \
        vmKernel/vmmBreak.c \
        vmKernel/vmmCmd.c \
        vmKernel/vmmCom.c \
        vmKernel/vmmLoad.c \
        vmKernel/vmmMain.c \
        vmKernel/vmmMsg.c \
        vmKernel/vmmOnlCh.c \
        vmKernel/vmmRet.c \
        vmKernel/vmmSys.c \
        vmKernel/vmmUtil.c \
        vmLib/libDef.c \
        vmLib/libFile.c \
        vmLib/libIec.c \
        vmLib/libModbus.c \
        vmLib/libSfc.c \
        vmLib/libSys.c \
        vmLib/libSys2.c \
        vmLib/libUtil.c

HEADERS += \
    inc.data/CANopen.h \
    inc.data/dataMain.h \
    inc.fc/fcDef.h \
    inc.fc/fcMain.h \
    inc.mect/mectMain.h \
    inc/BuildNr.h \
    inc/intDef.h \
    inc/intOpcds.h \
    inc/iolDef.h \
    inc/libDef.h \
    inc/md5.h \
    inc/osAlign.h \
    inc/osDef.h \
    inc/osFile.h \
    inc/osFirst.h \
    inc/osLast.h \
    inc/osSocket.h \
    inc/osTarget.h \
    inc/stdInc.h \
    inc/vmShared.h \
    inc/vmmDef.h \
    inc/vmmMain.h \
    ioData/crosstable.h \
    ioData/dataImpl.h \
    ioData/hmi_plc.h \
    ioData/system_ini.h \
    vmKernel/vmmSys.h \
    vmLib/fbIec.h \
    vmLib/fbSys.h \
    vmLib/fbUtil.h \
    vmLib/funFile.h \
    vmLib/funIec.h \
    vmLib/funSys.h \
    vmLib/funSys2.h \
    vmLib/funSys22.h \
    vmLib/funUtil.h \
    vmLib/libFile.h \
    vmLib/libIec.h \
    vmLib/libModbus.h \
    vmLib/libSfc.h \
    vmLib/libSys.h \
    vmLib/libSys2.h \
    vmLib/libUtil.h