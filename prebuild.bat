echo off
mkdir build\assets
cd build\assets
xcopy /e /y ..\..\..\assets .
cd ..
xcopy /e /y ..\..\bin .