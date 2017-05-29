@ECHO OFF

REM Staring CI
SET PROJECT_ROOT=%cd%
SET PROJECT_SHORTNAME=base
SET TRIPLE_CROSS=win_x64_vc140
SET GEN_STATUS=2
SET BUILD_DEBUG_STATUS=2
SET BUILD_RELEASE_STATUS=2
SET TEST_DEBUG_STATUS=2
SET TEST_RELEASE_STATUS=2

if not defined VSINSTALLDIR (call vcvarsall.bat amd64)
if exist build ( rmdir /s/q build )
if exist %TRIPLE_CROSS%.zip (del %TRIPLE_CROSS%.zip)

mkdir build
cd build

REM Running Generation
cmake .. -DCMAKE_GENERATOR_PLATFORM=x64 > gen.txt 2>&1
if NOT %ERRORLEVEL% EQU 0 (
  SET GEN_STATUS=1
  GOTO end
)
SET GEN_STATUS=0

REM Building Debug
msbuild Project.sln /t:Build /p:Configuration=Debug /property:Platform=x64 /property:PlatformToolset=v140 > build_debug.txt 2>&1
if NOT %ERRORLEVEL% EQU 0 (
  SET BUILD_DEBUG_STATUS=1
  GOTO fail
)
SET BUILD_DEBUG_STATUS=0

REM Building Release
msbuild Project.sln /t:Build /p:Configuration=Release /property:Platform=x64 /property:PlatformToolset=v140 > build_release.txt 2>&1
if NOT %ERRORLEVEL% EQU 0 (
  SET BUILD_DEBUG_STATUS=1
  GOTO fail
)
SET BUILD_RELEASE_STATUS=0

REM Running Tests
Debug\CraftCmakeBaseTests > test_debug.txt 2>&1
if NOT %ERRORLEVEL% EQU 0 (
  SET TEST_DEBUG_STATUS=1
  GOTO fail
)
SET TEST_DEBUG_STATUS=0


Release\CraftCmakeBaseTests > test_release.txt 2>&1
if NOT %ERRORLEVEL% EQU 0 (
  SET TEST_RELEASE_STATUS=1
  GOTO fail
)
SET TEST_RELEASE_STATUS=0

REM Gathering
mkdir %PROJECT_ROOT%\dist\include
mkdir %PROJECT_ROOT%\dist\bin\%TRIPLE_CROSS%\debug
mkdir %PROJECT_ROOT%\dist\lib\%TRIPLE_CROSS%\debug

COPY %PROJECT_ROOT%\build\Debug\*.dll %PROJECT_ROOT%\dist\bin\%TRIPLE_CROSS%\debug\
COPY %PROJECT_ROOT%\build\Debug\*.pdb %PROJECT_ROOT%\dist\bin\%TRIPLE_CROSS%\debug\
COPY %PROJECT_ROOT%\build\Debug\*.lib %PROJECT_ROOT%\dist\lib\%TRIPLE_CROSS%\debug\

COPY %PROJECT_ROOT%\build\Release\*.dll %PROJECT_ROOT%\dist\bin\%TRIPLE_CROSS%\
COPY %PROJECT_ROOT%\build\Release\*.lib %PROJECT_ROOT%\dist\lib\%TRIPLE_CROSS%\

ROBOCOPY /e %PROJECT_ROOT%\src  %PROJECT_ROOT%\dist\include\%PROJECT_SHORTNAME%\ *.h *.hpp

cd %PROJECT_ROOT%
7z a -tzip %TRIPLE_CROSS%.zip dist
GOTO end


:fail

GOTO end


:end
cd %PROJECT_ROOT%
