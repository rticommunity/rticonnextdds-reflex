#!/bin/bash
if [ $# != 4 ]
then 
  echo " provide domain-ID follwed by # of samples to be received followed by # of
  readers and frequency in Hertz"
  exit
fi
DOMAIN=$1
SAMPLE_COUNT=$2
READER_COUNT=$3
HERTZ=$4
for i in `seq  $READER_COUNT`
do
  ../objs/x64Linux2.6gcc4.4.5/reflex_test $DOMAIN qsperf sub $SAMPLE_COUNT $i > output_$i.txt &
 done
#start the QS
 gnome-terminal -x sh -c "$NDDSHOME/bin/rtiqueuingservice -cfgFile QsPerf.xml -cfgName QsPerf ;bash"
#start the writer
 gnome-terminal -e "bash -c \"../objs/x64Linux2.6gcc4.4.5/reflex_test $DOMAIN qsperf pub $SAMPLE_COUNT $READER_COUNT $HERTZ; exec bash\""

