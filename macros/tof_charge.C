static const Int_t kNPMTs = 15;
static const Int_t kMaxNHits = 200;

Int_t    pmt_hits[kNPMTs];
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

  return 1.;
}

void tof_charge() {
  gROOT -> ProcessLine( ".X chainFiles.C" );
  cout << "#Ev: " << pdsEvTree -> GetEntries() << endl;

  pdsEvTree->SetBranchStatus("*", kFALSE);

  pdsEvTree->SetBranchAddress("pmt_hits",pmt_hits);
  pdsEvTree->SetBranchAddress("pmt_ratio",pmt_ratio);
  pdsEvTree->SetBranchAddress("pmt_time",pmt_time);
  pdsEvTree->SetBranchAddress("pmt_peak",pmt_peak);
  pdsEvTree->SetBranchAddress("pmt_integral",pmt_integral);
  pdsEvTree->SetBranchAddress("pmt_flag",pmt_flag);
  pdsEvTree->SetBranchAddress("inBeamWindow",&inBeamWindow);
  pdsEvTree->SetBranchAddress("isBeamTrigger",&isBeamTrigger);
  pdsEvTree->SetBranchAddress("rf_time",&rf_time);

  pdsEvTree->SetBranchStatus("pmt_hits",kTRUE);
  pdsEvTree->SetBranchStatus("pmt_ratio",kTRUE);
  pdsEvTree->SetBranchStatus("pmt_time",kTRUE);
  pdsEvTree->SetBranchStatus("pmt_peak",kTRUE);
  pdsEvTree->SetBranchStatus("pmt_integral",kTRUE);
  pdsEvTree->SetBranchStatus("pmt_flag",kTRUE);
  pdsEvTree->SetBranchStatus("inBeamWindow",kTRUE);
  pdsEvTree->SetBranchStatus("isBeamTrigger",kTRUE);
  pdsEvTree->SetBranchStatus("rf_time",kTRUE);

  Double_t ymin = 0;
  Double_t ymax = 30;
  Double_t xmin = -1.7e3;
  Double_t xmax = 2.8e3;
  Int_t nbinsx = (xmax - xmin)/4;
  Int_t nbinsy = 250;
  
  TH2F* h_integral = new TH2F("h_integral",";uncal. tof;integral;",nbinsx,xmin,xmax,nbinsy,ymin,ymax*20);
  TH2F* h_peak = new TH2F("h_peak",";uncal. tof;height",nbinsx,xmin,xmax,nbinsy,ymin,ymax);
  TH2F* h_hits = new TH2F("h_hits",";uncal. tof;hits",nbinsx,xmin,xmax,nbinsy,ymin,ymax*2);
  TH2F* h_ratio = new TH2F("h_ratio",";uncal. tof;ev ratio",nbinsx,xmin,xmax,nbinsy,-0.1,1.1);
  TH1F* h_tof = new TH1F("h_tof",";uncal. tof;hits",nbinsx,xmin,xmax);
  TH2F* h_shape = new TH2F("h_shape",";sum;height",nbinsy,ymin,ymax*2,nbinsy,ymin,ymax);
  
  // Loop over events
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
      }
      // Loop over hits, if flagged
      if( Weight(pmt) ) {
	Double_t integral_sum = 0;
	Double_t peak_sum = 0;
	for( Int_t j = 0; j < pmt_hits[pmt]; j++) {
	    Double_t tof = pmt_time[pmt][j] - rf_time;
	    if( pmt_time[pmt][j] >= pmt_time[pmt][0] ) {
	      integral_sum += pmt_integral[pmt][j];
	      peak_sum += pmt_peak[pmt][j];
	    }
	    h_peak->Fill(tof,pmt_peak[pmt][j]);
	    h_tof->Fill(tof);
	    h_ratio->Fill(tof,pmt_peak[pmt][j]/pmt_peak[pmt][0]);
	}
	h_hits->Fill(pmt_time[pmt][0] - rf_time,peak_sum);
	h_integral->Fill(pmt_time[pmt][0] - rf_time,integral_sum);
	h_shape->Fill(peak_sum,pmt_peak[pmt][0]);
      }
    }
  }
  
  h_integral->Draw("colz");
  c1->SetLogz();
  c1->SaveAs("plots/tof-integral.C");
  c1->DrawClone();
  
  c1->cd();
  h_peak->Draw("colz");
  c1->SaveAs("plots/tof-height.C");
  c1->DrawClone();

  c1->cd();
  h_ratio->Draw("colz");
  c1->SaveAs("plots/tof-ratio.C");
  c1->DrawClone();

  c1->cd();
  h_tof->Draw("e");
  c1->cd()->SetLogy();
  c1->SaveAs("plots/tof-count.C");
  c1->DrawClone();

  c1->cd()->SetLogy(0);
  h_shape->Draw("colz");
  c1->SaveAs("plots/tof-shape.C");
  c1->DrawClone();

  c1->cd();
  h_hits->Draw("colz");
  c1->SaveAs("plots/tof-hits.C");
  c1->DrawClone();

  c1->Close();
}  
