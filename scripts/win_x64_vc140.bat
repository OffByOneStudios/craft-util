SET PROJECT_ROOT=%cd%
SET TRIPLE_CROSS=win_x64_vc140

mkdir build
cd build
cmake .. -DCMAKE_GENERATOR_PLATFORM=x64

msbuild craft_util.vcxproj /t:Build /p:Configuration=Debug /property:Platform=x64
msbuild craft_util.vcxproj /t:Build /p:Configuration=Release /property:Platform=x64


mkdir %PROJECT_ROOT%\out\include
mkdir %PROJECT_ROOT%\out\bin\%TRIPLE_CROSS%\debug
mkdir %PROJECT_ROOT%\out\lib\%TRIPLE_CROSS%\debug

COPY %PROJECT_ROOT%\build\Debug\craft_util.dll %PROJECT_ROOT%\out\bin\%TRIPLE_CROSS%\debug\
COPY %PROJECT_ROOT%\build\Debug\craft_util.pdb %PROJECT_ROOT%\out\bin\%TRIPLE_CROSS%\debug\
COPY %PROJECT_ROOT%\build\Debug\craft_util.lib %PROJECT_ROOT%\out\lib\%TRIPLE_CROSS%\debug\

COPY %PROJECT_ROOT%\build\Release\craft_util.dll %PROJECT_ROOT%\out\bin\%TRIPLE_CROSS%\
COPY %PROJECT_ROOT%\build\Release\craft_util.pdb %PROJECT_ROOT%\out\bin\%TRIPLE_CROSS%\
COPY %PROJECT_ROOT%\build\Release\craft_util.lib %PROJECT_ROOT%\out\lib\%TRIPLE_CROSS%\

ROBOCOPY /e %PROJECT_ROOT%\src  %PROJECT_ROOT%\out\include\util\ *.h *.hpp

cd %PROJECT_ROOT%
