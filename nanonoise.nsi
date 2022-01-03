Name "nanonoise"
SetCompressor lzma

# Defines
!define REGKEY "SOFTWARE\$(^Name)" ;
!define VERSION 1.00
!define COMPANY "Jakob Flierl"
!define URL http://koppi.github.com/nanonoise/

!define QTDIR C:\Qt\2009.04

Caption "nanonoise ${VERSION} Setup"

BrandingText " © Jakob Flierl "

XPStyle on

; The file to write
OutFile "nanonoise-install.exe"

; The default installation directory
InstallDir $PROGRAMFILES\nanonoise

InstallDirRegKey HKLM "Software\nanonoise" "Install_Dir"

RequestExecutionLevel admin

Page components
Page directory
Page instfiles

UninstPage uninstConfirm
uninstPage instfiles

Section "nanonoise (required)" ;
  
  SetShellVarContext all
  SetOverwrite on

  SectionIn RO

  SetOutPath $INSTDIR
  File "release\nanonoise.exe"
  File "${QTDIR}\qt\bin\QtCore4.dll"
  File "${QTDIR}\qt\bin\QtGui4.dll"
  File "${QTDIR}\qt\bin\QtXml4.dll"
  File "${QTDIR}\mingw\bin\mingwm10.dll"

  WriteRegStr HKLM SOFTWARE\nanonoise "Install_Dir" "$INSTDIR"
  
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\nanonoise" "DisplayName" "nanonoise"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\nanonoise" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\nanonoise" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\nanonoise" "NoRepair" 1
  WriteUninstaller "uninstall.exe"

  
SectionEnd

Section "Start Menu Shortcuts"

  SetShellVarContext all
  SetOverwrite on

  CreateDirectory "$SMPROGRAMS\nanonoise"
  CreateShortCut "$SMPROGRAMS\nanonoise\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\nanonoise\nanonoise.lnk" "$INSTDIR\nanonoise.exe" "" "$INSTDIR\nanonoise.exe" 0
  
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  SetShellVarContext all
  SetOverwrite on

  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\nanonoise"
  DeleteRegKey HKLM SOFTWARE\nanonoise

  ; Remove files and uninstaller
  Delete $INSTDIR\nanonoise.exe
  Delete $INSTDIR\uninstall.exe

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\nanonoise\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\nanonoise"
  RMDir "$INSTDIR"

SectionEnd
