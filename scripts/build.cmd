@echo off

if "%1"=="" (
    echo RELEASE
    set DIR="release"
    set ARGS="/O1"
) else (
    echo DEBUG
    set DIR="debug"
    set ARGS="/Zi"
)

if not exist "build/%DIR%" md "build/%DIR%"
cd "build/%DIR%"
cl.exe /nologo /EHsc /GA %ARGS% /Fe: a.exe ..\..\src\**.cc ..\..\assets\monitor-controller.res user32.lib ws2_32.lib shell32.lib
