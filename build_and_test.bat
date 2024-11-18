@echo off
setlocal enabledelayedexpansion

:: Enable more detailed output
set VERBOSE=true
echo Starting build and test process...
echo Current directory: %CD%

:: Set paths
set PROJECT_DIR=%~dp0
set BUILD_DIR=%PROJECT_DIR%build
set CMAKE_DIR=%PROJECT_DIR%cmake
set CMAKE_EXE=%CMAKE_DIR%\bin\cmake.exe
set ANALYZER=%BUILD_DIR%\bin\macho_analyzer.exe
set TEST_FILE=%PROJECT_DIR%zh_mac

echo.
echo Using following paths:
echo Project directory: %PROJECT_DIR%
echo Build directory: %BUILD_DIR%
echo CMake directory: %CMAKE_DIR%
echo CMake executable: %CMAKE_EXE%
echo Analyzer path: %ANALYZER%
echo Test file: %TEST_FILE%
echo.

:: Check if CMake exists
echo Checking for CMake...
if not exist "%CMAKE_EXE%" (
    echo Error: CMake not found at: %CMAKE_EXE%
    echo Please ensure CMake is installed in the cmake directory
    echo Press any key to exit...
    pause > nul
    exit /b 1
)
echo CMake found at: %CMAKE_EXE%

:: Clean and recreate build directory
echo.
echo Cleaning build directory...
if exist "%BUILD_DIR%" (
    echo Removing old build directory...
    rd /s /q "%BUILD_DIR%"
    if errorlevel 1 (
        echo Error: Failed to remove build directory
        echo Make sure no programs are using files in the build directory
        echo Press any key to exit...
        pause > nul
        exit /b 1
    )
)
echo Creating fresh build directory...
mkdir "%BUILD_DIR%"
cd "%BUILD_DIR%"
echo Current directory: %CD%
echo.

:: Configure and build
echo Configuring project with CMake...
"%CMAKE_EXE%" .. -G "Visual Studio 17 2022" -A x64
if errorlevel 1 (
    echo Error: CMake configuration failed with error code %errorlevel%
    echo Press any key to exit...
    pause > nul
    exit /b 1
)

echo.
echo Building project...
"%CMAKE_EXE%" --build . --config Debug
if errorlevel 1 (
    echo Error: Build failed with error code %errorlevel%
    echo Press any key to exit...
    pause > nul
    exit /b 1
)

:: Check if analyzer was built
echo.
echo Checking for analyzer executable...
if not exist "%ANALYZER%" (
    echo Error: Analyzer executable not found at: %ANALYZER%
    echo Build might have succeeded but executable is not in expected location
    echo Press any key to exit...
    pause > nul
    exit /b 1
)
echo Analyzer executable found.

:: Check if test file exists
echo.
echo Checking for test file...
if not exist "%TEST_FILE%" (
    echo Error: Test file not found at: %TEST_FILE%
    echo Press any key to exit...
    pause > nul
    exit /b 1
)
echo Test file found.

:: Run the analyzer
echo.
echo Running analysis on: %TEST_FILE%
echo Command: "%ANALYZER%" -i "%TEST_FILE%"
"%ANALYZER%" -i "%TEST_FILE%"
if errorlevel 1 (
    echo Error: Analyzer failed with error code %errorlevel%
    echo Press any key to exit...
    pause > nul
    exit /b 1
)

:: Check if JSON was created
echo.
echo Checking for output JSON file...
if exist "%TEST_FILE%.json" (
    echo Analysis complete! Results written to: %TEST_FILE%.json
    
    :: Optional: Open the JSON file in default editor
    echo Opening results...
    start "" "%TEST_FILE%.json"
) else (
    echo Error: Analysis failed - no output file created
    echo Press any key to exit...
    pause > nul
    exit /b 1
)

echo.
echo Process completed successfully!
echo Press any key to exit...
pause > nul
endlocal