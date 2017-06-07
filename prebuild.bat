echo off
mkdir build\assets
cd build\assets
xcopy /e /y /d /f /l ..\..\..\assets .
cd ..
xcopy /e /y ..\..\bin .