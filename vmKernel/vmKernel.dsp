# Microsoft Developer Studio Project File - Name="vmKernel" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=vmKernel - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vmKernel.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vmKernel.mak" CFG="vmKernel - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vmKernel - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "vmKernel - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/RunTime/SDK/V220.B4000/vmKernel", HHBGAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vmKernel - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 vmKernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 vmKernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "vmKernel - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 vmKernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 vmKernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "vmKernel - Win32 Release"
# Name "vmKernel - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\actDebug.c
# End Source File
# Begin Source File

SOURCE=.\actDFull.c
# End Source File
# Begin Source File

SOURCE=.\actDIncr.c
# End Source File
# Begin Source File

SOURCE=.\actDown.c
# End Source File
# Begin Source File

SOURCE=.\actEtc.c
# End Source File
# Begin Source File

SOURCE=.\actFile.c
# End Source File
# Begin Source File

SOURCE=.\dbiMain.c
# End Source File
# Begin Source File

SOURCE=.\fileMain.c
# End Source File
# Begin Source File

SOURCE=.\intArith.c
# End Source File
# Begin Source File

SOURCE=.\intConv.c
# End Source File
# Begin Source File

SOURCE=.\intInter.c
# End Source File
# Begin Source File

SOURCE=.\intLevel.c
# End Source File
# Begin Source File

SOURCE=.\intMain.c
# End Source File
# Begin Source File

SOURCE=.\intWid16.c
# End Source File
# Begin Source File

SOURCE=.\intWid32.c
# End Source File
# Begin Source File

SOURCE=.\intWid64.c
# End Source File
# Begin Source File

SOURCE=.\intWid8.c
# End Source File
# Begin Source File

SOURCE=.\intWide.c
# End Source File
# Begin Source File

SOURCE=.\ioMain.c
# End Source File
# Begin Source File

SOURCE=.\md5.c
# End Source File
# Begin Source File

SOURCE=.\vmmAct.c
# End Source File
# Begin Source File

SOURCE=.\vmMain.c
# End Source File
# Begin Source File

SOURCE=.\vmmBreak.c
# End Source File
# Begin Source File

SOURCE=.\vmmCmd.c
# End Source File
# Begin Source File

SOURCE=.\vmmCom.c
# End Source File
# Begin Source File

SOURCE=.\vmmLoad.c
# End Source File
# Begin Source File

SOURCE=.\vmmMain.c
# End Source File
# Begin Source File

SOURCE=.\vmmMsg.c
# End Source File
# Begin Source File

SOURCE=.\vmmOnlCh.c
# End Source File
# Begin Source File

SOURCE=.\vmmRet.c
# End Source File
# Begin Source File

SOURCE=.\vmmSys.c
# End Source File
# Begin Source File

SOURCE=.\vmmUtil.c
# End Source File
# Begin Source File

SOURCE=.\vmPrc.c
# End Source File
# Begin Source File

SOURCE=.\vmTimer.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\inc\BuildNr.h
# End Source File
# Begin Source File

SOURCE=..\inc\intDef.h
# End Source File
# Begin Source File

SOURCE=..\inc\intOpcds.h
# End Source File
# Begin Source File

SOURCE=..\inc\iolDef.h
# End Source File
# Begin Source File

SOURCE=..\inc\libDef.h
# End Source File
# Begin Source File

SOURCE=..\inc\md5.h
# End Source File
# Begin Source File

SOURCE=..\inc\osAlign.h
# End Source File
# Begin Source File

SOURCE=..\inc\osDef.h
# End Source File
# Begin Source File

SOURCE=..\inc\osFile.h
# End Source File
# Begin Source File

SOURCE=..\inc\osFirst.h
# End Source File
# Begin Source File

SOURCE=..\inc\osLast.h
# End Source File
# Begin Source File

SOURCE=..\inc\osSocket.h
# End Source File
# Begin Source File

SOURCE=..\inc\osTarget.h
# End Source File
# Begin Source File

SOURCE=..\inc\stdInc.h
# End Source File
# Begin Source File

SOURCE=..\inc\vmmDef.h
# End Source File
# Begin Source File

SOURCE=..\inc\vmmMain.h
# End Source File
# Begin Source File

SOURCE=.\vmmSys.h
# End Source File
# Begin Source File

SOURCE=..\inc\vmShared.h
# End Source File
# End Group
# Begin Group "Makefiles"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\objects.mk
# End Source File
# Begin Source File

SOURCE=.\vmKernel.mak
# End Source File
# End Group
# End Target
# End Project
