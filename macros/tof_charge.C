static const Int_t kNPMTs = 15;
static const Int_t kMaxNHits = 200;
static const Double_t kDelay = 0;//-671.25 - 77.267;
static const Double_t kIntToPE = 0.07084;

Int_t    pds_nevent;
Int_t    pds_hits;
Int_t    pmt_hits[kNPMTs];
Double_t pds_time[kMaxNHits];
Double_t pmt_time[kNPMTs][kMaxNHits];
Double_t pds_peak[kMaxNHits];
Double_t pmt_peak[kNPMTs][kMaxNHits];
Double_t pds_integral[kMaxNHits];
Double_t pmt_integral[kNPMTs][kMaxNHits];
Bool_t   pds_flag;
Bool_t   pmt_flag[kNPMTs];
Double_t pds_FWHM[kMaxNHits];
Double_t pmt_FWHM[kNPMTs][kMaxNHits];
Bool_t   inBeamWindow;
Bool_t   isBeamTrigger;
Double_t rf_time;

void tof_charge() {
  gROOT -> ProcessLine( ".X chainFiles.C" );
  cout << "#Ev: " << pdsEvTree -> GetEntries() << endl;

  pdsEvTree->SetBranchStatus("*", kFALSE);

  pdsEvTree->SetBranchAddress("pds_nevent",&pds_nevent);
  pdsEvTree->SetBranchAddress("pds_hits",&pds_hits);
  pdsEvTree->SetBranchAddress("pmt_hits",pmt_hits);
  pdsEvTree->SetBranchAddress("pds_time",pds_time);
  pdsEvTree->SetBranchAddress("pmt_time",pmt_time);
  pdsEvTree->SetBranchAddress("pds_peak",pds_peak);
  pdsEvTree->SetBranchAddress("pmt_peak",pmt_peak);
  pdsEvTree->SetBranchAddress("pds_integral",pds_integral);
  pdsEvTree->SetBranchAddress("pmt_integral",pmt_integral);
  pdsEvTree->SetBranchAddress("pds_flag",&pds_flag);
  pdsEvTree->SetBranchAddress("pmt_flag",pmt_flag);
  pdsEvTree->SetBranchAddress("pds_FWHM",pds_FWHM);
  pdsEvTree->SetBranchAddress("pmt_FWHM",pmt_FWHM);
  pdsEvTree->SetBranchAddress("inBeamWindow",&inBeamWindow);
  pdsEvTree->SetBranchAddress("isBeamTrigger",&isBeamTrigger);
  pdsEvTree->SetBranchAddress("rf_time",&rf_time);

  pdsEvTree->SetBranchStatus("pds_nevent",kTRUE);
  pdsEvTree->SetBranchStatus("pds_hits",kTRUE);
  pdsEvTree->SetBranchStatus("pmt_hits",kTRUE);
  pdsEvTree->SetBranchStatus("pds_time",kTRUE);
  pdsEvTree->SetBranchStatus("pmt_time",kTRUE);
  pdsEvTree->SetBranchStatus("pds_peak",kTRUE);
  pdsEvTree->SetBranchStatus("pmt_peak",kTRUE);
  pdsEvTree->SetBranchStatus("pds_integral",kTRUE);
  pdsEvTree->SetBranchStatus("pmt_integral",kTRUE);
  pdsEvTree->SetBranchStatus("pds_flag",kTRUE);
  pdsEvTree->SetBranchStatus("pmt_flag",kTRUE);
  pdsEvTree->SetBranchStatus("pds_FWHM",kTRUE);
  pdsEvTree->SetBranchStatus("pmt_FWHM",kTRUE);
  pdsEvTree->SetBranchStatus("inBeamWindow",kTRUE);
  pdsEvTree->SetBranchStatus("isBeamTrigger",kTRUE);
  pdsEvTree->SetBranchStatus("rf_time",kTRUE);

  // Create plots
  Double_t ymin = 0;
  Double_t ymax = 1000;
  Double_t xmin = -1.7e3;
  Double_t xmax = 2.8e3;
  Double_t hitmin = 0;
  Double_t hitmax = 3.2e3;
  Int_t nbinshit = (hitmax - hitmin);
  Int_t nbinsx = (xmax - xmin)*4;
  Int_t nbinsy = (ymax-ymin)/4;
  
  TH2F* h_integral = new TH2F("h_integral",";tof (ns);event integral (pe);",nbinsx,xmin,xmax,nbinsy,ymin,ymax);
  TH2F* h_integral_hit = new TH2F("h_integral_hit",";dt (ns);hit integral (pe);",nbinshit,hitmin,hitmax,nbinsy,ymin,ymax);
  TH2F* h_peak = new TH2F("h_peak",";tof (ns);event height (pe);",nbinsx,xmin,xmax,nbinsy,ymin,ymax);
  TH2F* h_peak_hit = new TH2F("h_peak_hit",";dt (ns);hit height (pe)",nbinshit,hitmin,hitmax,nbinsy,ymin,ymax);
  TH2F* h_FWHM_hit = new TH2F("h_FWHM_hit",";dt (ns);hit FWHM (ns)",nbinshit,hitmin,hitmax,nbinsy,ymin,ymax/2);
  TH1F* h_tof_hit = new TH1F("h_tof",";dt (ns);hits",nbinshit,hitmin,hitmax);
  
  TH1F* h_tof_prompt = new TH1F("h_tof_prompt",";tof (ns);events",nbinsx,xmin,xmax);
  TH1F* h_tof_filt = new TH1F("h_tof_filt",";tof (ns);events",nbinsx,xmin,xmax);
  
  TH2F* h_shape = new TH2F("h_shape",";singlet integral (pe);triplet integral (pe)",nbinsy,ymin,ymax,nbinsy,ymin,ymax);
  TH2F* h_ratio = new TH2F("h_ratio",";tof (ns);log( triplet / singlet )",nbinsx,xmin,xmax,nbinsy,-8,8);
  
  TH2F* h_integral_calib = new TH2F("h_integral_calib",";height (pe);integral (pe * ns)",100*nbinsy,ymin,ymax,100*nbinsy,ymin,ymax*20);

  // For removing gammas from spectrum
  TF1* fGaus = new TF1("fGaus","gaus",xmin,xmax);
  double height = 16.28; 
  double mean   = 76.9;
  double sigma  = 3.9;
  fGaus -> SetParameters(height, mean, sigma);
  TH1F* h_gamma_tof = new TH1F("h_gamma_tof","",nbinsx,xmin,xmax);
  for( int i = 1; i < h_gamma_tof->GetNbinsX()+1; i++ )
    h_gamma_tof->SetBinContent(i, fGaus->Eval(h_gamma_tof->GetBinCenter(i)));

  // Loop over events
  pdsEvTree->GetEntry(0);
  for( Int_t i = 0; pdsEvTree->GetEntry(i); i++ ) {
    // Status update
    if( i%(pdsEvTree->GetEntriesFast()/10) == 0 )
      cout << i << " of " << pdsEvTree->GetEntries() << endl;
    // Cut events
    if( inBeamWindow && pds_flag && pds_nevent == 1 ) {
      // Loop over PDS
      Double_t TOF = pds_time[0] - rf_time - kDelay;
      //for( Int_t pmt = 0; pmt < kNPMTs; pmt++ )
      //if( pmt_time[pmt][0] - rf_time - kDelay < TOF && pmt_flag[pmt] )
      //TOF = pmt_time[pmt][0] - rf_time - kDelay;
      Double_t event_singlet = 0;
      Double_t event_triplet = 0;
      Double_t triplet = 0;
      Double_t ev_ratio = 0;
      Double_t TOF_hit = 0;
      
      for( Int_t pmt = 0; pmt < kNPMTs; pmt++ ) {
	if( pmt_flag[pmt] ) {
	  triplet = 0;
	  
	  for( Int_t j = 0; j < pmt_hits[pmt]; j++) {
	    TOF_hit = pmt_time[pmt][j] - pds_time[0];
	    if( pmt_time[pmt][j] > pds_time[0] )
	      triplet += pmt_peak[pmt][j];

	    h_integral_hit->Fill(TOF_hit,pmt_integral[pmt][j]);
	    h_peak_hit->Fill(TOF_hit,pmt_peak[pmt][j]);
	    h_FWHM_hit->Fill(TOF_hit,pmt_FWHM[pmt][j]);
	    
	    if( pmt_time[pmt][j] > pds_time[0] &&
		(pmt_peak[pmt][0] > 5 &&
		 TOF < 103.28+77.267 && TOF > 14.54+77.267) ) // approx 100MeV - 800MeV
	      h_tof_hit->Fill(TOF_hit);
	    
	    h_integral_calib->Fill(pmt_peak[pmt][j],pmt_integral[pmt][j]);
	  }
	  event_singlet += pmt_peak[pmt][0];
	  event_triplet += triplet;
	}
      }
      h_shape->Fill(event_singlet,event_triplet);
      h_ratio->Fill(TOF,TMath::Log(event_triplet/event_singlet));

      h_tof_prompt->Fill(TOF);
      h_integral->Fill(TOF,event_singlet+event_triplet);
      h_peak->Fill(TOF,pds_peak[0]);
    }
  }

  // Remove gamma peak
  h_tof_filt->Add(h_tof_prompt,h_gamma_tof,1,-1);
  for( int i = 1; i < h_tof_filt->GetNbinsX()+1; i++ )
    if( h_tof_filt->GetBinContent(i) < 0 )
      h_tof_filt->SetBinContent(i, 0);
  
  // Save plots
  h_integral->SaveAs("plots/tof-integral.C");
  h_integral_hit->SaveAs("plots/tof-integral_hit.C");
  h_peak->SaveAs("plots/tof-height.C");
  h_peak_hit->SaveAs("plots/tof-height_hit.C");
  h_FWHM_hit->SaveAs("plots/tof-FWHM_hit.C");
  h_tof_hit->SaveAs("plots/tof-hit.C");

  h_tof_prompt->SaveAs("plots/tof-prompt.C");
  h_tof_filt->SaveAs("plots/tof-prompt_filt.C");

  h_shape->SaveAs("plots/tof-shape.C");
  h_ratio->SaveAs("plots/tof-ratio.C");

  // Draw plot(s)
  h_integral_calib->ProfileX()->Fit("pol1");
  c1->SaveAs("plots/tof-integral_calib.C");
}  
