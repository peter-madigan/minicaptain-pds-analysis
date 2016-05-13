Double_t calib_time = -671.25e-9; // sec 
Double_t time_err   = 2e-9; // sec -- overestimate
Double_t tof_length = 23.18; // m -- 2016-3 pmadigan
Double_t length_err = 0.01; // m 

Double_t c    = 3e8; // m/s
Double_t mass = 939.565379; // MeV/c2 -- PDG 2012
Double_t mass_err = 0.000021; // MeV/c2 -- PDG 2012

using namespace TMath;

static const Int_t kNPMTs = 15;
static const Int_t kMaxNHits = 200;
static const Double_t kIntToPE = 0.07084;

UInt_t   runno;
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

void tof_to_spectrum() {
  gROOT -> ProcessLine(".X chainFiles.C");
  gROOT -> ProcessLine(".X plots/tof-prompt_filt.C");

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

  Int_t nbinsx = 200;
  Double_t xmin = 0;
  Double_t xmax = 1e3;
  Int_t nbinsy = 200;
  Double_t ymin= 0;
  Double_t ymax= 2e2;

  TF1* tofFromEnergy = new TF1("tofFromEnergy","[0]/[1]*((x+[2])/sqrt((x+[2])^2-[2]^2)-1)*1e9",10,800);
  tofFromEnergy->FixParameter(0, tof_length);
  tofFromEnergy->FixParameter(1, c);
  tofFromEnergy->FixParameter(2, mass);
  TF1* tofToEnergy = new TF1("tofToEnergy","[2]*(sqrt(1 / (((x) * 1e-9 * [1] / [0])^2 - 1) + 1) - 1)",14.54+77.26,456.54+77.26);
  tofToEnergy->FixParameter(0, tof_length);
  tofToEnergy->FixParameter(1, c);
  tofToEnergy->FixParameter(2, mass);
  
  Double_t energy;
  Double_t energy_err;
  TH1F* h_spectrum = new TH1F("h_spectrum",";Neutron E_{kin} (MeV);Count",nbinsx,xmin,xmax);
  TH2F* h_peak = new TH2F("h_peak",";Neutron E_{kin} (MeV);PDS response (pe)",nbinsx,xmin,xmax,nbinsy,ymin,ymax);
  TH2F* h_integral = new TH2F("h_integral",";Neutron E_{kin} (MeV);Integrated charge (pe)",nbinsx,xmin,xmax,nbinsy,ymin,ymax);

  TH1F* h_spectrum_corr = new TH1F("h_spectrum_corr",";Neutron E_{kin} (MeV);Count",nbinsx,xmin,xmax);

  TObjArray* h_spectrum_shift = new TObjArray();
  h_spectrum_shift->Add(new TH1F("h_spectrum_p1",";Neutron E_{kin} (MeV);Count",nbinsx,xmin,xmax));
  h_spectrum_shift->Add(new TH1F("h_spectrum_m1",";Neutron E_{kin} (MeV);Count",nbinsx,xmin,xmax));

  // Loop over ev
  for( Int_t ientry = 0; pdsEvTree -> GetEntry(ientry); ientry++ ) {
    if( pds_flag && inBeamWindow && !isBeamTrigger && pds_nevent==1 ) {
      // Find n KE
      Double_t time = (pds_time[0] - rf_time) * 1e-9;
      Double_t p = time_to_p(time);
      Double_t E = Sqrt( Power(p,2) + Power(mass,2) ) - mass;
      h_spectrum -> Fill(E);
      //MCConvolve( h_spectrum, p, p_err(time) );
      
      // Loop over pmts
      Double_t integral_sum = 0;
      for( Int_t pmt = 0; pmt < kNPMTs; pmt++ ) {
	if( pmt_flag[pmt] && (true || pmt == 4 || pmt == 8 || pmt == 9 || pmt == 14) ) {
	  for( Int_t hit = 0; hit < pmt_hits[pmt]; hit++ ) {
	    if( pmt_time[pmt][hit] >= pds_time[0]-10 ) { 
	      integral_sum += pmt_peak[pmt][hit];
	    }
	  }
	}
      }
      h_integral -> Fill( E, integral_sum );
      //MCConvolve( h_integral, p, p_err(time), integral_sum );
      h_peak -> Fill( E, pds_peak[0] );                   
      //MCConvolve( h_peak, p, p_err(time), peak_sum );
      
      // Fill gamma removed hists
      time = (pds_time[0] - rf_time) * 1e-9;
      //time = h_tof_filt->GetRandom()*1e-9;
      //time += calib_time - tof_length/c;
      p = time_to_p(time);
      E = Sqrt( Power(p,2) + Power(mass,2) ) - mass;
      h_spectrum_corr -> Fill(E);

      time += -2e-9;
      p = time_to_p(time);
      E = Sqrt( Power(p,2) + Power(mass,2) ) - mass;
      ((TH1F*)h_spectrum_shift->At(0)) -> Fill(E);

      time += +4e-9;
      p = time_to_p(time);
      E = Sqrt( Power(p,2) + Power(mass,2) ) - mass;
      ((TH1F*)h_spectrum_shift->At(1)) -> Fill(E);
    }
  }  

  TCanvas* c1 = new TCanvas();

  gStyle->SetOptStat(0);

  //h_spectrum->GetXaxis()->SetRangeUser(0,800);
  //h_spectrum->GetYaxis()->SetRangeUser(1,200);
  h_spectrum->Draw("");

  Float_t rightmax = tofFromEnergy->Eval(10);
  Float_t rightmin = tofFromEnergy->Eval(800);
  Float_t scale = (300)/(rightmax);
  tofFromEnergy->FixParameter(0, tof_length * scale);
  tofFromEnergy->Draw("same");

  //TGaxis *axis = new TGaxis(800,0,800,tofFromEnergy->Eval(10),0,rightmax,510,"+L");
  //axis->SetTitle("TOF (ns)");
  //axis->Draw("same");

  c1 -> DrawClone();
  c1 -> cd();

  //((TH1F*)h_spectrum_shift->At(0))->GetXaxis()->SetRangeUser(0,800);
  //((TH1F*)h_spectrum_shift->At(0))->GetYaxis()->SetRangeUser(1,200);
  
  ((TH1F*)h_spectrum_shift->At(0))->SetLineColor(kRed);
  ((TH1F*)h_spectrum_shift->At(1))->SetLineColor(kRed);
  (TH1F*)h_spectrum_shift->At(0)->Draw("");
  (TH1F*)h_spectrum_shift->At(1)->Draw("same");

  //h_spectrum_corr->GetXaxis()->SetRangeUser(0,800);
  //h_spectrum_corr->GetYaxis()->SetRangeUser(1,200);
  h_spectrum_corr->Draw("same");

  c1 -> DrawClone();
  c1 -> cd();

  h_peak -> Draw("colz");
  
  c1 -> DrawClone();
  c1 -> cd();

  h_integral -> Draw("colz");
}

Double_t time_to_p(Double_t time) { // expects units of sec
  // check for "bad" events
  if( time - calib_time < 0 ) return -1;
    
  // get calibrated time
  time = time - calib_time + tof_length/c;
  
  // calculate momentum (MeV/c)
  Double_t p = tof_length * (mass) / Sqrt( Power( time * c, 2) - Power( tof_length, 2) );
  
  return p;
}


Double_t p_err(Double_t time) {
  // check for "bad" events    
  if( time - calib_time < 0 ) return -1;
  Double_t p = time_to_p(time);

  // get calibrated time             
  time = time - calib_time + tof_length/c;

  // calculate momentum (MeV/c)      
  Double_t part_length = Power( length_err * ( mass/Sqrt( Power(time * c, 2) - Power(tof_length,2)) + mass * tof_length / Power(Power(time * c, 2) - Power(tof_length,2),1.5) ), 2);
  Double_t part_time = time_err * tof_length * mass * time * Power( c, 2);
  part_time = part_time / Power( Power(time*c,2) - Power(tof_length,2), 1.5);
  part_time = Power( part_time, 2);
  Double_t part_mass = Power( mass_err * tof_length / Sqrt( Power(time * c,2) - Power(tof_length,2)), 2);
  Double_t p_err = Sqrt( part_length + part_time + part_mass);
  /*  cout << "Time0:" << time << "\n"
       << "Momentum:" << p << "\n"
       << "Length:" << Sqrt(part_length) << "\n"
       << "Time:" << Sqrt(part_time) << "\n"
       << "Mass:" << Sqrt(part_mass)
       << endl; */
  return p_err;
}

void MCConvolve( TH1* h, Double_t mean, Double_t sigma ) {
  TF1* myFunc = new TF1("myFunc","gaus",0,2e3);
  myFunc -> SetParameters(1, mean, sigma);

  for( int i = 0; i < 100; i++ ) {
    h -> Fill( myFunc -> GetRandom(), .01);
  }
  /* cout << myFunc -> GetParameter(0) << "\t"
       << myFunc -> GetParameter(1) << "\t"
       << myFunc -> GetParameter(2) << "\t"
       << endl;*/

  myFunc -> Delete();
}

void MCConvolve( TH2* h, Double_t mean, Double_t sigma, Double_t y ) {
  TF1* myFunc = new TF1("myFunc","gaus",0,2e3);
  myFunc -> SetParameters(1, mean, sigma);

  for( int i = 0; i < 100; i++ ) {
    h -> Fill( myFunc -> GetRandom(), y, .01);
  }
  /* cout << myFunc -> GetParameter(0) << "\t" 
     << myFunc -> GetParameter(1) << "\t"                                                    
     << myFunc -> GetParameter(2) << "\t"                                                    
     << endl;*/

  myFunc -> Delete();
}
