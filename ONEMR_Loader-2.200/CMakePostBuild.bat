﻿::Msg "%username%" developer: %1, PROJECT_NAME: %2, LIBRARY_OUTPUT_PATH: %3, lib_output: %4

@echo POST_BUILD_PART

@set "$Title=Built mod install"
@set "$Message=Done: "

@echo "working at LIBRARY_OUTPUT_PATH: %3"
@cd "%3"

@echo "remove minhook built static lib because who needs it even"
@if exist "minhook.x32.lib" del "minhook.x32.lib"

@echo "copy dll to game: %2.dll => %4"
@if not exist "%2.dll" set "$Message=Can´t found %3/%2.dll to copy dll!"
if exist "%2.dll" copy "%2.dll" %4

::BEEEp if smth wrong
@if not "%$Message%" == "Done: " rundll32 user32.dll,MessageBeep
::add warn emoji
@if not "%$Message%" == "Done: " set "$Title=⚠️ Failed to %$Title%"

@if "%$Message%" == "Done: " set "$Message=%$Message%%4"

::push notify
@powershell -Command "& {Add-Type -AssemblyName System.Windows.Forms; Add-Type -AssemblyName System.Drawing; $notify = New-Object System.Windows.Forms.NotifyIcon; $notify.Icon = [System.Drawing.SystemIcons]::Information; $notify.Visible = $true; $notify.ShowBalloonTip(0, '%$Title%', '%$Message%', [System.Windows.Forms.ToolTipIcon]::None)}"
@echo [%$Title%]: %$Message%