@echo off

rem Get the directory of the batch file
set "BatchDir=%~dp0"

rem Loop through each bitmap file in the directory
for %%F in ("%BatchDir%*.bmp") do (
    echo Processing file: %%F
    rem Run BMP2Cpp.exe on the current file
    "%BatchDir%BMP2Cpp.exe" "%%F"
)
