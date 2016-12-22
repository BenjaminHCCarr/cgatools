@echo off

rem cmake -G "Visual Studio 9 2008 Win64"

mkdir build32
pushd build32

cmake -G "Visual Studio 9 2008" ..

popd
