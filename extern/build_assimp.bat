SET SOURCE_DIR=./assimp
SET GENERATOR=Visual Studio 17 2022

SET BINARIES_DIR="./assimp/build"
cmake . -G "%GENERATOR%" -A x64 -S %SOURCE_DIR% -B %BINARIES_DIR% -DASSIMP_INSTALL=OFF -DBUILD_SHARED_LIBS=OFF
cmake --build %BINARIES_DIR% --config debug
cmake --build %BINARIES_DIR% --config release

copy assimp\build\include\assimp\config.h assimp\include\
mkdir .\assimp\libs\
xcopy assimp\build\lib assimp\libs /s /e