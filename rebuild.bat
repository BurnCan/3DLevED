@echo off
setlocal

:: Prompt for repo URL
set /p REPO_URL=Enter the Git repository URL (e.g., https://github.com/yourusername/your-repo.git): 
if "%REPO_URL%"=="" (
    echo Repository URL is required.
    exit /b
)

:: Prompt for branch
set /p BRANCH=Enter the branch to clone (default: main): 
if "%BRANCH%"=="" set BRANCH=main

:: Extract repo name
for %%A in (%REPO_URL%) do set "REPO_NAME=%%~nA"

:: Create temp directory
set TMP_DIR=%TEMP%\repo_clone_%RANDOM%
mkdir "%TMP_DIR%"
copy "%~f0" "%TMP_DIR%\" >nul
cd /d "%TMP_DIR%"

echo Cloning branch %BRANCH% from %REPO_URL%...
git clone --branch %BRANCH% --single-branch %REPO_URL%

cd "%REPO_NAME%" || exit /b

:: Build
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
mingw32-make -j4

echo Build complete.

:: Copy output to the repo-named directory
set OUTPUT_DIR=%TMP_DIR%\%REPO_NAME%
mkdir "%OUTPUT_DIR%"
xcopy /E /I /Y ..\bin\* "%OUTPUT_DIR%"

echo Build files copied to %OUTPUT_DIR%.

:: Automatically remove temp directory
cd /d %TEMP%
rmdir /s /q "%TMP_DIR%"
echo Removed %TMP_DIR%.

endlocal
