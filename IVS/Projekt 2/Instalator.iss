; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{99C03AEE-0924-4BB9-A2F0-424B6AF45569}
AppName=IVS2018
AppVersion=1.0.0
;AppVerName=IVS2018 0.7.5
AppPublisher=IVS Dream Team
DefaultDirName={pf}\IVS2018
DisableProgramGroupPage=yes
OutputBaseFilename=IVS18_install
Compression=lzma
SolidCompression=yes
InfoBeforeFile=info.txt
DisableWelcomePage=no

[Languages]
Name: "czech"; MessagesFile: "compiler:Languages\Czech.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "C:\Users\Roman\Disk Google\IVS\Projekt 2\Calculator.exe"; DestDir: "{app}"; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{commonprograms}\IVS2018"; Filename: "{app}\Calculator.exe"
Name: "{commondesktop}\IVS2018"; Filename: "{app}\Calculator.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\Calculator.exe"; Description: "{cm:LaunchProgram,IVS2018}"; Flags: nowait postinstall skipifsilent

