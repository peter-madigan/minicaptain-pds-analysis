static const Int_t kNPMTs = 15;
static const Int_t kMaxNHits = 1000;

Int_t pmt_hits[kNPMTs];
Double_t pmt_ratio[kNPMTs];
Double_t pmt_time[kNPMTs][kMaxNHits];
Double_t pmt_peak[kNPMTs][kMaxNHits];
Double_t pmt_integral[kNPMTs][kMaxNHits];
Bool_t   pmt_flag[kNPMTs];
Bool_t   inBeamWindow;
Bool_t   isBeamTrigger;
Double_t rf_time;

Double_t Weight(Int_t pmt, Int_t hit=0) {
  if( !pmt_flag[pmt] ) return 0;
  if( !inBeamWindow ) return 0;
  if( isBeamTrigger ) return 0;
  if( pmt_peak[pmt][hit] < 0 ) return 0;

  return 1.;
}

void tof_charge() {
  gROOT -> ProcessLine( ".X chainFiles.C" );
  cout << "#Ev: " << pdsEvTree -> GetEntries() << endl;

  pdsEvTree->SetBranchAddress("pmt_hits",pmt_hits);
  pdsEvTree->SetBranchAddress("pmt_ratio",pmt_ratio);
  pdsEvTree->SetBranchAddress("pmt_peak",pmt_peak);
  pdsEvTree->SetBranchAddress("pmt_integral",pmt_integral);
  pdsEvTree->SetBranchAddress("pmt_flag",pmt_flag);
  pdsEvTree->SetBranchAddress("inBeamWindow",&inBeamWindow);
  pdsEvTree->SetBranchAddress("isBeamTrigger",&isBeamTrigger);
  pdsEvTree->SetBranchAddress("rf_time",&rf_time);

  Double_t ymin = 0;
  Double_t ymax = 1e2;
  Double_t xmin = -1.7e3;
  Double_t xmax = 100;
  Int_t nbinsx = (xmax - xmin)/2;
  Int_t nbinsy = 150;
  
  TH2F* h_integral = new TH2F("h_integral",";uncal. tof;integral;",nbinsx,xmin,xmax,nbinsy,ymin,ymax*1e2);
  TH2F* h_peak = new TH2F("h_peak",";uncal. tof;height",nbinsx,xmin,xmax,nbinsy,ymin,ymax);
  TH2F* h_hits = new TH2F("h_hits",";uncal. tof;hits",nbinsx,xmin,xmax,nbinsy,ymin,ymax);
  TH2F* h_ratio = new TH2F("h_ratio",";uncal. tof;pulse ratio",nbinsx,xmin,xmax,nbinsy,-2,2);
  TH1F* h_tof = new TH1F("h_tof",";uncal. tof",nbinsx,xmin,xmax);
  TH2F* h_shape = new TH2F("h_shape",";integral;height",nbinsy,ymin,ymax*10,nbinsy,ymin,ymax);
  
  pdsEvTree->GetEntry(0);
  for( Int_t i = 0; pdsEvTree->GetEntry(i); i++ ) {
    for( Int_t pmt = 0; pmt < 15; pmt++ ) {
      for( Int_t j = 0; j < pmt_hits[pmt]; j++) {
	Double_t tof = pmt_time[pmt][j] - rf_time;
	h_integral->Fill(tof,pmt_integral[pmt][j],Weight(pmt,j));
	h_peak->Fill(tof,pmt_peak[pmt][j],Weight(pmt,j));
	h_ratio->Fill(tof,pmt_ratio[pmt],Weight(pmt));
	h_tof->Fill(tof,Weight(pmt,j));
	h_shape->Fill(pmt_integral[pmt][j],pmt_peak[pmt][j],Weight(pmt,j));
	h_hits->Fill(tof,pmt_hits[pmt],Weight(pmt));
	if( i%(pdsEvTree->GetEntries()/100) == 0 ) {
          cout << i << "of" << pdsEvTree->GetEntries() << endl;
	  cout << "tof " << tof << endl;
	  cout << "peak " << pmt_peak[pmt][j] << endl;
	  cout << "integral " << pmt_integral[pmt][j] << endl;
	  cout << "rf " << rf_time << "\ttime" << pmt_time[pmt][j] << endl;
	}
      }
    }
  }
  
  h_integral->Draw("colz");
  c1->SetLogz();
  c1->DrawClone();
  
  c1->cd();
  h_peak->Draw("colz");
  c1->DrawClone();

  c1->cd();
  h_ratio->Draw("colz");
  c1->DrawClone();

  c1->cd();
  h_tof->Draw("e");
  c1->cd()->SetLogy();
  c1->DrawClone();

  c1->cd()->SetLogy(0);
  h_shape->Draw("colz");
  c1->DrawClone();

  c1->cd();
  h_hits->Draw("colz");
  c1->DrawClone();

}  
