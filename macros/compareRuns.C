#include <vector>
#include <iterator>
#include <iostream>

void compareRuns() {
  TChain* ch = new TChain("pdsEvTree","pdsEvTree");
  for(int i = 132; i < 323; i++) {
    cout << ch->Add(Form("~/work/pds_analysis/pds_analysisCode/data/pdsTree6%03d/pdsEvTree_*.root",i)) << endl;
  }

  TCanvas* c = new TCanvas("c","",1600,800);
  c->Divide(5,2);

  gStyle->SetOptStat(1110);
  gStyle->SetStatX(0.3);
  gStyle->SetStatY(0.9);
  
  vector<TString> runNumbers;
  runNumbers.push_back("runno >= 6235 && runno <= 6254");//low intensity
  runNumbers.push_back("runno >= 6139 && runno <= 6234");//med intensity
  runNumbers.push_back("runno >= 6255 && runno <= 6259");//high intensity
  runNumbers.push_back("runno >= 6132 && runno <= 6138");//HV off
  runNumbers.push_back("runno >= 6300 && runno <= 6323");//cosmic
  
  Int_t pad = 1;
  for( vector<TString>::iterator it = runNumbers.begin(); it != runNumbers.end(); ++it ) {
    cout << "Runs are " << *it << endl;
    
    c -> cd(pad) -> SetGridx(); c -> cd(pad) -> SetGridy();
    c -> cd(pad) -> SetLogy();
    Int_t nevent = ch->Draw(Form("peak>>hPeak_%d(100,-1000,0)",pad),"("+ *it +") * !isBeamTrigger","goff");
    if( nevent > 0 ) {
      gROOT->ProcessLine(Form("hPeak_%d->Sumw2(); hPeak_%d->Scale(1./%d);",pad,pad,nevent));
      gROOT->ProcessLine(Form("hPeak_%d->Draw(\"\");",pad));
      gROOT->ProcessLine(Form("hPeak_%d->GetYaxis()->SetRangeUser(1e-6,1);",pad));
      c -> cd(pad+5) -> SetGridx(); c -> cd(pad+5) -> SetGridy();
      c -> cd(pad+5) -> SetLogy();
      ch->Draw(Form("integral>>hInteg_%d(100,-5e3,0)",pad),"("+*it+") * !isBeamTrigger","goff");
      gROOT->ProcessLine(Form("hInteg_%d->Sumw2(); hInteg_%d->Scale(1./%d);",pad,pad,nevent));
      gROOT->ProcessLine(Form("hInteg_%d->Draw(\"\");",pad));
      gROOT->ProcessLine(Form("hInteg_%d->GetYaxis()->SetRangeUser(1e-6,1);",pad));
    }      
    pad++;
  }
}
