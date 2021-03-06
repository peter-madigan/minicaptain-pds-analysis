#!/bin/bash
cd /global/homes/p/pmadigan/work/pds_analysis/pds_analysisCode
source /project/projectdirs/captain/releases/CAPTAIN/captain.profile

if [ "$runnumber" == "" ]; then
    runno="$@"
else
    runno="$runnumber"
fi

if [ "$datadir" == "" ]; then
    datadirs=( /project/projectdirs/captain/data/2016/pmt/2016WNRRun2 /project/projectdirs/captain/data/2016/pmt/2016WNRRun /project/projectdirs/captain/data/2016/pmt/Cosmic )
    outdir=./data
else
    datadirs=( "$datadir" )
    outdir=./calib
fi

echo "Checking for run $runno..."
for datadir in "${datadirs[@]}"; do
    if [ -d $datadir/run$runno ]; then
	echo "Run found!"
	i=0
	mkdir -v $outdir/pdsTree$runno
	for infile in `ls $datadir/run$runno`; do
	    echo "Checking $infile..."
	#if [ ! -f $infile ]; then
	#    echo "...is directory. Skipping..."
	#if [ ! -s $infile ]; then
	#    echo "...is empty. Skipping..."
	    if [ ! ${infile: -5} == ".root" ]; then
		echo "...is a ${infile: -5} file. Skipping..."
	    else
		root -q -b "PDSAnalysis.cc+(\"$datadir/run$runno/$infile\",$runno,\"$outdir/pdsTree$runno/pdsEvTree_$i.root\",\"$opt\")"
		let "i++"
	    fi
	done
    else
	echo "ERROR: Run not found!"
    fi
done

echo "Done!"