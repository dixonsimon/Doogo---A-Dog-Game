@echo off
setlocal
echo Building Doogo...

:: --- CONFIGURATION ---
:: Change this if Raylib is installed somewhere else (e.g. D:\raylib)
set RAYLIB_ROOT=C:\raylib
:: ---------------------

:: 1. Add the compiler (w64devkit) to the PATH temporarily
set PATH=%RAYLIB_ROOT%\w64devkit\bin;%PATH%

:: 2. Compile
:: We use %RAYLIB_ROOT% to make sure we find the include (-I) and library (-L) files
gcc src\main.c src\player.c src\world.c src\ui.c src\screens.c -o Doogo.exe -O1 -Wall -std=c99 -Wno-missing-braces -I src -I %RAYLIB_ROOT%\raylib\src -L %RAYLIB_ROOT%\raylib\src -lraylib -lopengl32 -lgdi32 -lwinmm

:: 3. Check for errors
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [ERROR] Build Failed! 
    echo Please check if Raylib is actually installed at: %RAYLIB_ROOT%
) else (
    echo.
    echo Build Success! Run Doogo.exe to play.
)

pause