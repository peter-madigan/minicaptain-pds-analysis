Double_t calib_time = -900.5e-9; // 2016-4-4 pmadigan
Double_t time_err   = 4e-9; // sec -- overestimate
Double_t tof_length = 23.18; // m -- 2016-3 pmadigan
Double_t length_err = 0.01; // m 

Double_t c    = 3e8; // m/s
Double_t mass = 939.565379; // MeV/c2 -- PDG 2012
Double_t mass_err = 0.000021; // MeV/c2 -- PDG 2012

using namespace TMath;

void tof_to_spectrum() {
  gROOT -> ProcessLine(".X chainFiles.C");

  Int_t pds_nevent;
  Double_t pds_peak[1000];
  Double_t pds_integral[1000];
  Double_t pds_time[1000];
  Double_t rf_time[1000];
  Bool_t  pds_flag[1000];
  Bool_t  inBeamWindow[1000];
  Bool_t  isBeamTrigger[1000];
    
  pdsEvTree -> SetBranchAddress("pds_nevent", &pds_nevent);
  pdsEvTree -> SetBranchAddress("pds_peak",    pds_peak);
  pdsEvTree -> SetBranchAddress("pds_integral",pds_integral);
  pdsEvTree -> SetBranchAddress("pds_time",    pds_time);
  pdsEvTree -> SetBranchAddress("rf_time",     rf_time);
  pdsEvTree -> SetBranchAddress("pds_flag",   pds_flag);
  pdsEvTree -> SetBranchAddress("inBeamWindow",inBeamWindow);
  pdsEvTree -> SetBranchAddress("isBeamTrigger",isBeamTrigger);

  Int_t nbinsx = 1000;
  Double_t xmin = 0;
  Double_t xmax = 2.5e3;
  Int_t nbinsy = 200;
  Double_t ymin= 0;
  Double_t ymax= 100;

  Double_t energy;
  Double_t energy_err;
  TH1F* h_spectrum = new TH1F("h_spectrum",";Neutron p (MeV/c);Count",nbinsx,xmin,xmax);
  h_spectrum -> Sumw2();
  TH2F* h_peak = new TH2F("h_peak",";Neutron p (MeV/c);PDS response (pe)",nbinsx,xmin,xmax,nbinsy,ymin,ymax);
  TH2F* h_integral = new TH2F("h_integral",";Neutron p (MeV/c);Integrated charge (pe ns)",nbinsx,xmin,xmax,nbinsy,ymin,1e3);
  for( Int_t ientry = 0; pdsEvTree -> GetEntry(ientry); ientry++ ) {
    for( Int_t jentry = 0; jentry < pds_nevent; jentry++ ) {
      if( pds_flag[jentry] && inBeamWindow[jentry] && !(pds_integral[jentry] < 75)) {
	Double_t time = (pds_time[jentry] - rf_time[jentry]) * 1e-9;
	Double_t p = time_to_p(time);
	Double_t E = Sqrt( Power(p,2) + Power(mass,2) );
	h_spectrum -> Fill(p);
	//MCConvolve( h_spectrum, p, p_err(time) );
	h_peak -> Fill( p, pds_peak[jentry] );
	//MCConvolve( h_peak, p, p_err(time), pds_peak[jentry] );
	h_integral -> Fill( p, pds_integral[jentry] );
	//MCConvolve( h_integral, p, p_err(time), pds_integral[jentry] );
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
