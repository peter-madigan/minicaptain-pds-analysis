#!/bin/bash
cd /global/homes/p/pmadigan/work/pds_analysis/pds_analysisCode
source /project/projectdirs/captain/releases/CAPTAIN/captain.profile

root -q -b PDSAnalysis.cc+

for runno in "$@"; do
    echo "Submitting job for run $runno..."
    qsub -v runnumber="$runno" -v opt="" -l projectio=1 -j y -o "./logs" runSingle.sh
done
