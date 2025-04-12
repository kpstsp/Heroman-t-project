@echo off
setlocal

REM Create assets directory if it doesn't exist
if not exist assets mkdir assets

REM Check if font file exists
if not exist assets\font.ttf (
    echo Downloading font file...
    curl -L -o assets\font.ttf https://github.com/googlefonts/roboto/raw/main/src/hinted/Roboto-Regular.ttf
    if %ERRORLEVEL% neq 0 (
        echo Failed to download font file
        echo Please manually download a font file and place it in the assets directory as font.ttf
        exit /b 1
    )
)

echo Assets setup complete! 