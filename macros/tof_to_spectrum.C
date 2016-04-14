Double_t calib_time = -671e-9; // sec 
Double_t time_err   = 2e-9; // sec -- overestimate
Double_t tof_length = 23.18; // m -- 2016-3 pmadigan
Double_t length_err = 0.01; // m 

Double_t c    = 3e8; // m/s
Double_t mass = 939.565379; // MeV/c2 -- PDG 2012
Double_t mass_err = 0.000021; // MeV/c2 -- PDG 2012

using namespace TMath;

void tof_to_spectrum() {
  gROOT -> ProcessLine(".X chainFiles.C");

  UInt_t runno;
  Int_t pmt_nevent[15];
  Int_t pmt_hits[15];
  Double_t pmt_peak[15][200];
  Double_t pmt_integral[15][200];
  Double_t pmt_time[15][200];
  Double_t rf_time;
  Bool_t  pmt_flag[15];
  Bool_t  inBeamWindow;
  Bool_t  isBeamTrigger;

  pdsEvTree -> SetBranchStatus("*", kFALSE);
    
  pdsEvTree -> SetBranchAddress("runno", &runno);
  pdsEvTree -> SetBranchAddress("pmt_hits",    pmt_hits);
  pdsEvTree -> SetBranchAddress("pmt_peak",    pmt_peak);
  pdsEvTree -> SetBranchAddress("pmt_integral",pmt_integral);
  pdsEvTree -> SetBranchAddress("pmt_time",    pmt_time);
  pdsEvTree -> SetBranchAddress("rf_time",    &rf_time);
  pdsEvTree -> SetBranchAddress("pmt_flag",    pmt_flag);
  pdsEvTree -> SetBranchAddress("inBeamWindow",&inBeamWindow);
  pdsEvTree -> SetBranchAddress("isBeamTrigger",&isBeamTrigger);

  pdsEvTree -> SetBranchStatus("runno", kTRUE);
  pdsEvTree -> SetBranchStatus("pmt_hits",kTRUE);
  pdsEvTree -> SetBranchStatus("pmt_peak",kTRUE);
  pdsEvTree -> SetBranchStatus("pmt_integral",kTRUE);
  pdsEvTree -> SetBranchStatus("pmt_time",kTRUE);
  pdsEvTree -> SetBranchStatus("rf_time",kTRUE);
  pdsEvTree -> SetBranchStatus("pmt_flag",kTRUE);
  pdsEvTree -> SetBranchStatus("inBeamWindow",kTRUE);
  pdsEvTree -> SetBranchStatus("isBeamTrigger",kTRUE);

  Int_t nbinsx = 1000;
  Double_t xmin = 0;
  Double_t xmax = 2.5e3;
  Int_t nbinsy = 200;
  Double_t ymin= 0;
  Double_t ymax= 1e5;

  Double_t energy;
  Double_t energy_err;
  TH1F* h_spectrum = new TH1F("h_spectrum",";Neutron p (MeV/c);Count",nbinsx,xmin,xmax);
  h_spectrum -> Sumw2();
  TH2F* h_peak = new TH2F("h_peak",";Neutron p (MeV/c);PDS response (pe)",nbinsx,xmin,xmax,nbinsy,ymin,100);
  TH2F* h_integral = new TH2F("h_integral",";Neutron p (MeV/c);Integrated charge (pe ns)",nbinsx,xmin,xmax,nbinsy,ymin,1e3);
  for( Int_t ientry = 0; pdsEvTree -> GetEntry(ientry); ientry++ ) {
    for( Int_t pmt = 0; pmt < 15; pmt++ ) {
      if( pmt_flag[pmt] && inBeamWindow && !isBeamTrigger ) {
	Double_t time = (pmt_time[pmt][0] - rf_time) * 1e-9;
	Double_t p = time_to_p(time);
	Double_t E = Sqrt( Power(p,2) + Power(mass,2) );
	h_spectrum -> Fill(p);
	//MCConvolve( h_spectrum, p, p_err(time) );
	Double_t peak_sum = 0;
	Double_t integral_sum = 0;
	for( Int_t hit = 0; hit < pmt_hits[pmt]; hit++ ) {
	  if( pmt_time[pmt][hit] >= pmt_time[pmt][0] ) { 
	    peak_sum += pmt_peak[pmt][hit];
	    integral_sum += pmt_integral[pmt][hit];
	  }
	}
	h_peak -> Fill( p, peak_sum );                                                 
	//MCConvolve( h_peak, p, p_err(time), peak_sum );
	h_integral -> Fill( p, integral_sum );                                           
	//MCConvolve( h_integral, p, p_err(time), integral_sum );
      }
    }
  }
  
  h_spectrum -> Draw("e");

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
