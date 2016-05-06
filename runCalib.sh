#!/bin/bash
source /project/projectdirs/captain/releases/CAPTAIN/captain.profile

# single pe calibration

runs=( 9989 9996 9995 9994 9992 9991 9990 )

cd /global/homes/p/pmadigan/work/pds_analysis/pds_analysisCode

datadir=/project/projectdirs/captain/data/2016/pmt/Cosmic
outdir=./calib

root -q -b PDSAnalysis.cc+

for runno in "${runs[@]}"; do
    echo "Checking for run $runno..."
    if [ -d $datadir/run$runno ]; then
	echo "Run found! Submitting run $runno..."
	qsub -v runnumber="$runno" -v datadir="$datadir" -v opt="c s r" -l projectio=1 -j y -o "./logs" runSingle.sh
    fi
done

echo "Waiting for queue..."
progress=`qstat -u pmadigan`
while [ "$progress" != "" ]; do
    qstat -u pmadigan
    sleep 10
    progress=`qstat -u pmadigan`
done
root -q -l -b macros/pmt_calib.C

