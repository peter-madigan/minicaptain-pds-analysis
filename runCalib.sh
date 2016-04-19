#!/bin/bash
source /project/projectdirs/captain/releases/CAPTAIN/captain.profile

# single pe calibration

runs=( 9996 9995 9994 9992 9991 )

cd /global/homes/p/pmadigan/work/pds_analysis/pds_analysisCode

datadir=/project/projectdirs/captain/data/2016/pmt/Cosmic
outdir=./calib

for runno in "${runs[@]}"; do
    echo "Checking for run $runno..."
    if [ -d $datadir/run$runno ]; then
	echo "Run found! Submitting run $runno..."
	qsub -v runnumber="$runno" -v datadir="$datadir" -l projectio=1 -j y -o "./logs" runSingle.sh
    fi
done

echo "Waiting for queue..."
progress=`qstat -u pmadigan`
while [ "$progress" != "" ]; do
    sleep 1
    progress=`qstat -u pmadigan`
done
root -q -b macros/pmt_calib.C

# rate calibration

runs=( 9993 )

for runno in "${runs[@]}"; do
    echo "Checking for run $runno..."
    if [ -d $datadir/run$runno ]; then
        echo "Run found!"
        i=0
        mkdir -v $outdir/pdsTree$runno
        for infile in `ls "$datadir/run$runno"`; do
            if [ -f "$infile" ] && [ -s "$infile" ] && [ "${infile: -5}" == ".root" ]; then
                root -q -b "PDSAnalysis.cc+(\"$datadir/run$runno/$infile\",$runno,\"$outdir/pdsTree$runno/pdsEvTree_$i.root\")"
                let "i++"
            fi
        done
    fi
done

root -q -b macros/pmt_rate.C