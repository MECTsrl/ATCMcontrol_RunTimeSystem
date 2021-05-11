TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

DEFINES += _SOF_4CPC_SRC_

LIBS += -Lpthread

SOURCES += \
        CANopen/CANopen.c \
        ioData/dataImpl.c \
        ioData/dataMain.c \
        ioData/hmi_plc.c \
        ioTest/tstImpl.c \
        ioTest/tstMain.c \
        ioTest/tstSimu.c \
        main.c \
        osKernel/fcAdapt.c \
        osKernel/mectAdapt.c \
        osKernel/mectCfgUtil.c \
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
        vmLib/libDatalog.c \
        vmLib/libDef.c \
        vmLib/libFile.c \
        vmLib/libHW119.c \
        vmLib/libIec.c \
        vmLib/libMect.c \
        vmLib/libMectUserUtility.c \
        vmLib/libModbus.c \
        vmLib/libSfc.c \
        vmLib/libSys.c \
        vmLib/libSys2.c \
        vmLib/libUSB.c \
        vmLib/libUtil.c \
        vmLib/privUtyMect.c

HEADERS += \
    inc.bac/bacFun.h \
    inc.data/CANopen.h \
    inc.data/dataMain.h \
    inc.fc/fcComm.h \
    inc.fc/fcDef.h \
    inc.fc/fcMBus.h \
    inc.fc/fcMain.h \
    inc.fc/fcModbus.h \
    inc.fc/fcProfi.h \
    inc.fc/fcSerCom.h \
    inc.fc/fcTime.h \
    inc.fc/profiDef.h \
    inc.fc/profiMain.h \
    inc.mect/mectCfgUtil.h \
    inc.mect/mectMain.h \
    inc.mect/mectRetentive.h \
    inc/BuildNr.h \
    inc/intDef.h \
    inc/intOpcds.h \
    inc/iolDef.h \
    inc/libDef.h \
    inc/libMBus2.h \
    inc/md5.h \
    inc/osAlign.h \
    inc/osDef.h \
    inc/osFile.h \
    inc/osFirst.h \
    inc/osLast.h \
    inc/osMBus2.h \
    inc/osSocket.h \
    inc/osTarget.h \
    inc/stdInc.h \
    inc/uthash.h \
    inc/vmShared.h \
    inc/vmmDef.h \
    inc/vmmMain.h \
    ioData/hmi_plc.h \
    vmKernel/vmmSys.h \
    vmLib/fbFile.h \
    vmLib/fbIec.h \
    vmLib/fbMBus2.h \
    vmLib/fbSys.h \
    vmLib/fbSys2.h \
    vmLib/fbUtil.h \
    vmLib/funBac.h \
    vmLib/funCanOpen.h \
    vmLib/funDatalog.h \
    vmLib/funFile.h \
    vmLib/funHW119.h \
    vmLib/funIec.h \
    vmLib/funMBus2.h \
    vmLib/funMect.h \
    vmLib/funMectUserUtility.h \
    vmLib/funModbus.h \
    vmLib/funSys.h \
    vmLib/funSys2.h \
    vmLib/funSys22.h \
    vmLib/funUSB.h \
    vmLib/funUtil.h \
    vmLib/libDatalog.h \
    vmLib/libFile.h \
    vmLib/libHW119.h \
    vmLib/libIec.h \
    vmLib/libMect.h \
    vmLib/libMectUserUtility.h \
    vmLib/libModbus.h \
    vmLib/libSfc.h \
    vmLib/libSys.h \
    vmLib/libSys2.h \
    vmLib/libUSB.h \
    vmLib/libUtil.h \
    vmLib/privUtyMect.h
