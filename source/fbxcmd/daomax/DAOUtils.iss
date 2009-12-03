; -- MaxMSHUtils.iss --
;

[Setup]
AppName=DA:O Utilities for 3ds Max
AppVerName=DA:O Utilities {code:CurVer} for 3ds Max
AppPublisher=DA:O Max Plugins
AppCopyright=Copyright © 2009, DA:O Max Utilities 
OutputBaseFilename=dao-max-plugins-1.0.0
DisableProgramGroupPage=yes
Compression=lzma
SolidCompression=yes
DirExistsWarning=no
EnableDirDoesntExistWarning=yes
UsePreviousAppDir=yes
DefaultDirName={win}{\}Installer\daoutils
UninstallFilesDir={win}{\}Installer\daoutils
Uninstallable=yes
DisableDirPage=yes
ArchitecturesInstallIn64BitMode=x64
VersionInfoVersion=1.0.0

SourceDir=.

[Types]
;Name: "full"; Description: "Full installation"
Name: "custom"; Description: "Custom installation"; Flags: iscustom

[Components]
Name: "gmax12"; Description: "gmax 1.2"; Types: custom;
;Name: "max4"; Description: "3ds Max 4"; Types: custom;
Name: "max42"; Description: "3ds Max 4.2"; Types: custom;
Name: "max5"; Description: "3ds Max 5"; Types: custom;
Name: "max6"; Description: "3ds Max 6"; Types: custom;
Name: "max7"; Description: "3ds Max 7"; Types: custom;
Name: "max8"; Description: "3ds Max 8"; Types: custom;
Name: "max9"; Description: "3ds Max 9 (Win32)"; Types: custom;
Name: "max9x64"; Description: "3ds Max 9 (x64)"; Types: custom;
Name: "max10"; Description: "3ds Max 2008 (Win32)"; Types: custom;
Name: "max10x64"; Description: "3ds Max 2008 (x64)"; Types: custom;
Name: "max11"; Description: "3ds Max 2009 (Win32)"; Types: custom;
Name: "max11x64"; Description: "3ds Max 2009 (x64)"; Types: custom;
Name: "max12"; Description: "3ds Max 2010 (Win32)"; Types: custom;
Name: "max12x64"; Description: "3ds Max 2010 (x64)"; Types: custom;
Name: "src"; Description: "Program Source"; Types: custom;

[Files]
Source: "README.TXT"; DestDir: "{app}"; Flags: isreadme ignoreversion;
Source: "CHANGELOG.TXT"; DestDir: "{app}"; Flags: isreadme ignoreversion;
Source: "LICENSE.TXT"; DestDir: "{app}"; Flags: isreadme ignoreversion;

Source: "c:\gmax12\winmm.dll"; DestDir: "{code:InstallPath|gmax12}"; Components: "gmax12"; Flags: ignoreversion;
Source: "Staging\Win32\Release - gmax\DAOUtils.dlu"; DestDir: "{code:InstallPath|gmax12}{\}plugins"; Components: "gmax12"; Flags: ignoreversion;
Source: "DAOUtils.ini"; DestDir: "{code:InstallPath|gmax12}{\}plugcfg"; Components: "gmax12"; Flags: ignoreversion;

;Source: "Staging\Win32\Release - Max 4\Readme.txt"; DestName: "MSHPlugins_Readme.txt"; DestDir: "{code:InstallPath|max4}"; Components: "max4"; Flags: isreadme ignoreversion;
;Source: "Staging\Win32\Release - Max 4\DAOUtils.dlu"; DestDir: "{code:InstallPath|max4}{\}plugins"; Components: "max4"; Flags: ignoreversion;
;Source: "Staging\Win32\Release - Max 4\DAOUtils.ini"; DestDir: "{code:InstallPath|max4}{\}plugcfg"; Components: "max4"; Flags: ignoreversion;

Source: "Staging\Win32\Release - Max 4.2\DAOUtils.dlu"; DestDir: "{code:InstallPath|max42}{\}plugins"; Components: "max42"; Flags: ignoreversion;
Source: "DAOUtils.ini"; DestDir: "{code:InstallPath|max42}{\}plugcfg"; Components: "max42"; Flags: ignoreversion;

Source: "Staging\Win32\Release - Max 5\DAOUtils.dlu"; DestDir: "{code:InstallPath|max5}{\}plugins"; Components: "max5"; Flags: ignoreversion;
Source: "DAOUtils.ini"; DestDir: "{code:InstallPath|max5}{\}plugcfg"; Components: "max5"; Flags: ignoreversion;

Source: "Staging\Win32\Release - Max 6\DAOUtils.dlu"; DestDir: "{code:InstallPath|max6}{\}plugins"; Components: "max6"; Flags: ignoreversion;
Source: "DAOUtils.ini"; DestDir: "{code:InstallPath|max6}{\}plugcfg"; Components: "max6"; Flags: ignoreversion;

Source: "Staging\Win32\Release - Max 7\DAOUtils.dlu"; DestDir: "{code:InstallPath|max7}{\}plugins"; Components: "max7"; Flags: ignoreversion;
Source: "DAOUtils.ini"; DestDir: "{code:InstallPath|max7}{\}plugcfg"; Components: "max7"; Flags: ignoreversion;

Source: "Staging\Win32\Release - Max 8\DAOUtils.dlu"; DestDir: "{code:InstallPath|max8}{\}plugins"; Components: "max8"; Flags: ignoreversion;
Source: "DAOUtils.ini"; DestDir: "{code:InstallPath|max8}{\}plugcfg"; Components: "max8"; Flags: ignoreversion;

Source: "Staging\Win32\Release - Max 9\DAOUtils.dlu"; DestDir: "{code:InstallPath|max9}{\}plugins"; Components: "max9"; Flags: ignoreversion;
Source: "DAOUtils.ini"; DestDir: "{code:InstallPath|max9}{\}plugcfg"; Components: "max9"; Flags: ignoreversion;
Source: "DAOUtils.ini"; DestDir: "{localappdata}{\}Autodesk\3dsmax\9 - 32bit\enu\plugcfg"; Components: "max9"; Flags: ignoreversion;

Source: "Staging\x64\Release - Max 9\DAOUtils.dlu"; DestDir: "{code:InstallPath|max9x64}{\}plugins"; Components: "max9x64"; Flags: ignoreversion;
Source: "DAOUtils.ini"; DestDir: "{code:InstallPath|max9x64}{\}plugcfg"; Components: "max9x64"; Flags: ignoreversion;
Source: "DAOUtils.ini"; DestDir: "{localappdata}{\}Autodesk\3dsmax\9 - 64bit\enu\plugcfg"; Components: "max9x64"; Flags: ignoreversion;

Source: "Staging\Win32\Release - Max 2008\DAOUtils.dlu"; DestDir: "{code:InstallPath|max10}{\}plugins"; Components: "max10"; Flags: ignoreversion;
Source: "DAOUtils.ini"; DestDir: "{code:InstallPath|max10}{\}plugcfg"; Components: "max10"; Flags: ignoreversion;
Source: "DAOUtils.ini"; DestDir: "{localappdata}{\}Autodesk\3dsmax\2008 - 32bit\enu\plugcfg"; Components: "max10"; Flags: ignoreversion;

Source: "Staging\x64\Release - Max 2008\DAOUtils.dlu"; DestDir: "{code:InstallPath|max10x64}{\}plugins"; Components: "max10x64"; Flags: ignoreversion;
Source: "DAOUtils.ini"; DestDir: "{code:InstallPath|max10x64}{\}plugcfg"; Components: "max10x64"; Flags: ignoreversion;
Source: "DAOUtils.ini"; DestDir: "{localappdata}{\}Autodesk\3dsmax\2009 - 64bit\enu\plugcfg"; Components: "max10x64"; Flags: ignoreversion;

Source: "Staging\Win32\Release - Max 2009\DAOUtils.dlu"; DestDir: "{code:InstallPath|max11}{\}plugins"; Components: "max11"; Flags: ignoreversion;
Source: "DAOUtils.ini"; DestDir: "{code:InstallPath|max11}{\}plugcfg"; Components: "max11"; Flags: ignoreversion;
Source: "DAOUtils.ini"; DestDir: "{localappdata}{\}Autodesk\3dsmax\2009 - 32bit\enu\plugcfg"; Components: "max11"; Flags: ignoreversion;

Source: "Staging\x64\Release - Max 2009\DAOUtils.dlu"; DestDir: "{code:InstallPath|max11x64}{\}plugins"; Components: "max11x64"; Flags: ignoreversion;
Source: "DAOUtils.ini"; DestDir: "{code:InstallPath|max11x64}{\}plugcfg"; Components: "max11x64"; Flags: ignoreversion;
Source: "DAOUtils.ini"; DestDir: "{localappdata}{\}Autodesk\3dsmax\2010 - 64bit\enu\plugcfg"; Components: "max11x64"; Flags: ignoreversion;

Source: "Staging\Win32\Release - Max 2010\DAOUtils.dlu"; DestDir: "{code:InstallPath|max12}{\}plugins"; Components: "max12"; Flags: ignoreversion;
Source: "DAOUtils.ini"; DestDir: "{code:InstallPath|max12}{\}plugcfg"; Components: "max12"; Flags: ignoreversion;
Source: "DAOUtils.ini"; DestDir: "{localappdata}{\}Autodesk\3dsmax\2010 - 32bit\enu\plugcfg"; Components: "max12"; Flags: ignoreversion;

Source: "Staging\x64\Release - Max 2010\DAOUtils.dlu"; DestDir: "{code:InstallPath|max12x64}{\}plugins"; Components: "max12x64"; Flags: ignoreversion;
Source: "DAOUtils.ini"; DestDir: "{code:InstallPath|max12x64}{\}plugcfg"; Components: "max12x64"; Flags: ignoreversion;
Source: "DAOUtils.ini"; DestDir: "{localappdata}{\}Autodesk\3dsmax\2010 - 64bit\enu\plugcfg"; Components: "max12x64"; Flags: ignoreversion;

;Source: "Staging\Src\1.0.0\*"; DestDir:"{code:DAOPath}{\}tools{\}Mods{\}daomax"; Components: "src"; Flags: ignoreversion recursesubdirs;

[Code]
var sVersion: String;

function InitializeSetup(): Boolean;
begin
  sVersion := '1.0.0';
  Result := True;
end;

function InstallPath(Param: String): String;
  var
    Names: TArrayOfString;
    I: Integer;
begin
  Result := '';
  case Param of
    'gmax12':
        if not RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\gmax', 'uninstallpath', Result) then
          Result := ExpandConstant('{sd}{\}gmax12');
    'max4':
        if not RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\3ds max 4', 'uninstallpath', Result) then
          Result := ExpandConstant('{sd}{\}3dsmax4');
    'max42':
        if not RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\3ds max 4', 'uninstallpath', Result) then
          Result := ExpandConstant('{sd}{\}3dsmax42');
    'max5':
        if not RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\3ds max 5', 'uninstallpath', Result) then
          Result := ExpandConstant('{sd}{\}3dsmax5');
    'max6':
        if not RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Autodesk\3dsMax\6.0', 'InstallDir', Result) then
          Result := ExpandConstant('{sd}{\}3dsmax6');
    'max7':
        if not RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Autodesk\3dsMax\7.0', 'InstallDir', Result) then
          Result := ExpandConstant('{sd}{\}3dsmax7');
    'max8':
        if not RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Autodesk\3dsMax\8.0', 'InstallDir', Result) then
          Result := ExpandConstant('{sd}{\}3dsmax8');
    'max9': 
        begin
          if RegGetSubkeyNames(HKEY_LOCAL_MACHINE, 'SOFTWARE\Autodesk\3dsMax\9.0', Names) then begin
            for I := 0 to GetArrayLength(Names)-1 do begin
              if RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Autodesk\3dsMax\9.0\' + Names[I], 'Installdir', Result) then begin
                break;
              end;
            end;
          end;
          if (Length(Result) = 0) then
            Result := ExpandConstant('{pf32}{\}AutoDesk\3ds Max 9}{\}plugcfg');
        end;
    'max9x64':
        begin
          if RegGetSubkeyNames(HKEY_LOCAL_MACHINE, 'SOFTWARE\Autodesk\3dsMax\9.0', Names) then begin
            for I := 0 to GetArrayLength(Names)-1 do begin
              if RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Autodesk\3dsMax\9.0\' + Names[I], 'InstallDir', Result) then
                break;              
            end;
          end;
          if Length(Result) = 0 then
            Result := ExpandConstant('{pf64}{\}AutoDesk\3ds Max 9}{\}plugcfg');
        end;
    'max10': 
        begin
          if RegGetSubkeyNames(HKEY_LOCAL_MACHINE, 'SOFTWARE\Autodesk\3dsMax\10.0', Names) then begin
            for I := 0 to GetArrayLength(Names)-1 do begin
              if RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Autodesk\3dsMax\10.0\' + Names[I], 'Installdir', Result) then begin
                break;
              end;
            end;
          end;
          if (Length(Result) = 0) then
            Result := ExpandConstant('{pf32}{\}AutoDesk\3ds Max 2008}{\}plugcfg');
        end;
    'max10x64':
        begin
          if RegGetSubkeyNames(HKEY_LOCAL_MACHINE, 'SOFTWARE\Autodesk\3dsMax\10.0', Names) then begin
            for I := 0 to GetArrayLength(Names)-1 do begin
              if RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Autodesk\3dsMax\10.0\' + Names[I], 'InstallDir', Result) then
                break;              
            end;
          end;
          if Length(Result) = 0 then
            Result := ExpandConstant('{pf64}{\}AutoDesk\3ds Max 2008}{\}plugcfg');
        end;
    'max11': 
        begin
          if RegGetSubkeyNames(HKEY_LOCAL_MACHINE, 'SOFTWARE\Autodesk\3dsMax\11.0', Names) then begin
            for I := 0 to GetArrayLength(Names)-1 do begin
              if RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Autodesk\3dsMax\11.0\' + Names[I], 'Installdir', Result) then begin
                break;
              end;
            end;
          end;
          if (Length(Result) = 0) then
            Result := ExpandConstant('{pf32}{\}AutoDesk\3ds Max 2009}{\}plugcfg');
        end;
    'max11x64':
        begin
          if RegGetSubkeyNames(HKEY_LOCAL_MACHINE, 'SOFTWARE\Autodesk\3dsMax\11.0', Names) then begin
            for I := 0 to GetArrayLength(Names)-1 do begin
              if RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Autodesk\3dsMax\11.0\' + Names[I], 'InstallDir', Result) then
                break;              
            end;
          end;
          if Length(Result) = 0 then
            Result := ExpandConstant('{pf64}{\}AutoDesk\3ds Max 2009}{\}plugcfg');
        end;
    'max12': 
        begin
          if RegGetSubkeyNames(HKEY_LOCAL_MACHINE, 'SOFTWARE\Autodesk\3dsMax\12.0', Names) then begin
            for I := 0 to GetArrayLength(Names)-1 do begin
              if RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Autodesk\3dsMax\12.0\' + Names[I], 'Installdir', Result) then begin
                break;
              end;
            end;
          end;
          if (Length(Result) = 0) then
            Result := ExpandConstant('{pf32}{\}AutoDesk\3ds Max 2010}{\}plugcfg');
        end;
    'max12x64':
        begin
          if RegGetSubkeyNames(HKEY_LOCAL_MACHINE, 'SOFTWARE\Autodesk\3dsMax\12.0', Names) then begin
            for I := 0 to GetArrayLength(Names)-1 do begin
              if RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Autodesk\3dsMax\12.0\' + Names[I], 'InstallDir', Result) then
                break;              
            end;
          end;
          if Length(Result) = 0 then
            Result := ExpandConstant('{pf64}{\}AutoDesk\3ds Max 2010}{\}plugcfg');
        end;		
    else
      Result := '';
  end;
end;

function RegInstallPath(Param: String): String;
  var
    Names: TArrayOfString;
    I: Integer;
begin
  Result := '';
  case Param of
    'gmax 1.2':
		if IsWin64() then begin
			RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\WOW6432Node\Microsoft\Windows\CurrentVersion\Uninstall\gmax', 'uninstallpath', Result);
		end else begin
			RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\gmax', 'uninstallpath', Result);
		end;
		
    '3ds Max 4':
        RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\3ds max 4', 'uninstallpath', Result);
    '3ds Max 4.2':
        RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\3ds max 4', 'uninstallpath', Result);
    '3ds Max 5':
        RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\3ds max 5', 'uninstallpath', Result);
    '3ds Max 6':
        RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Autodesk\3dsMax\6.0', 'InstallDir', Result);
    '3ds Max 7':
        RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Autodesk\3dsMax\7.0', 'InstallDir', Result);
    '3ds Max 8':
        RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Autodesk\3dsMax\8.0', 'InstallDir', Result);
    '3ds Max 9 (Win32)':
        if RegGetSubkeyNames(HKEY_LOCAL_MACHINE, 'SOFTWARE\Autodesk\3dsMax\9.0', Names) then begin
          for I := 0 to GetArrayLength(Names)-1 do begin
            if RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Autodesk\3dsMax\9.0\' + Names[I], 'InstallDir', Result) then
              break;              
          end;
        end;
    '3ds Max 9 (x64)':
        if RegGetSubkeyNames(HKEY_LOCAL_MACHINE, 'SOFTWARE\Autodesk\3dsMax\9.0', Names) then begin
          for I := 0 to GetArrayLength(Names)-1 do begin
            if RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Autodesk\3dsMax\9.0\' + Names[I], 'InstallDir', Result) then
              break;              
          end;
        end;
    '3ds Max 2008 (Win32)':
        if RegGetSubkeyNames(HKEY_LOCAL_MACHINE, 'SOFTWARE\Autodesk\3dsMax\10.0', Names) then begin
          for I := 0 to GetArrayLength(Names)-1 do begin
            if RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Autodesk\3dsMax\10.0\' + Names[I], 'InstallDir', Result) then
              break;              
          end;
        end;
    '3ds Max 2008 (x64)':
        if RegGetSubkeyNames(HKEY_LOCAL_MACHINE, 'SOFTWARE\Autodesk\3dsMax\10.0', Names) then begin
          for I := 0 to GetArrayLength(Names)-1 do begin
            if RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Autodesk\3dsMax\10.0\' + Names[I], 'InstallDir', Result) then
              break;              
          end;
        end;
    '3ds Max 2009 (Win32)':
        if RegGetSubkeyNames(HKEY_LOCAL_MACHINE, 'SOFTWARE\Autodesk\3dsMax\11.0', Names) then begin
          for I := 0 to GetArrayLength(Names)-1 do begin
            if RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Autodesk\3dsMax\11.0\' + Names[I], 'InstallDir', Result) then
              break;              
          end;
        end;
    '3ds Max 2009 (x64)':
        if RegGetSubkeyNames(HKEY_LOCAL_MACHINE, 'SOFTWARE\Autodesk\3dsMax\11.0', Names) then begin
          for I := 0 to GetArrayLength(Names)-1 do begin
            if RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Autodesk\3dsMax\11.0\' + Names[I], 'InstallDir', Result) then
              break;              
          end;
        end;
    '3ds Max 2010 (Win32)':
        if RegGetSubkeyNames(HKEY_LOCAL_MACHINE, 'SOFTWARE\Autodesk\3dsMax\12.0', Names) then begin
          for I := 0 to GetArrayLength(Names)-1 do begin
            if RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Autodesk\3dsMax\12.0\' + Names[I], 'InstallDir', Result) then
              break;              
          end;
        end;
    '3ds Max 2010 (x64)':
        if RegGetSubkeyNames(HKEY_LOCAL_MACHINE, 'SOFTWARE\Autodesk\3dsMax\12.0', Names) then begin
          for I := 0 to GetArrayLength(Names)-1 do begin
            if RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Autodesk\3dsMax\12.0\' + Names[I], 'InstallDir', Result) then
              break;              
          end;
        end;
    else
      Result := '';
  end;
end;

function IsInstalled(Param: String): Boolean;
begin
  Result := Length(RegInstallPath(Param)) <> 0;
  if Result then begin
    if not IsWin64() and (Param = '3ds Max 9 (x64)') then
       Result := False;
    if not IsWin64() and (Param = '3ds Max 2008 (x64)') then
       Result := False;
    if not IsWin64() and (Param = '3ds Max 2009 (x64)') then
       Result := False;
    if not IsWin64() and (Param = '3ds Max 2010 (x64)') then
       Result := False;
  end;
end;

function CurVer(Param: String): String;
begin
  Result := sVersion;
end;

function DAOPath(Param: String): String;
begin
  Result := '';
  if not RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{F20C1251-1D0A-4944-B2AE-678581B33B19}', 'InstallLocation', Result) then
  begin
    if not RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\nwn2.exe', 'Path', Result) then
        Result := ExpandConstant('{pf}{\}Neverwinter Nights 2');
  end;
end;

procedure CurPageChanged(CurPageID: Integer);
var 
    i: Integer;
begin
  if CurPageId = wpSelectComponents then
    for i := 0 to WizardForm.ComponentsList.Items.Count - 1 do
       WizardForm.ComponentsList.Checked[i] := IsInstalled(WizardForm.ComponentsList.ItemCaption[i]);
end;
