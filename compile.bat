@echo off

REM Create obj directory if it doesn't exist
if not exist obj mkdir obj

REM Clean up previous build
if exist class_scheduler.exe del class_scheduler.exe
if exist obj\*.o del obj\*.o

REM Set raylib paths - update these to match your Windows installation
set RAYLIB_PATH=C:\raylib

REM Compile all source files
echo Compiling source files...
for %%f in (src\*.cpp) do (
    echo Compiling %%f...
    g++ -c "%%f" -o "obj\%%~nf.o" -std=c++11 -Wall -I./ -I"%RAYLIB_PATH%\include"
)

REM Link object files with raylib
echo Linking...
g++ obj\*.o -o class_scheduler.exe -L"%RAYLIB_PATH%\lib" -lraylib -lopengl32 -lgdi32 -lwinmm

echo Build complete! Run with class_scheduler.exe 