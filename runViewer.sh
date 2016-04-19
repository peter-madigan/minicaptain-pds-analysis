#!/bin/bash
datadir=/project/projectdirs/captain/data/2016/pmt/2016WNRRun2
datadir=/project/projectdirs/captain/data/2016/pmt/Cosmic
fileno=2

if [ ! $@ ]; then
    root -l "PDSAnalysis.cc+(\"outFile_1.root\",1,\"pdsEvTree_.root\",false,true)"
else
    root -l "PDSAnalysis.cc+(\"$datadir/run$@/outFile_$fileno.root\",$@,\"pdsEvTree_.root\",true,true)"
fi
