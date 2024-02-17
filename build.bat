@echo off

@REM Arguments passed: Project name to build, and deploy location
SET "PROJECT=%~1"
SET "DEPLOY_LOCATION=%~2"

@REM Check arguments validity
if "%PROJECT%" NEQ "NewVegasReloaded" (
    if "%PROJECT%" NEQ "OblivionReloaded" (
        echo "Project %PROJECT% invalid. Use 'NewVegasReloaded' or 'OblivionReloaded'."
        PAUSE
        exit /b %ERRORLEVEL%
    )
)

@REM build project
msbuild -target:%PROJECT% /property:Configuration=Release /property:Platform=x86 -m

if %ERRORLEVEL% NEQ 0 (
    echo Build has failed. Deploy aborted.
    PAUSE
    exit /b %ERRORLEVEL%
)

@REM Cleanup in case of previous build
rmdir "build" /s /q

if "%PROJECT%" == "NewVegasReloaded" (
    SET "FOLDER=nvse"
)
if "%PROJECT%" == "OblivionReloaded" (
    SET "FOLDER=obse"
)

@REM Copy dll & pdb
xcopy "%PROJECT%\Release\%PROJECT%.dll" "build\%PROJECT%\%FOLDER%\Plugins\" /y /d
xcopy "%PROJECT%\Release\%PROJECT%.pdb" "build\%PROJECT%\%FOLDER%\Plugins\" /y /d

@REM Copy config
xcopy "resources\%PROJECT%.dll.config" "build\%PROJECT%\%FOLDER%\Plugins\" /y /d
xcopy "resources\%PROJECT%.dll.defaults.toml" "build\%PROJECT%\%FOLDER%\Plugins\" /y /d
echo F|xcopy "resources\%PROJECT%.dll.defaults.toml" "build\%PROJECT%\%FOLDER%\Plugins\%PROJECT%.dll.toml" /y /d

@REM Copy Shaders
set GAME=%PROJECT:Reloaded=""%
robocopy /mir .\%GAME%\Shaders .\build\%PROJECT%\Shaders\%PROJECT%\Shaders
robocopy /mir .\%GAME%\Effects .\build\%PROJECT%\Shaders\%PROJECT%\Effects

@REM Copy Textures
robocopy /mir .\resources\Textures\ .\build\%PROJECT%\Textures\

@REM Copy to deploy if 2nd arg was passed
if "%DEPLOY_LOCATION%" NEQ "" (
    echo deploying to "%DEPLOY_LOCATION%"
    robocopy /mir "build\%PROJECT%\." "%DEPLOY_LOCATION%"
) else (
    echo "No deploy stage."
)

PAUSE

@REM handle Robocopy weird error codes
if (%ERRORLEVEL% LSS 8) exit /b 0
exit /b %ERRORLEVEL%
