static const Int_t kNPMTs = 15;
static const Int_t kMaxNHits = 200;

Int_t    pmt_hits[kNPMTs];
Double_t pmt_ratio[kNPMTs];
Double_t pmt_time[kNPMTs][kMaxNHits];
Double_t pmt_peak[kNPMTs][kMaxNHits];
Double_t pmt_integral[kNPMTs][kMaxNHits];
Bool_t   pmt_flag[kNPMTs];
Double_t pmt_FWHM[kNPMTs][kMaxNHits];
Bool_t   inBeamWindow;
Bool_t   isBeamTrigger;
Double_t rf_time;

Double_t Weight(Int_t pmt, Int_t hit=0) {
  if( !pmt_flag[pmt] ) return 0;
  if( !inBeamWindow ) return 0;
  if( isBeamTrigger ) return 0;

  return 1.;
}

void lifetime_fit() {
  gROOT -> ProcessLine( ".X chainFiles.C" );
  cout << "#Ev: " << pdsEvTree -> GetEntries() << endl;

  pdsEvTree->SetBranchStatus("*", kFALSE);

  pdsEvTree->SetBranchAddress("pmt_hits",pmt_hits);
  pdsEvTree->SetBranchAddress("pmt_ratio",pmt_ratio);
  pdsEvTree->SetBranchAddress("pmt_time",pmt_time);
  pdsEvTree->SetBranchAddress("pmt_peak",pmt_peak);
  pdsEvTree->SetBranchAddress("pmt_integral",pmt_integral);
  pdsEvTree->SetBranchAddress("pmt_flag",pmt_flag);
  pdsEvTree->SetBranchAddress("pmt_FWHM",pmt_FWHM);
  pdsEvTree->SetBranchAddress("inBeamWindow",&inBeamWindow);
  pdsEvTree->SetBranchAddress("isBeamTrigger",&isBeamTrigger);
  pdsEvTree->SetBranchAddress("rf_time",&rf_time);

  pdsEvTree->SetBranchStatus("pmt_hits",kTRUE);
  pdsEvTree->SetBranchStatus("pmt_ratio",kTRUE);
  pdsEvTree->SetBranchStatus("pmt_time",kTRUE);
  pdsEvTree->SetBranchStatus("pmt_peak",kTRUE);
  pdsEvTree->SetBranchStatus("pmt_integral",kTRUE);
  pdsEvTree->SetBranchStatus("pmt_flag",kTRUE);
  pdsEvTree->SetBranchStatus("pmt_FWHM",kTRUE);
  pdsEvTree->SetBranchStatus("inBeamWindow",kTRUE);
  pdsEvTree->SetBranchStatus("isBeamTrigger",kTRUE);
  pdsEvTree->SetBranchStatus("rf_time",kTRUE);

  Double_t ymin = 0;
  Double_t ymax = 30;
  Double_t xmin = -1.7e3;
  Double_t xmax = 2.8e3;
  Int_t nbinsx = (xmax - xmin);
  Int_t nbinsy = 250;
  
  TH1F* h = new TH1F("h",";time;mean pe",nbinsx,xmin,xmax);
  h->Sumw2();

  TF1* f = new TF1("f","[0]*(exp(-(x-[5])/[2]) - exp(-(x-[5])/[4])) + [1]*(exp(-(x-[5])/[3])-exp(-(x-[5])/[4]))",xmin,xmax);

  // Loop over events
  Double_t t0 = 0;
  Double_t n  = 0;
  pdsEvTree->GetEntry(0);
  for( Int_t i = 0; pdsEvTree->GetEntry(i); i++ ) {
    // Loop over pmts
    for( Int_t pmt = 0; pmt < kNPMTs; pmt++ ) {
      // Status update
      if( i%(pdsEvTree->GetEntriesFast()/10) == 0 ) {
	cout << i << "of" << pdsEvTree->GetEntries() << endl;
	cout << "peak " << pmt_peak[pmt][0] << endl;
	cout << "integral " << pmt_integral[pmt][0] << endl;
	cout << "rf " << rf_time << "\ttime " << pmt_time[pmt][0] << endl;
	if(n!=0) cout << t0/n << endl;
      }
      // Loop over hits, if flagged
      if( Weight(pmt) ) {
	t0 += (pmt_time[pmt][0]-rf_time)*pmt_peak[pmt][0];
	n  += pmt_peak[pmt][0];
	for( Int_t j = 0; j < pmt_hits[pmt]; j++) {
	    Double_t tof = pmt_time[pmt][j] - rf_time;
	    h->Fill(tof,pmt_peak[pmt][j]);
	}
      }
    }
  }
  t0 = -665;

  f->SetRange(t0,t0+1600);
  f->FixParameter(5,t0);
  f->SetParameters(1,1,4,1600,0.2,t0);
  
  h->Draw("e");
  h->Fit("f","rs");
}  
