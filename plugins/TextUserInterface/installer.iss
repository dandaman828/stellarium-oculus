; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
AppName=Stellarium TUI Plugin
AppVerName=Stellarium TUI Plugin 0.1.0
AppPublisher=Stellarium
AppPublisherURL=http://stellarium.org/
AppSupportURL=http://stellarium.org/
AppUpdatesURL=http://stellarium.org/
DefaultDirName={userappdata}\Stellarium\modules\TextUserInterface
DefaultGroupName=Stellarium
AllowNoIcons=yes
LicenseFile=COPYING
OutputDir=..\win32installers
OutputBaseFilename=stellarium-tui-plugin
SetupIconFile=..\..\stellarium\data\stellarium.ico
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "README"; DestDir: "{app}"; Flags: ignoreversion
Source: "builds\msys\src\libTextUserInterface.dll"; DestDir: "{app}"; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\{cm:UninstallProgram,Stellarium TUI Plugin}"; Filename: "{uninstallexe}"

