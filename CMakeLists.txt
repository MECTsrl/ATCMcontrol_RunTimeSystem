cmake_minimum_required(VERSION 3.5)

project(FarosPLC VERSION 3.000)
project(FarosPLC LANGUAGES C)

set(BUILD_SOURCES
    CANopen/CANopen.c
    inc/BuildNr.h
    inc.data/CANopen.h
    inc.data/dataMain.h
    inc.fc/fcDef.h
    inc.fc/fcMain.h
    inc/intDef.h
    inc/intOpcds.h
    inc/iolDef.h
    inc/libDef.h
    inc/md5.h
    inc.mect/mectMain.h
    inc.mect/mectRetentive.h
    inc/osAlign.h
    inc/osDef.h
    inc/osFile.h
    inc/osFirst.h
    inc/osLast.h
    inc/osSocket.h
    inc/osTarget.h
    inc/stdInc.h
    inc/vmmDef.h
    inc/vmmMain.h
    inc/vmShared.h
    ioData/crosstable.c
    ioData/crosstable.h
    ioData/dataClients.c
    ioData/dataDiagnostics.c
    ioData/dataEngine.c
    ioData/dataFbMect.c
    ioData/dataHmiPlc.c
    ioData/dataImpl.c
    ioData/dataImpl.h
    ioData/dataMain.c
    ioData/dataServers.c
    ioData/dataUtils.c
    ioData/hmi_plc.c
    ioData/hmi_plc.h
    ioData/system_ini.c
    ioData/system_ini.h
    ioTest/tstImpl.c
    ioTest/tstMain.c
    ioTest/tstSimu.c
    osKernel/fcAdapt.c
    osKernel/mectAdapt.c
    osKernel/osFieldB.c
    osKernel/osFlash.c
    osKernel/osLib.c
    osKernel/osMain.c
    osKernel/osTask.c
    osShared/osLoad.c
    osShared/osMsg.c
    osShared/osUtil.c
    vmKernel/actDebug.c
    vmKernel/actDFull.c
    vmKernel/actDIncr.c
    vmKernel/actDown.c
    vmKernel/actEtc.c
    vmKernel/actFile.c
    vmKernel/dbiMain.c
    vmKernel/fileMain.c
    vmKernel/intInter.c
    vmKernel/intLevel.c
    vmKernel/intMain.c
    vmKernel/ioMain.c
    vmKernel/md5.c
    vmKernel/vmmAct.c
    vmKernel/vmMain.c
    vmKernel/vmmBreak.c
    vmKernel/vmmCmd.c
    vmKernel/vmmCom.c
    vmKernel/vmmLoad.c
    vmKernel/vmmMain.c
    vmKernel/vmmMsg.c
    vmKernel/vmmOnlCh.c
    vmKernel/vmmRet.c
    vmKernel/vmmSys.c
    vmKernel/vmmSys.h
    vmKernel/vmmUtil.c
    vmKernel/vmPrc.c
    vmKernel/vmTimer.c
    vmLib/fbIec.h
    vmLib/fbSys.h
    vmLib/fbUtil.h
    vmLib/funFile.h
    vmLib/funIec.h
    vmLib/funSys22.h
    vmLib/funSys2.h
    vmLib/funSys.h
    vmLib/funUtil.h
    vmLib/libDef.c
    vmLib/libFile.c
    vmLib/libFile.h
    vmLib/libIec.c
    vmLib/libIec.h
    vmLib/libModbus.c
    vmLib/libModbus.h
    vmLib/libSfc.c
    vmLib/libSfc.h
    vmLib/libSys2.c
    vmLib/libSys2.h
    vmLib/libSys.c
    vmLib/libSys.h
    vmLib/libUtil.c
    vmLib/libUtil.h
    )

set(PROJECT_SOURCES
    ${BUILD_SOURCES}
    vmKernel/intArith.c
    vmKernel/intConv.c
    vmKernel/intWide.c
    vmKernel/intWid8.c
    vmKernel/intWid16.c
    vmKernel/intWid32.c
    vmKernel/intWid64.c
    )

find_package(Threads REQUIRED)


add_executable(FarosPLC main.c ${BUILD_SOURCES})

target_include_directories(FarosPLC INTERFACE ${CMAKE_SOURCE_DIR})
target_compile_definitions(FarosPLC PUBLIC _SOF_4CPC_SRC_)
target_link_libraries(FarosPLC ${CMAKE_THREAD_LIBS_INIT} rt m)

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static")

