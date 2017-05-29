@ECHO OFF

echo Staring CI
PROJECT_ROOT=$(pwd)
PROJECT_SHORTNAME=util
TRIPLE_CROSS=osx_x64_clang
GEN_STATUS=2
BUILD_DEBUG_STATUS=2
BUILD_RELEASE_STATUS=2
TEST_DEBUG_STATUS=2
TEST_RELEASE_STATUS=2

mkdir build
cd build

echo Running Generation

if ! cmake .. > gen.txt
then
  GEN_STATUS=1
  exit
fi
GEN_STATUS=0

echo Building Debug
if ! make -j4 > build_debug.txt 2>&1
then
  BUILD_DEBUG_STATUS=1
  exit
fi
BUILD_DEBUG_STATUS=0

echo Running Tests
# Debug\CraftCmakeBaseTests > test_debug.txt 2>&1
# if NOT %ERRORLEVEL% EQU 0 (
#   SET TEST_DEBUG_STATUS=1
#   GOTO fail
# )
# SET TEST_DEBUG_STATUS=0
#
#
# Release\CraftCmakeBaseTests > test_release.txt 2>&1
# if NOT %ERRORLEVEL% EQU 0 (
#   SET TEST_RELEASE_STATUS=1
#   GOTO fail
# )
TEST_RELEASE_STATUS=0

echo Gathering
mkdir -p $PROJECT_ROOT/dist/include/$PROJECT_SHORTNAME
mkdir -p $PROJECT_ROOT/dist/bin/$TRIPLE_CROSS/debug
mkdir -p $PROJECT_ROOT/dist/lib/$TRIPLE_CROSS/debug

# cp  $PROJECT_ROOT/buildDebug\*.dll %PROJECT_ROOT%\dist\bin\%TRIPLE_CROSS%\debug\
# COPY %PROJECT_ROOT%\build\Debug\*.pdb %PROJECT_ROOT%\dist\bin\%TRIPLE_CROSS%\debug\
# COPY %PROJECT_ROOT%\build\Debug\*.lib %PROJECT_ROOT%\dist\lib\%TRIPLE_CROSS%\debug\
#
cp $PROJECT_ROOT/build/*.dylib $PROJECT_ROOT/dist/lib/$TRIPLE_CROSS

cp -R $PROJECT_ROOT/src/*.h  $PROJECT_ROOT/dist/include/$PROJECT_SHORTNAME/
cp -R $PROJECT_ROOT/src/*.hpp  $PROJECT_ROOT/dist/include/$PROJECT_SHORTNAME/

cd $PROJECT_ROOT
tar -czvf $PROJECT_ROOT/$TRIPLE_CROSS.tar.gz dist
