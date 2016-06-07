#!/bin/bash
datadir=/project/projectdirs/captain/data/2016/pmt/2016WNRRun2
fileno=0

if [ ! $1 ]; then
    root -l "PDSAnalysis.cc+(\"outFile_1.root\",1,\"pdsEvTree_.root\",\"v\")"
else
    datadir=/project/projectdirs/captain/data/2016/pmt/2016WNRRun2
    #datadir=/project/projectdirs/captain/data/2016/pmt/Cosmic
    if [ -e $datadir/run$1/outFile_$fileno.root ]; then
	root -l "PDSAnalysis.cc+(\"$datadir/run$1/outFile_$fileno.root\",$1,\"pdsEvTree_.root\",\"v\")"
    else 
	datadir=/project/projectdirs/captain/data/2016/pmt/2016WNRRun
	if [ -e $datadir/run$1/outFile_$fileno.root ]; then
            root -l "PDSAnalysis.cc+(\"$datadir/run$1/outFile_$fileno.root\",$1,\"pdsEvTree_.root\",\"v\")"
	else
	    echo "File for run $1 not found!"
	fi
    fi
fi
