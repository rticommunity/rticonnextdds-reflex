RTI RefleX
====================

Reflection-based Type Modeling for DDS Topics.

RefleX is a short for Reflection for DDS-XTypes. The main objective of 
this library is to create DDS-XTypes compliant type representations 
directly from native C++ types. RefleX is declarative i.e., it is not 
a reflection API. There is no separate code generation step involved 
(other than compilation). The RefleX library will take your 
application-level datatypes in C++ and will happily map them 
to equivalent DDS topic types.

More reading:
- [RefleX announcement](http://blogs.rti.com/2014/03/22/rti-reflex-reflection-based-type-modeling-for-dds-topics/)
- [RefleX whitepaper](http://community.rti.com/paper/c-template-library-data-centric-type-modeling-dds-xtypes) 
- [RefleX presentation](http://www.slideshare.net/SumantTambe/overloading-in-overdrive-a-generic-datacentric-library-for)
- [RefleX tutorial](http://rticommunity.github.io/rticonnextdds-reflex)
- [Reflex API docs](http://rticommunity.github.io/rticonnextdds-reflex/docs/html/index.html)

### Windows Build
1. Use Visual Studio 2013 
2. Set REFLEXHOME, BOOSTHOME (1.46+), and NDDSHOME (5.1.0+)
3. Set PATH=%PATH%;%REFLEXHOME%\objs\i86Win32VS2013;%NDDSHOME%\lib\i86Win32VS2012
4. Run %NDDSHOME%\scripts\rtiddsgen -language C++ -namespace -d test idl\complex.idl
5. Open win32\reflex-vs2013.sln

### Linux Build
1. Recommended compilers: g++ 4.8, clang 3.3 or better
2. export REFLEXHOME, BOOSTHOME (1.46+) and NDDSHOME (5.0.0+)
3. export ARCH=i86Linux2.6gcc4.8.2, for example.
4. See make/Makefile* for details
5. export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$NDDSHOME/lib/$ARCH:$REFLEXHOME/objs/$ARCH
6. Build:               make -f make/Makefile.i86Linux-gcc CXX=g++48 (g++48 must be in your $PATH)
7. Build tests:         make/Makefile.i86Linux-gcc test
8. Build perf_test:     make/Makefile.i86Linux-gcc perf_test
9. Build property_test: make/Makefile.i86Linux-gcc prop_test CXX=g++5 CXXSTD=c++14 RANDOM=$RANDOM

