#!/bin/bash
source /project/projectdirs/captain/releases/CAPTAIN/captain.profile

cd /global/homes/p/pmadigan/work/pds_analysis/pds_analysisCode

datadir=/project/projectdirs/captain/data/2016/pmt/Cosmic
outdir=./calib

root -q -b PDSAnalysis.cc+

# rate calibration

runs=( 90007 ) #( 9993 9986 9987 9988 )

for runno in "${runs[@]}"; do
    echo "Checking for run $runno..."
    if [ -d $datadir/run$runno ]; then
        echo "Run found!"
        i=0
        mkdir -v $outdir/pdsTree$runno
        for infile in `ls "$datadir/run$runno"`; do
	    echo "Checking file $infile..."
            if [ "${infile: -5}" == ".root" ]; then
                root -q -l -b "PDSAnalysis.cc+(\"$datadir/run$runno/$infile\",$runno,\"$outdir/pdsTree$runno/pdsEvTree_$i.root\",\"s r\")"
                let "i++"
            fi
        done
    fi
done

root -l macros/pmt_rate.C
