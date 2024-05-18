@echo off
setlocal enabledelayedexpansion

rem Set the output file name
set output=out.h

rem Create an empty output file
type nul > %output%

rem Flag to determine if it's the first file
set "firstFile=true"

rem Loop through all TXT files in the current directory
for %%f in (*.txt) do (
    rem Skip the batch file itself
    if not "%%~nf"=="%~n0" (
        rem Check if it's the first file
        if "!firstFile!"=="true" (
            set "firstFile=false"
            rem Copy the content of the first text file to the output file without skipping lines
            type "%%f" >> %output%
        ) else (
            rem Copy the content of subsequent text files to the output file, skipping the first six lines
            for /f "skip=6 tokens=*" %%l in (%%f) do (
                echo %%l >> %output%
            )
        )
        rem Delete the processed text file
        del "%%f"
    )
)

endlocal
