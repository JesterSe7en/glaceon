#!/bin/bash

SOURCE_DIR="./assimp"
GENERATOR="Ninja"

BINARIES_DIR="./assimp/build"
cmake . -G "$GENERATOR" -A x64 -S "$SOURCE_DIR" -B "$BINARIES_DIR" -DASSIMP_INSTALL=OFF -DBUILD_SHARED_LIBS=OFF
cmake --build "$BINARIES_DIR" --config debug
cmake --build "$BINARIES_DIR" --config release

cp assimp/build/include/assimp/config.h assimp/include/
mkdir -p ./assimp/libs/
cp -r assimp/build/lib/* assimp/libs/
