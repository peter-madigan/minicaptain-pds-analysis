#!bin/bash
cd /global/homes/p/pmadigan/work/pds_analysis/pds_analysisCode
source /project/projectdirs/captain/releases/CAPTAIN/captain.profile

if [ "$runnumber" == "" ]; then
    runno="$@"
else
    runno="$runnumber"
fi

datadir=/project/projectdirs/captain/data/2016/pmt/2016WNRRun2
outdir=./data

echo "Checking for run $runno..."
if [ -d $datadir/run$runno ]; then
    echo "Run found!"
    i=0
    mkdir -v $outdir/pdsTree$runno
    for infile in `ls "$datadir/run$runno"`; do
	if [ -f "$infile" ] && [ -s "$infile" ] && [ "${infile: -5}" == ".root" ]; then
	    root -q -b "PDSAnalysis.cc+(\"$datadir/run$runno/$infile\",$runno,\"$outdir/pdsTree$runno/pdsEvTree_$i.root\",true)"
	    let "i++"
	fi
    done
else
    echo "ERROR: Run not found!"
fi
