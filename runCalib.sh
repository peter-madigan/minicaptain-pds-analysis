#!bin/bash
source /project/projectdirs/captain/releases/CAPTAIN/captain.profile

runs=( 9996 9995 9994 9992 9991 )

cd /global/homes/p/pmadigan/work/pds_analysis/pds_analysisCode

datadir=/project/projectdirs/captain/data/2016/pmt/Cosmic
outdir=./calib

for runno in $runs; do
    mkdir -v $outdir/pdsTree$runno
    if [ -d $datadir/run$runno ]; then
	i=0
	for infile in `ls $datadir/run$runno`; do
	    if [ ${infile: -5} == ".root" ]; then
		root -q -b "PDSAnalysis.cc+(\"$datadir/run$runno/$infile\",$runno,\"$outdir/pdsTree$runno/pdsEvTree_$i.root\",true)"
		let "i++"
	    fi
	done
    fi
done

root macros/pmt_calib.C