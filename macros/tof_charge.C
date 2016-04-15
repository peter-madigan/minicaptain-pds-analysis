static const Int_t kNPMTs = 15;
static const Int_t kMaxNHits = 200;
static const Double_t kDelay = -671.25 - 77.267;

Int_t    pds_hits;
Double_t pds_ratio;
Double_t pds_time[kMaxNHits];
Double_t pds_peak[kMaxNHits];
Double_t pds_integral[kMaxNHits];
Bool_t   pds_flag;
Double_t pds_FWHM[kMaxNHits];
Bool_t   inBeamWindow;
Bool_t   isBeamTrigger;
Double_t rf_time;

Double_t Weight(Int_t pmt=-1, Int_t hit=0) {
  if( !pds_flag ) return 0;
  if( !inBeamWindow ) return 0;
  //if( isBeamTrigger ) return 0;
  //if( pds_peak[hit] > 5 ) return 0;

  return 1.;
}

void tof_charge() {
  gROOT -> ProcessLine( ".X chainFiles.C" );
  cout << "#Ev: " << pdsEvTree -> GetEntries() << endl;

  pdsEvTree->SetBranchStatus("*", kFALSE);

  pdsEvTree->SetBranchAddress("pds_hits",&pds_hits);
  pdsEvTree->SetBranchAddress("pds_ratio",&pds_ratio);
  pdsEvTree->SetBranchAddress("pds_time",pds_time);
  pdsEvTree->SetBranchAddress("pds_peak",pds_peak);
  pdsEvTree->SetBranchAddress("pds_integral",pds_integral);
  pdsEvTree->SetBranchAddress("pds_flag",&pds_flag);
  pdsEvTree->SetBranchAddress("pds_FWHM",pds_FWHM);
  pdsEvTree->SetBranchAddress("inBeamWindow",&inBeamWindow);
  pdsEvTree->SetBranchAddress("isBeamTrigger",&isBeamTrigger);
  pdsEvTree->SetBranchAddress("rf_time",&rf_time);

  pdsEvTree->SetBranchStatus("pds_hits",kTRUE);
  pdsEvTree->SetBranchStatus("pds_ratio",kTRUE);
  pdsEvTree->SetBranchStatus("pds_time",kTRUE);
  pdsEvTree->SetBranchStatus("pds_peak",kTRUE);
  pdsEvTree->SetBranchStatus("pds_integral",kTRUE);
  pdsEvTree->SetBranchStatus("pds_flag",kTRUE);
  pdsEvTree->SetBranchStatus("pds_FWHM",kTRUE);
  pdsEvTree->SetBranchStatus("inBeamWindow",kTRUE);
  pdsEvTree->SetBranchStatus("isBeamTrigger",kTRUE);
  pdsEvTree->SetBranchStatus("rf_time",kTRUE);

  // Create plots
  Double_t ymin = 0;
  Double_t ymax = 100;
  Double_t xmin = -1.7e3;
  Double_t xmax = 2.8e3;
  Int_t nbinsx = (xmax - xmin);
  Int_t nbinsy = 250;
  
  TH2F* h_integral = new TH2F("h_integral",";tof (ns);integral;",nbinsx,xmin,xmax,nbinsy,ymin,ymax*20);
  TH2F* h_peak = new TH2F("h_peak",";tof (ns);height",nbinsx,xmin,xmax,nbinsy,ymin,ymax);
  TH2F* h_hits = new TH2F("h_hits",";tof (ns);hits",nbinsx,xmin,xmax,nbinsy,ymin,ymax*2);
  TH2F* h_FWHM = new TH2F("h_FWHM",";tof (ns);FWHM",nbinsx,xmin,xmax,nbinsy,ymin,ymax);
  TH1F* h_tof = new TH1F("h_tof",";tof (ns);hits",nbinsx,xmin,xmax);
  TH1F* h_tof_prompt = new TH1F("h_tof_prompt",";tof (ns);hits",nbinsx,xmin,xmax);
  TH1F* h_tof_filtered = new TH1F("h_tof_prompt",";tof (ns);hits",nbinsx,xmin,xmax);
  TH2F* h_shape = new TH2F("h_shape",";sum;height",nbinsy,ymin,ymax*2,nbinsy,ymin,ymax);

  TH2F* h_ratio = new TH2F("h_ratio",";tof (ns);integral_{prompt}/integral_{ev}",90,60,150,nbinsy,0,1);
  
  // For removing gammas from spectrum
  TF1* fGaus = new TF1("fGaus","gaus",xmin,xmax);
  double height = 67.623; 
  double mean   = 77.267;
  double sigma  = 4.9;
  fGaus -> SetParameters(height, mean, sigma);
  TH1F* h_gamma_tof = new TH1F("h_gamma_tof","",nbinsx,xmin,xmax);
  for( int i = 1; i < h_gamma_tof->GetNbinsX()+1; i++ )
    h_gamma_tof->SetBinContent(i, fGaus->Eval(h_gamma_tof->GetBinCenter(i)));

  // Loop over events
  pdsEvTree->GetEntry(0);
  for( Int_t i = 0; pdsEvTree->GetEntry(i); i++ ) {
    // Status update
    if( i%(pdsEvTree->GetEntriesFast()/10) == 0 ) {
      cout << i << "of" << pdsEvTree->GetEntries() << endl;
      cout << "peak " << pds_peak[0] << endl;
      cout << "integral " << pds_integral[0] << endl;
      cout << "rf " << rf_time << "\ttime " << pds_time[0] << endl;
    }
    // Loop over hits, if flagged
    if( Weight() ) {
      Double_t integral_sum = 0;
      Double_t peak_sum = 0;
      for( Int_t j = 0; j < pds_hits; j++) {
	Double_t tof = pds_time[j] - rf_time -kDelay;
	if( pds_time[j] >= pds_time[0] ) {
	  integral_sum += pds_integral[j];
	  peak_sum += pds_peak[j];
	}
	h_peak->Fill(tof,pds_peak[j]);
	h_tof->Fill(tof);
	h_FWHM->Fill(tof,pds_FWHM[j]);
      }
      h_tof_prompt->Fill(pds_time[0] - rf_time - kDelay);
      h_hits->Fill(pds_time[0] - rf_time - kDelay, peak_sum);
      h_integral->Fill(pds_time[0] - rf_time - kDelay,integral_sum);
      h_shape->Fill(integral_sum,pds_peak[0]);
      h_ratio->Fill(pds_time[0] - rf_time - kDelay,pds_integral[0]/integral_sum);
    }
  }
  h_tof_filtered->Add(h_tof_prompt,h_gamma_tof,1,-1);
  for( int i = 1; i < h_tof_filtered->GetNbinsX()+1; i++ )
    if( h_tof_filtered->GetBinContent(i) < 0 )
      h_tof_filtered->SetBinContent(i, 0);
  
  h_integral->Draw("colz");
  c1->SetLogz();
  c1->SaveAs("plots/tof-integral.C");
  c1->DrawClone();
  
  c1->cd();
  h_peak->Draw("colz");
  c1->SaveAs("plots/tof-height.C");
  c1->DrawClone();

  c1->cd();
  h_FWHM->Draw("colz");
  c1->SaveAs("plots/tof-FWHM.C");
  c1->DrawClone();

  c1->cd();
  h_tof->Draw();
  c1->cd()->SetLogy();
  c1->SaveAs("plots/tof-count.C");
  c1->DrawClone();

  c1->cd();
  h_tof_prompt->Draw();
  fGaus->Draw("same");
  c1->cd()->SetLogy();
  c1->SaveAs("plots/tof-count-prompt.C");
  c1->DrawClone();

  c1->cd();
  h_tof_filtered->Draw();
  c1->cd()->SetLogy();
  c1->SaveAs("plots/tof-count-filt.C");
  c1->DrawClone();

  c1->cd()->SetLogy(0);
  h_shape->Draw("colz");
  c1->SaveAs("plots/tof-shape.C");
  c1->DrawClone();

  c1->cd();
  h_ratio->Draw("colz");
  c1->SaveAs("plots/tof-ratio.C");
  c1->DrawClone();

  c1->cd();
  h_hits->Draw("colz");
  c1->SaveAs("plots/tof-hits.C");
  c1->DrawClone();

  c1->Close();
}  
