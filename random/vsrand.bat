@echo off

if "#%1" == "#/h" echo "vsrand.bat <number> [Debug|Release] [x64|Win32]" & goto exit

if "#%1" == "#" (set RANDOM_SEED=%RANDOM%) else (set RANDOM_SEED=%1)
if "#%2" == "#" (set BUILD_CONF="Release|x64") else (set BUILD_CONF="%2|%3")

echo Testing with RANDOM_SEED=%RANDOM_SEED% and %BUILD_CONF%
echo %RANDOM_SEED% >> RANDOM_SEEDS.txt 
REM The following command updates the timestamp of reflex_prop_test.cxx
REM but copy trick does not work.
REM @copy /b test\reflex_prop_test.cxx  +,, 
touch test\reflex_prop_test.cxx
devenv win\reflex-vs2015.sln /build %BUILD_CONF% /project reflex-prop-test 
win\x64\Release\reflex-prop-test 1000
set BUILD_CONF=

:exit