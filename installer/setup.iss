#define AppName      "Calculator"
#define AppVersion   "1.0.0"
#define AppPublisher "Levan Kotorashvili"
#define AppURL       "https://github.com/levanikotorashvili20-spec/calculator-v3"
#define AppExeName   "calculator v3.exe"

[Setup]
AppId={{3D8C9D60-6616-4397-AE33-F44C5D8C78A6}
AppName={#AppName}
AppVersion={#AppVersion}
AppPublisher={#AppPublisher}
AppPublisherURL={#AppURL}
AppSupportURL={#AppURL}
DefaultDirName={autopf}\{#AppName}
DisableProgramGroupPage=yes
PrivilegesRequiredOverridesAllowed=dialog
LicenseFile=..\LICENSE
SetupIconFile=..\resources\app.ico
UninstallDisplayIcon={app}\{#AppExeName}
OutputDir=output
OutputBaseFilename=CalculatorSetup-{#AppVersion}
Compression=lzma2
SolidCompression=yes
WizardStyle=modern

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "..\Release\{#AppExeName}"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{autoprograms}\{#AppName}"; Filename: "{app}\{#AppExeName}"
Name: "{autodesktop}\{#AppName}"; Filename: "{app}\{#AppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#AppExeName}"; Description: "{cm:LaunchProgram,{#AppName}}"; Flags: nowait postinstall skipifsilent