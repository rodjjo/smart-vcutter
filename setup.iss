[Setup]
AppName=Smart VCutter - Loop Creator
AppVersion=1.0
DefaultDirName={pf}\Smart VCutter Loop Creator
DefaultGroupName=Smart VCutter Loop Creator
UninstallDisplayIcon={app}\uninstall.ico
Compression=lzma2
SolidCompression=yes
OutputBaseFilename=Setup
OutputDir=.\build\bin

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "build\bin\smart-vcutter.exe"; DestDir: "{app}"
Source: "build\bin\*.dll"; DestDir: "{app}"
Source: "data\images\app.ico"; DestDir: "{app}"
Source: "data\images\uninstall.ico"; DestDir: "{app}"

[Icons]
Name: "{group}\Smart Loop Creator"; Filename: "{app}\smart-vcutter.exe"; IconFilename: {app}\app.ico
Name: "{userdesktop}\Smart Loop Creator"; Filename: "{app}\smart-vcutter.exe"; IconFilename: {app}\app.ico; Tasks: desktopicon