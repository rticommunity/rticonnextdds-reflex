RTI RefleX
====================

Reflection-based type modeling for DDS-XTypes. Current support for C++ only.

Windows Build
1. Use at least VS2013 preview
2. Set REFLEXHOME, BOOSTHOME (1.46+), and NDDSHOME (5.0.0+)
3. Set PATH=%PATH%;%REFLEXHOME%\objs\i86Win32VS2013;%NDDSHOME%\lib\i86Win32VS2012
4. Open win32\reflex-vs2013.sln

Linux Build
1. Recommended compilers: g++ 4.8, clang 3.3 or better
2. export REFLEXHOME, BOOSTHOME (1.46+) and NDDSHOME (5.0.0+)
3. export ARCH=i86Linux2.6gcc4.4.5, for example.
4. See make/Makefile* for details
5. export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$NDDSHOME/lib/$ARCH:$REFLEXHOME/objs/$ARCH
6. Example: make -f make/Makefile.i86Linux-gcc CXX=g++48 (g++48 must be in your $PATH)

