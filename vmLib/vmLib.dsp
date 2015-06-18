# Microsoft Developer Studio Project File - Name="vmLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=vmLib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vmLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vmLib.mak" CFG="vmLib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vmLib - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "vmLib - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/RunTime/SDK/V220.B4000/vmLib", IHBGAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vmLib - Win32 Release"

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
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "vmLib - Win32 Debug"

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
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "vmLib - Win32 Release"
# Name "vmLib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\libDef.c
# End Source File
# Begin Source File

SOURCE=.\libFile.c
# End Source File
# Begin Source File

SOURCE=.\libIec.c
# End Source File
# Begin Source File

SOURCE=.\libSfc.c
# End Source File
# Begin Source File

SOURCE=.\libSys.c
# End Source File
# Begin Source File

SOURCE=.\libSys2.c
# End Source File
# Begin Source File

SOURCE=.\libUtil.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\fbFile.h
# End Source File
# Begin Source File

SOURCE=.\fbIec.h
# End Source File
# Begin Source File

SOURCE=.\fbMBus2.h
# End Source File
# Begin Source File

SOURCE=.\fbSys.h
# End Source File
# Begin Source File

SOURCE=.\fbSys2.h
# End Source File
# Begin Source File

SOURCE=.\fbUtil.h
# End Source File
# Begin Source File

SOURCE=.\funBac.h
# End Source File
# Begin Source File

SOURCE=.\funFile.h
# End Source File
# Begin Source File

SOURCE=.\funIec.h
# End Source File
# Begin Source File

SOURCE=.\funMBus2.h
# End Source File
# Begin Source File

SOURCE=.\funSys.h
# End Source File
# Begin Source File

SOURCE=.\funSys2.h
# End Source File
# Begin Source File

SOURCE=.\funSys22.h
# End Source File
# Begin Source File

SOURCE=.\funUtil.h
# End Source File
# Begin Source File

SOURCE=.\libFile.h
# End Source File
# Begin Source File

SOURCE=.\libIec.h
# End Source File
# Begin Source File

SOURCE=..\inc\libMBus2.h
# End Source File
# Begin Source File

SOURCE=.\libSfc.h
# End Source File
# Begin Source File

SOURCE=.\libSys.h
# End Source File
# Begin Source File

SOURCE=.\libSys2.h
# End Source File
# Begin Source File

SOURCE=.\libUtil.h
# End Source File
# Begin Source File

SOURCE=..\inc\osMBus2.h
# End Source File
# End Group
# Begin Group "Makefiles"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\objects.mk
# End Source File
# Begin Source File

SOURCE=.\vmLib.mak
# End Source File
# End Group
# End Target
# End Project
