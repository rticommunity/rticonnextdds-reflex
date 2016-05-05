#!/bin/bash

if [ "$#" -ge "1" ]; then 
  begin=$1
else
  begin=1
fi

if [ "$#" -eq "2" ]; then 
  end=$2
else
  end=65535
fi

for i in `seq $begin $end`;
do
  touch test/reflex_prop_test.cxx 
  
  make -f make/Makefile.i86Linux-gcc CXX=g++5 CXXSTD=c++14 RANDOM=$i prop_test 
  
  if [ "$?" -ne "0" ]; then 
    make -f make/Makefile.i86Linux-gcc CXX=g++5 CXXSTD=c++14 RANDOM=$i prop_test 2>&1 > compile_$i.txt
    continue
  fi
  
  objs/i86Linux3gcc4.8.2/reflex_prop_test 1
  
  if [ "$?" -ne "0" ]; then 
    objs/i86Linux3gcc4.8.2/reflex_prop_test 1 > run_$i.txt
  fi
done
