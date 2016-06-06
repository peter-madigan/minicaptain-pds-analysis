#include <vector>

Double_t calib_time = -671.25e-9; // sec 
Double_t time_err  = 2e-9; // sec -- likely an overestimate
Double_t peak_err  = 2.5/6.5; // average width (ADC) over average 1pe peak (ADC)
Double_t tof_length = 23.18; // m -- 2016-3 pmadigan
Double_t length_err = 0.01; // m 

Double_t c    = 3e8; // m/s
Double_t mass = 939.565379; // MeV/c2 -- PDG 2012
Double_t mass_err = 0.000021; // MeV/c2 -- PDG 2012
Double_t background_rate = 800; // Hz
Double_t gamma_width = 4.9e-9; // s

using namespace TMath;
using namespace std;

static const Int_t kNPMTs = 15;
static const Int_t kMaxNHits = 200;
static const Double_t kIntToPE = 1./5.912;

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

  vector<double> energybins;
  double width = 3e-9;
  for( double t = 1000e-9; t > tof_length/c; t-=width ) {
    if( time_to_E(t) > 1200 ) continue; 
    energybins.push_back(time_to_E(t));
    if( time_to_E(t) < 10 )
      t-=100*width-width;
    if( time_to_E(t) < 100 )
      t-=10*width-width;
  }
  std::sort(energybins.begin(),energybins.end());
  vector<double> lightbins;
  for( int i = 0; i < 200; i++ )
    lightbins.push_back(i*2);

  vector<double> ratiobins;
  for( int i = 0; i < 200; i++ )
    ratiobins.push_back(i*(10./200));
  
  TH1F* h_spectrum = new TH1F("h_spectrum",";Neutron E_{kin} (MeV);Frac. (MeV^{-1})",
			      energybins.size()-1,&energybins[0]);
  h_spectrum->Sumw2();
  TH1F* h_prompt = new TH1F("h_prompt",";Neutron E_{kin} (MeV);Pbrompt light yield (pe / n MeV)",
			    energybins.size()-1,&energybins[0]);
  h_prompt->Sumw2();
  TH1F* h_total = new TH1F("h_total",";Neutron E_{kin} (MeV);Total light yield (pe / n MeV)",
			   energybins.size()-1,&energybins[0]);
  h_total->Sumw2();
  TH1F* h_ratio = new TH1F("h_ratio",";Neutron E_{kin} (MeV);Triplet/singlet (MeV^{-1})",
			   energybins.size()-1,&energybins[0]);
  h_ratio->Sumw2();
  
  TH1F* h_spectrum_corr = new TH1F("h_spectrum_corr",";Neutron E_{kin} (MeV);Frac. (MeV^{-1})",
				   energybins.size()-1,&energybins[0]);
  h_spectrum_corr->Sumw2();
  TObjArray* h_spectrum_shift = new TObjArray();
  h_spectrum_shift->Add(new TH1F("h_spectrum_p1",";Neutron E_{kin} (MeV);Frac. (MeV^{-1})",
				 energybins.size()-1,&energybins[0]));
  h_spectrum_shift->Add(new TH1F("h_spectrum_m1",";Neutron E_{kin} (MeV);Frac. (MeV^{-1})",
				 energybins.size()-1,&energybins[0]));
  //((TH1F*)h_spectrum_shift->At(0))->Sumw2();
  //((TH1F*)h_spectrum_shift->At(1))->Sumw2();

  TH1F* h_background = new TH1F("h_background",";Neutron E_{kin} (MeV);Frac. (MeV^{-1})",
				energybins.size()-1,&energybins[0]);
  h_background->Sumw2();

  // Loop over ev
  Int_t n = 0;
  for( Int_t ientry = 0; pdsEvTree -> GetEntry(ientry); ientry++ ) {
    if( ientry%(pdsEvTree->GetEntriesFast()/10) == 0 )
      std::cout << ientry << "/" << pdsEvTree->GetEntries() << std::endl;
    
    if( pds_flag && inBeamWindow && !isBeamTrigger && pds_nevent==1 ) {
      // Find n KE
      Double_t time = (pds_time[0] - rf_time) * 1e-9 - calib_time + tof_length/c;;
      Double_t E = time_to_E(time);
      //if( E > xmax ) continue;
      h_spectrum -> Fill(E);
      //Convolve( h_spectrum, E, E_err(time) );

      // Fill background
      n++;
      Background(h_background);

      // Loop over pmts
      Double_t prompt_sum = 0;
      Double_t prompt_err = 0;
      Double_t sum = 0;
      Double_t sum_err = 0;
      for( Int_t pmt = 0; pmt < kNPMTs; pmt++ ) {
	if( pmt_flag[pmt] )
	  for( Int_t hit = 0; hit < pmt_hits[pmt]; hit++ )
	    if( pmt_time[pmt][hit] >= pds_time[0]-20 ) { 
	      sum += pmt_peak[pmt][hit];  
	      //sum += pmt_integral[pmt][hit] * kIntToPE;
	      sum_err += Power(peak_err,2);
	      if( pmt_time[pmt][hit] <= pds_time[0]+20 ) {
		prompt_sum += pmt_peak[pmt][hit];
		//prompt_sum += pmt_integral[pmt][hit] * kIntToPE;
		prompt_err += Power(peak_err,2);
	      }	      
	    }
      }
      sum_err    = Sqrt(sum_err);
      prompt_err = Sqrt(prompt_err);
    
      h_total->Fill(E,sum);
      h_prompt->Fill(E,prompt_sum);
      if( prompt_sum > 0 )
	h_ratio->Fill(E,(sum-prompt_sum)/prompt_sum);

      // Fill shifted histograms
      time = (pds_time[0] - rf_time) * 1e-9 - calib_time + tof_length/c;;
      //time = h_tof_filt->GetRandom()*1e-9;
      E = time_to_E(time);
      h_spectrum_corr -> Fill(E);
      
      time -= gamma_width;
      E = time_to_E(time);
      ((TH1F*)h_spectrum_shift->At(0))->Fill(E);
      
      time += 2 * gamma_width;
      E = time_to_E(time);
      ((TH1F*)h_spectrum_shift->At(1))->Fill(E);
    }
  }  
  
  TCanvas* c1 = new TCanvas();
  gStyle->SetOptStat(0);

  // Basic spectrum
  for( int i = 1; i < h_spectrum->GetNbinsX()+1; i++ ) {
    h_spectrum->SetBinContent(i,h_spectrum->GetBinContent(i)/h_spectrum->GetBinWidth(i));
    h_spectrum->SetBinError(i,h_spectrum->GetBinError(i)/h_spectrum->GetBinWidth(i));
    h_background->SetBinContent(i,h_background->GetBinContent(i)/h_background->GetBinWidth(i));
    h_background->SetBinError(i,h_background->GetBinError(i)/h_background->GetBinWidth(i));
  }
  h_spectrum->SetLineColor(kBlack);
  h_spectrum->GetYaxis()->SetRangeUser(1e-5,1);
  c1->SetLogy(); 
  h_spectrum->Scale(1./n);
  h_spectrum->Draw("e1");
  h_background->Scale(1./n);
  h_background->Draw("e1 same");

  // Spectrum with systematic
  c1 -> DrawClone();
  c1 -> cd();
  
  for( int i = 1; i < h_spectrum_corr->GetNbinsX()+1; i++ ) {
    h_spectrum_corr->SetBinContent(i,h_spectrum_corr->GetBinContent(i)/h_spectrum_corr->GetBinWidth(i));
    h_spectrum_corr->SetBinError(i,h_spectrum_corr->GetBinError(i)/h_spectrum_corr->GetBinWidth(i));
    ((TH1F*)h_spectrum_shift->At(0))->SetBinContent(i,((TH1F*)h_spectrum_shift->At(0))->GetBinContent(i)/((TH1F*)h_spectrum_shift->At(0))->GetBinWidth(i));
    ((TH1F*)h_spectrum_shift->At(1))->SetBinContent(i,((TH1F*)h_spectrum_shift->At(1))->GetBinContent(i)/((TH1F*)h_spectrum_shift->At(1))->GetBinWidth(i));
  }
  TH1F* h_spectrum_sub = (TH1F*)h_spectrum_corr->Clone("h_spectrum_sub");

  for( int i = 1; i < h_spectrum_sub->GetNbinsX()+1; i++ ) {
    Double_t v0 = h_spectrum_sub->GetBinContent(i);
    Double_t v1 =((TH1F*)h_spectrum_shift->At(0))->GetBinContent(i);
    Double_t v2 =((TH1F*)h_spectrum_shift->At(1))->GetBinContent(i);
    Double_t err = Sqrt(Power(v1-v0,2)/2 + Power(v2-v0,2)/2 + Power(h_spectrum_sub->GetBinError(i),2));
    //std::cout << v0 << ' ' << v1 << ' ' << v2 << ' ' << err << std::endl;
    h_spectrum_sub->SetBinError(i,err);
  }
  h_spectrum_corr->SetLineColor(kBlack);
  h_spectrum_corr->GetYaxis()->SetRangeUser(1e-5,1);
  c1->SetLogy();
  h_spectrum_corr->Scale(1./n);
  h_spectrum_corr->Draw("e1");
  ((TH1F*)h_spectrum_shift->At(0))->Scale(1./n);
  ((TH1F*)h_spectrum_shift->At(0))->SetLineColor(kGray);
  ((TH1F*)h_spectrum_shift->At(0))->Draw("same");
  ((TH1F*)h_spectrum_shift->At(1))->Scale(1./n);
  ((TH1F*)h_spectrum_shift->At(1))->SetLineColor(kGray);
  ((TH1F*)h_spectrum_shift->At(1))->Draw("same");
  h_background->SetLineColor(kBlue+2);
  h_background->Draw("e1 same");
  h_spectrum_sub->Scale(1./n);
  h_spectrum_sub->Add(h_background,-1);
  h_spectrum_sub->SetLineColor(kRed+2);
  h_spectrum_sub->Draw("e1 same");

  // Prompt light
  c1 -> DrawClone();
  c1 -> cd();

  h_prompt->Scale(1./n);
  h_prompt->Divide(h_spectrum);
  for( int i = 1; i <= h_prompt->GetNbinsX(); i++ ) {
    h_prompt->SetBinContent(i,h_prompt->GetBinContent(i)/h_prompt->GetBinWidth(i));
    h_prompt->SetBinError(i,h_prompt->GetBinError(i)/h_prompt->GetBinWidth(i));
  }
  h_prompt->Draw("e1");

  // Total light
  c1 -> DrawClone();
  c1 -> cd();

  h_total->Scale(1./n);
  h_total->Divide(h_spectrum);
  for( int i = 1; i <= h_total->GetNbinsX(); i++ ) {
    h_total->SetBinContent(i,h_total->GetBinContent(i)/h_total->GetBinWidth(i));
    h_total->SetBinError(i,h_total->GetBinError(i)/h_total->GetBinWidth(i));
  }
  h_total->Draw("e1");

  // Ratio
  c1 -> DrawClone();
  c1 -> cd();

  h_ratio->Scale(1./n);
  h_ratio->Divide(h_spectrum);
  for( int i = 1; i <= h_ratio->GetNbinsX(); i++ ) {
    h_ratio->SetBinContent(i,h_ratio->GetBinContent(i)/h_ratio->GetBinWidth(i));
    h_ratio->SetBinError(i,h_ratio->GetBinError(i)/h_ratio->GetBinWidth(i));
  }
  h_ratio->Draw("e1");
}

// ~~~~~~~~~~~~~~~~~~~
// Helpers
// ~~~~~~~~~~~~~~~~~~~

Double_t time_to_E(Double_t time) { // expects units of sec
  // check for "bad" events
  if( time <= tof_length/c ) return -1;
    
  // calculate momentum (MeV/c)
  Double_t p = tof_length * (mass) / Sqrt( Power( time * c, 2) - Power( tof_length, 2) );
  Double_t E = Sqrt( Power(p,2) + Power(mass,2) ) - mass;
  
  return E;
}


Double_t E_err(Double_t time) {
  // check for "bad" events    
  if( time <= tof_length/c ) return -1;
  Double_t E = time_to_E(time);
  Double_t p = Sqrt(Power(E+mass,2) - Power(mass,2));

  // calculate momentum error (MeV/c)      
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

  Double_t E_err = p * p_err / Sqrt(Power(p,2) + Power(mass,2));
  //std::cout << E_err << std::endl;
  return E_err;
}

void Background(TH1* h) {
  for( int i = 1; i <= h->GetNbinsX(); i++ ) {
    Double_t dt = GetDt(h->GetBinLowEdge(i),h->GetBinLowEdge(i) + h->GetBinWidth(i));
    h->Fill(h->GetBinCenter(i), dt * background_rate);
  }
}

Double_t GetDt(Double_t e_low, Double_t e_high) {
  Double_t t_low  = TfromE(e_high);
  Double_t t_high = TfromE(e_low);
  
  return t_high - t_low;
}

Double_t TfromE(Double_t E) {
  return (E+mass)*tof_length/(c*Sqrt(Power(E+mass,2)-Power(mass,2)));
}

void Convolve( TH1* h, Double_t mean, Double_t sigma ) {
  TF1* myFunc = new TF1("myFunc","gaus",0,2e3);
  myFunc -> SetParameters(1, mean, sigma);

  for( int i = 1; i < h->GetNbinsX()+1; i++ ) {
    Double_t low = h->GetBinLowEdge(i);
    Double_t high = low + h->GetBinWidth(i);
    h->SetBinContent(i, h->GetBinContent(i) + myFunc->Integral(low,high));
  }
  /* cout << myFunc -> GetParameter(0) << "\t"
       << myFunc -> GetParameter(1) << "\t"
       << myFunc -> GetParameter(2) << "\t"
       << endl;*/

  myFunc -> Delete();
}

void Convolve( TH2* h, Double_t x, Double_t xsigma, Double_t y, Double_t ysigma) {
  TF1* myFunc_x = new TF1("fx","gaus",0,2e3);
  TF1* myFunc_y = new TF1("fy","gaus",0,2e3); 
  myFunc_x->SetParameters(1, x, xsigma);
  myFunc_y->SetParameters(1, y, ysigma);

  for( int i = 1; i < h->GetNbinsX(); i++ )
    for( int j = 1; j < h->GetNbinsY(); j++ ) {
      Double_t low_x = h->GetXaxis()->GetBinLowEdge(i);
      Double_t high_x = low_x + h->GetXaxis()->GetBinWidth(i);
      Double_t low_y = h->GetYaxis()->GetBinLowEdge(j);
      Double_t high_y = low_y + h->GetYaxis()->GetBinWidth(j);
      h->SetBinContent(i,j,h->GetBinContent(i,j) + myFunc_x->Integral(low_x,high_x) * myFunc_y->Integral(low_y,high_y));
    }
  /* cout << myFunc -> GetParameter(0) << "\t" 
     << myFunc -> GetParameter(1) << "\t"                                                    
     << myFunc -> GetParameter(2) << "\t"                                                    
     << endl;*/

  myFunc_x->Delete();
  myFunc_y->Delete();
}

TGraphAsymmErrors* AsymmProfileX(TH2* h) {
  TAxis* xaxis = h->GetXaxis();
  TAxis* yaxis = h->GetYaxis();
  TGraphAsymmErrors* p = new TGraphAsymmErrors();

  for( Int_t binx = 1; binx < xaxis->GetNbins()+1; binx++ ) {
    Double_t sum = 0;
    Double_t n   = 0;
    Double_t sqsum_high = 0;
    Double_t n_high     = 0;
    Double_t sqsum_low  = 0;
    Double_t n_low      = 0;
    for( Int_t biny = 1; biny < yaxis->GetNbins()+1; biny++ ) {
      sum += h->GetBinContent(binx,biny) * yaxis->GetBinCenter(biny);
      n   += h->GetBinContent(binx,biny);
    }
    if( n == 0 ) continue;
    Double_t mean = sum/n;
    p->SetPoint(binx,xaxis->GetBinCenter(binx),mean);

    for( Int_t biny = 1; biny <yaxis->GetNbins()+1; biny++ ) {
      if( yaxis->GetBinCenter(biny) >= mean ) {
	sqsum_high += Power(yaxis->GetBinCenter(biny) - mean,2) * h->GetBinContent(binx,biny);
	n_high     += h->GetBinContent(binx,biny);
      } else {
	sqsum_low += Power(yaxis->GetBinCenter(biny) - mean,2) * h->GetBinContent(binx,biny);
        n_low     += h->GetBinContent(binx,biny);
      }
    }
    Double_t err_high = Sqrt(sqsum_high/n_high);
    Double_t err_low  = Sqrt(sqsum_low/n_low);
    Double_t err_x    = xaxis->GetBinWidth(binx)/2;
    p->SetPointError(binx, err_x, err_x, err_low, err_high);
  }

  p->GetXaxis()->SetTitle(xaxis->GetTitle());
  p->GetYaxis()->SetTitle(yaxis->GetTitle());
  return p;
}
