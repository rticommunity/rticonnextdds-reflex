@echo off

if "#%1" == "#" (set RANDOM_SEED=%RANDOM%) else (set RANDOM_SEED=%1)
echo Testing with RANDOM_SEED=%RANDOM_SEED% 
echo %RANDOM_SEED% >> RANDOM_SEEDS.txt 
REM The following command updates the timestamp of reflex_prop_test.cxx
REM but copy trick does not work.
REM @copy /b test\reflex_prop_test.cxx  +,, 
touch test\reflex_prop_test.cxx
devenv win\reflex-vs2015.sln /build "Release|x64" /project reflex-prop-test 
win\x64\Release\reflex-prop-test 1000
