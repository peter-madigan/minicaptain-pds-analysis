#include "../tutorials/fit/langaus.C"
#include <vector>

Double_t calib_time = -671.8e-9; // sec 
Double_t time_err  = 4.5e-9; // sec (width of gamma peak)
Double_t peak_err  = 2.5/6.5; // average width (ADC) over average 1pe peak (ADC)
Double_t tof_length = 23.2; // m -- 2016-3
Double_t length_err = 0.1; // m 

Double_t c    = 3e8; // m/s
Double_t mass = 939.565379; // MeV/c2 -- PDG 2012
Double_t mass_err = 0.000021; // MeV/c2 -- PDG 2012
Double_t background_rate  = 680 * 14; // Hz (measured by random trigger using 3pe sum threshold)
Double_t background_light = 930 * 14; // Hz (                      ''                          )
Double_t gamma_width = 0.5e-9; // s (error in gamma peak + error in TOF length)

using namespace TMath;
using namespace std;

static const Int_t kNPMTs = 15;
static const Int_t kMaxNHits = 200;
static const Double_t kIntToPE = 1./13.5; 

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

// langau fitting parameters
Double_t fr[2];
Double_t sv[4], pllo[4], plhi[4], fp[4], fpe[4];
Double_t chi2;
Int_t    ndf;
fr[0] = 0; fr[1] = 300;
pllo[0]=0.01; pllo[1]=0.1; pllo[2]=0; pllo[3]=0.1;
plhi[0]=100; plhi[1]=300; plhi[2]=1; plhi[3]=100;
sv[0]=10; sv[1]=30; sv[2]=1e-5; sv[3]=20;

void tof_to_spectrum() {
  cout << "Loading analysis trees..." << endl;
  gROOT -> ProcessLine(".X chainFiles.C");
  
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

  cout << "Creating bins..." << endl;
  vector<double> energybins;
  double width = 1e-9;
  for( double t = 1000e-9; time_to_E(t) < 900; t-=width ) {
    energybins.push_back(time_to_E(t));
    if( time_to_E(t) < 10 )
      t-=50*width-width;
    else if( time_to_E(t) < 100 )
      t-=10*width-width;
  }
  std::sort(energybins.begin(),energybins.end());
  vector<double> lightbins;
  width = 10;
  for( double pe = 0; pe < 300; pe+=width )
    lightbins.push_back(pe);

  vector<double> ratiobins;
  for( int i = 0; i < 200; i++ )
    ratiobins.push_back(i*(10./200));
  
  cout << "Creating plots..." << endl;
  TH1F* h_spectrum = new TH1F("h_spectrum",";Neutron E_{kin} (MeV);Frac. of triggers (MeV^{-1})",
			      energybins.size()-1,&energybins[0]);
  h_spectrum->Sumw2();
  std::vector<TH1F*> h_spectrum_sys;
  h_spectrum_sys.push_back(new TH1F("h_spectrum_p1",";Neutron E_{kin} (MeV);Frac. of triggers (MeV^{-1})",
                                    energybins.size()-1,&energybins[0]));
  h_spectrum_sys.push_back(new TH1F("h_spectrum_m1",";Neutron E_{kin} (MeV);Frac. of triggers (MeV^{-1})",
                                    energybins.size()-1,&energybins[0]));
  h_spectrum_sys[0]->Sumw2();
  h_spectrum_sys[1]->Sumw2();
  
  TH1F* h_prompt = new TH1F("h_prompt",";Neutron E_{kin} (MeV);Prompt light yield (pe / n MeV)",
			    energybins.size()-1,&energybins[0]);
  h_prompt->Sumw2();
  std::vector<TH1F*> h_prompt_sys;
  h_prompt_sys.push_back(new TH1F("h_prompt_p1",
				  ";Neutron E_{kin} (MeV);Prompt light yield (pe / n MeV)",
				  energybins.size()-1,&energybins[0]));
  h_prompt_sys.push_back(new TH1F("h_prompt_m1",
				  ";Neutron E_{kin} (MeV);Prompt light yield (pe / n MeV)",
				  energybins.size()-1,&energybins[0]));
  h_prompt_sys[0]->Sumw2();
  h_prompt_sys[1]->Sumw2();

  TH1F* h_total = new TH1F("h_total",";Neutron E_{kin} (MeV);Total light yield (pe / n MeV)",
			   energybins.size()-1,&energybins[0]);
  h_total->Sumw2();
  std::vector<TH1F*> h_total_sys;
  h_total_sys.push_back(new TH1F("h_total_p1",
                                  ";Neutron E_{kin} (MeV);Total light yield (pe / n MeV)",
                                  energybins.size()-1,&energybins[0]));
  h_total_sys.push_back(new TH1F("h_total_m1",
                                  ";Neutron E_{kin} (MeV);Total light yield (pe / n MeV)",
                                  energybins.size()-1,&energybins[0]));
  h_total_sys[0]->Sumw2();
  h_total_sys[1]->Sumw2();
  std::vector<TH1F*> h_total_bin;
  std::vector< std::vector<TH1F*> > h_total_bin_sys(energybins.size()-1, std::vector<TH1F*>(2));
  for( Int_t i = 0; i < energybins.size()-1; i++ ) {
    TH1F* new_hist = new TH1F(Form("h_total_%d",i),
			      Form("Energy bin: %.3g to %.3g MeV;Total light yield (pe / n MeV);Frac. of triggers (MeV^{-1})",energybins[i],energybins[i+1]),lightbins.size()-1,&lightbins[0]);
    h_total_bin.push_back(new_hist);
    h_total_bin[i]->Sumw2();
    std::vector<TH1F*> h_sys(2);
    h_sys[0] = (new TH1F(Form("h_total_p1_%d",i),
		      ";Total light yield (pe / n MeV);Frac. of triggers (MeV^{-1})",
		      lightbins.size()-1,&lightbins[0]));
    h_sys[1] = (new TH1F(Form("h_total_m1_%d",i),
		      ";Total light yield (pe / n MeV);Frac. of triggers (MeV^{-1})",
		      lightbins.size()-1,&lightbins[0]));
    h_sys[0]->Sumw2();
    h_sys[1]->Sumw2();
    h_total_bin_sys[i] = h_sys;
  }
  
  TH1F* h_ratio = new TH1F("h_ratio",";Neutron E_{kin} (MeV);Triplet/singlet (MeV^{-1})",
			   energybins.size()-1,&energybins[0]);
  h_ratio->Sumw2();
  std::vector<TH1F*> h_ratio_sys;
  h_ratio_sys.push_back(new TH1F("h_ratio_p1",
				 ";Neutron E_{kin} (MeV);Triplet/singlet (MeV^{-1})",
				 energybins.size()-1,&energybins[0]));
  h_ratio_sys.push_back(new TH1F("h_ratio_m1",
				 ";Neutron E_{kin} (MeV);Triplet/singlet (MeV^{-1})",
				 energybins.size()-1,&energybins[0]));
  h_ratio_sys[0]->Sumw2();
  h_ratio_sys[1]->Sumw2();

  TH1F* h_background = new TH1F("h_background",";Neutron E_{kin} (MeV);Frac. of triggers (MeV^{-1})",
				energybins.size()-1,&energybins[0]);
  h_background->Sumw2();
  TH1F* h_background_yield = new TH1F("h_background_yield",
				      ";Neutron E_{kin} (MeV);Total light yield (pe / n MeV)",
				      energybins.size()-1,&energybins[0]);
  h_background_yield->Sumw2();

  cout << "Looping over events..." << endl;
  // Loop over ev
  Int_t n = 0;
  for( Int_t ientry = 0; pdsEvTree -> GetEntry(ientry); ientry++ ) {
    if( ientry%(pdsEvTree->GetEntriesFast()/10) == 0 )
      std::cout << ientry << "/" << pdsEvTree->GetEntries() << std::endl;
    
    if( inBeamWindow ) {
      // Fill background
      n++;
      Background(h_background,background_rate);
      Background(h_background_yield,background_light);
    }
    
    if( pds_flag && inBeamWindow && pds_nevent==1 ) {
      // Find n KE
      Double_t time = (pds_time[0] - rf_time) * 1e-9 - calib_time + tof_length/c;;
      Double_t E = time_to_E(time);

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

      if( prompt_sum > 0 && sum-prompt_sum > 0 ) {
	// Fill histograms
	h_spectrum->Fill(E);
	h_total->Fill(E,sum);
	h_prompt->Fill(E,prompt_sum);
	h_ratio->Fill(E,(sum-prompt_sum)/prompt_sum);
	if( h_total->FindBin(E) > 0 && h_total->FindBin(E)-1 < h_total_bin.size() )
	  h_total_bin[h_total->FindBin(E)-1]->Fill(sum);

	// Fill shifted histograms
	time -= gamma_width;
	E = time_to_E(time);
	h_spectrum_sys[0]->Fill(E);
	h_prompt_sys[0]->Fill(E,prompt_sum);
	h_total_sys[0]->Fill(E,sum);
	h_ratio_sys[0]->Fill(E,(sum-prompt_sum)/prompt_sum);
	if( h_total->FindBin(E) > 0 && h_total->FindBin(E)-1 < h_total_bin_sys.size() )
          h_total_bin_sys[h_total->FindBin(E)-1][0]->Fill(sum);
	
	time += 2 * gamma_width;
	E = time_to_E(time);
	h_spectrum_sys[1]->Fill(E);
	h_prompt_sys[1]->Fill(E,prompt_sum);
	h_total_sys[1]->Fill(E,sum);
	h_ratio_sys[1]->Fill(E,(sum-prompt_sum)/prompt_sum);
	if( h_total->FindBin(E) > 0 && h_total->FindBin(E)-1 < h_total_bin_sys.size() )
          h_total_bin_sys[h_total->FindBin(E)-1][1]->Fill(sum);
      }
    }
  }  

  cout << "Drawing plots..." << endl;
  TCanvas* c1 = new TCanvas();
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1111);
  vector<Int_t> ColorTable;
  ColorTable.push_back(kBlue+2);
  ColorTable.push_back(kRed+2);
  ColorTable.push_back(kGreen+2);
  ColorTable.push_back(kViolet+2);
  ColorTable.push_back(kBlue);
  ColorTable.push_back(kRed);
  ColorTable.push_back(kGreen);
  ColorTable.push_back(kViolet);
  ColorTable.push_back(kBlue-2);
  ColorTable.push_back(kRed-2);
  ColorTable.push_back(kGreen-2);
  ColorTable.push_back(kViolet-2);
 
  // Basic spectrum with systematic
  c1->cd()->SetLogy(1);
  c1->cd()->SetLogx(1);
  
  h_background->Scale(1./n);
  h_spectrum->Scale(1./n);
  h_spectrum_sys[0]->Scale(1./n);
  h_spectrum_sys[1]->Scale(1./n);
  Normalize(h_background);
  Normalize(h_spectrum);
  Normalize(h_spectrum_sys[0]);
  Normalize(h_spectrum_sys[1]);
  //TGraphAsymmErrors* g_spectrum_err = CalculateSystematicError(h_spectrum,h_spectrum_sys);
  TH1F* g_spectrum_err = CalculateSystematicError(h_spectrum,h_spectrum_sys);

  h_spectrum_sys[0]->SetLineColor(kGray);
  h_spectrum_sys[1]->SetLineColor(kGray);
  h_spectrum_sys[0]->Draw("hist");
  h_spectrum_sys[1]->Draw("hist same");
  h_spectrum->SetLineColor(kBlack);
  h_spectrum->SetMarkerStyle(7);
  h_spectrum->Draw("e1 same");
  h_background->SetLineColor(kRed+2);
  h_background->SetMarkerColor(kRed+2);
  h_background->SetMarkerStyle(7);
  h_background->Draw("e1 same");
  g_spectrum_err->SetMarkerStyle(7);
  g_spectrum_err->SetLineColor(kBlue+2);
  g_spectrum_err->Add(h_background,-1);
  g_spectrum_err->Draw("e1 same");
  //g_spectrum_err->Draw("same PZ");

  c1->SaveAs("plots/spectrum.C");

  // Prompt light
  c1->DrawClone();
  c1->cd()->SetLogy(0);
  c1->cd()->SetLogx(0);

  h_prompt->Scale(1./n);
  h_prompt->Divide(h_spectrum);
  h_prompt_sys[0]->Scale(1./n);
  h_prompt_sys[0]->Divide(h_spectrum);
  h_prompt_sys[1]->Scale(1./n);
  h_prompt_sys[1]->Divide(h_spectrum);
  h_background_yield->Scale(1./n);
  h_background_yield->Divide(h_spectrum);
  Normalize(h_prompt);
  Normalize(h_prompt_sys[0]);
  Normalize(h_prompt_sys[1]);
  Normalize(h_background_yield);
  //TGraphAsymmErrors* g_prompt_err = CalculateSystematicError(h_prompt,h_prompt_sys);
  TH1F* g_prompt_err = CalculateSystematicError(h_prompt,h_prompt_sys);
  h_prompt_sys[0]->SetLineColor(kGray);
  h_prompt_sys[1]->SetLineColor(kGray);
  h_prompt_sys[0]->Draw("hist");
  h_prompt_sys[1]->Draw("hist same");
  h_prompt->SetLineColor(kBlack);
  h_prompt->SetMarkerStyle(7);
  h_prompt->Draw("e1 same");
  h_background_yield->SetLineColor(kRed+2);
  h_background_yield->SetMarkerColor(kRed+2);
  h_background_yield->SetMarkerStyle(7);
  h_background_yield->Draw("e1 same");
  g_prompt_err->SetMarkerStyle(7);
  g_prompt_err->SetLineColor(kBlue+2);
  g_prompt_err->Add(h_background_yield,-1);
  //g_prompt_err->Draw("same PZ");
  g_prompt_err->Draw("same e");

  c1->SaveAs("plots/spectrum-prompt.C");

  // Total light
  c1 -> DrawClone();
  c1 -> cd();
  c1->SetLogy(0);
  c1->SetLogx(0);

  h_total->Scale(1./n);
  h_total->Divide(h_spectrum);
  h_total_sys[0]->Scale(1./n);
  h_total_sys[0]->Divide(h_spectrum);
  h_total_sys[1]->Scale(1./n);
  h_total_sys[1]->Divide(h_spectrum);
  Normalize(h_total);
  Normalize(h_total_sys[0]);
  Normalize(h_total_sys[1]);
  //TGraphAsymmErrors* g_total_err = CalculateSystematicError(h_total,h_total_sys);
  TH1F* g_total_err = CalculateSystematicError(h_total,h_total_sys);
  h_total_sys[0]->SetLineColor(kGray);
  h_total_sys[1]->SetLineColor(kGray);
  h_total_sys[0]->Draw("hist");
  h_total_sys[1]->Draw("hist same");
  h_total->SetLineColor(kBlack);
  h_total->SetMarkerStyle(7);
  h_total->Draw("e1 same");
  h_background_yield->Draw("e1 same");
  g_total_err->SetLineColor(kBlue+2);
  g_total_err->SetMarkerStyle(7);
  g_total_err->Add(h_background_yield,-1);
  //g_total_err->Draw("same PZ");
  g_total_err->Draw("same e");

  c1->SaveAs("plots/spectrum-total.C");

  c1->DrawClone();
  c1->cd()->Clear();
  c1->SetLogy(0);
  c1->SetLogx(0);
  
  Int_t color = 0;
  TLegend* leg = new TLegend(0.5,0.6,0.9,0.9);
  TH1F* g_total_MPV = (TH1F*)h_spectrum->Clone("g_total_MPV");
  TH1F* g_resolution = (TH1F*)h_spectrum->Clone("g_resolution");
  g_total_MPV->Reset();
  g_total_MPV->GetXaxis()->SetTitle("Neutron E_{kin} (MeV)");
  g_total_MPV->GetYaxis()->SetTitle("MPV total photon yield (pe / n MeV)");
  g_resolution->Reset();
  g_resolution->GetXaxis()->SetTitle("Neutron E_{kin} (MeV)");
  g_resolution->GetYaxis()->SetTitle("Resolution (Sigma / MPV)");
  TF1* fit = new TF1("langau",langaufun,fr[0],fr[1],4);
  fit->SetParNames("Width","MP","Area","GSigma");
  for( int i = 0; i < 4; i++ )
    fit->SetParLimits(i,pllo[i],plhi[i]);
  
  for( int i = 0; i < h_total_bin.size(); i++ ) {
    //color = 0;
    h_total_bin[i]->Scale(1./n);
    h_total_bin[i]->Scale(h_spectrum->GetBinContent(i+1));
    h_total_bin[i]->Scale(1./h_spectrum->GetBinWidth(i+1));
    h_total_bin_sys[i][0]->Scale(1./n);
    h_total_bin_sys[i][0]->Scale(h_spectrum->GetBinContent(i+1));
    h_total_bin_sys[i][0]->Scale(1./h_spectrum->GetBinWidth(i+1));
    h_total_bin_sys[i][1]->Scale(1./n);
    h_total_bin_sys[i][1]->Scale(h_spectrum->GetBinContent(i+1));
    h_total_bin_sys[i][1]->Scale(1./h_spectrum->GetBinWidth(i+1));
    h_total_bin[i] = CalculateSystematicError(h_total_bin[i],h_total_bin_sys[i]);
    h_total_bin[i]->SetMarkerStyle(7);
    
    TFitResultPtr guess = h_total_bin[i]->Fit("landau","S 0 Q");
    if( (Int_t)guess != 0 ) continue;
    sv[0]=guess->Parameter(2); 
    sv[1]=guess->Parameter(1);
    sv[2]=h_total_bin[i]->Integral("w"); 
    sv[3]=sv[0];
    cout << "Guess: " << sv[0] << " " << sv[1] << " " << sv[2] << " " << sv[3] << endl;
    fit->SetParameters(sv);
    TFitResultPtr fit_result = h_total_bin[i]->Fit("langau","S0RB");
    if( (Int_t)fit_result != 0 || fit_result->Ndf() <= 0 ) continue;

    g_total_MPV->SetBinContent(i+1,fit->GetParameter(1));
    g_total_MPV->SetBinError(i+1,fit->GetParError(1));
    g_resolution->SetBinContent(i+1,fit->GetParameter(3)/fit->GetParameter(1));
    g_resolution->SetBinError(i+1,Sqrt(Power(fit->GetParError(3)/fit->GetParameter(1),2) + Power(fit->GetParError(1)*fit->GetParameter(3)/Power(fit->GetParameter(1),2),2)));
    
    cout << "Filled with: " << g_total_MPV->GetBinContent(i+1) << " " << g_total_MPV->GetBinError(i+1) << endl;

    if( h_spectrum->FindBin(10) == i+1 ||
	h_spectrum->FindBin(50) == i+1 ||
	h_spectrum->FindBin(100) == i+1 ||
	h_spectrum->FindBin(350) == i+1 ||
	h_spectrum->FindBin(500) == i+1 ||
	h_spectrum->FindBin(750) == i+1 ) {
      h_total_bin[i]->GetYaxis()->SetRangeUser(1e-9,4e-7);
      h_total_bin[i]->SetTitle(Form("%s    #chi^{2}/ndf = %.2g/%d",h_total_bin[i]->GetTitle(),fit->GetChisquare(),fit->GetNDF()));
      h_total_bin[i]->SetLineColor(ColorTable[color]);
      h_total_bin[i]->SetMarkerColor(ColorTable[color]);
      fit->SetLineColor(ColorTable[color]);
      h_total_bin[i]->Draw("e same");
      fit->DrawCopy("l same");
      //c1->Update();
      leg->AddEntry(h_total_bin[i]);
      color++;
    }
  }
  leg->Draw("same");

  c1->SaveAs("plots/spectrum-fits.C");

  c1->DrawClone();
  c1->cd();

  g_total_MPV->SetMarkerColor(kBlack);
  g_total_MPV->SetLineColor(kBlack);
  g_total_MPV->SetMarkerStyle(7);
  g_total_MPV->Draw("e1");
  h_background_yield->Draw("e1 same");

  c1->SaveAs("plots/spectrum-MPV.C");

  c1->DrawClone();
  c1->cd()->SetLogy();

  g_resolution->SetMarkerColor(kBlack);
  g_resolution->SetLineColor(kBlack);
  g_resolution->SetMarkerStyle(7);
  g_resolution->GetYaxis()->SetRangeUser(0.1,10);
  g_resolution->Draw("e1");

  c1->SaveAs("plots/spectrum-resolution.C");

  // Ratio
  c1->DrawClone();
  c1->cd();
  c1->SetLogx(0);
  c1->SetLogy(0);

  h_ratio->Scale(1./n);
  h_ratio->Divide(h_spectrum);
  h_ratio_sys[0]->Scale(1./n);
  h_ratio_sys[0]->Divide(h_spectrum);
  h_ratio_sys[1]->Scale(1./n);
  h_ratio_sys[1]->Divide(h_spectrum);
  Normalize(h_ratio);
  Normalize(h_ratio_sys[0]);
  Normalize(h_ratio_sys[1]);
  //TGraphAsymmErrors* g_ratio_err = CalculateSystematicError(h_ratio,h_ratio_sys);
  TH1F* g_ratio_err = CalculateSystematicError(h_ratio,h_ratio_sys);
  h_ratio_sys[0]->SetLineColor(kGray);
  h_ratio_sys[1]->SetLineColor(kGray);
  h_ratio_sys[0]->Draw("hist");
  h_ratio_sys[1]->Draw("hist same");
  h_ratio->SetLineColor(kBlack);
  h_ratio->SetMarkerStyle(7);
  h_ratio->Draw("e1 same");
  g_ratio_err->SetMarkerStyle(7);
  g_ratio_err->SetLineColor(kBlue+2);
  //g_ratio_err->Draw("same PZ");
  g_ratio_err->Draw("same e");

  c1->SaveAs("plots/spectrum-ratio.C");
}

// ~~~~~~~~~~~~~~~~~~~
// Helpers
// ~~~~~~~~~~~~~~~~~~~
TH1F* CalculateSystematicError(TH1F* h, std::vector<TH1F*> h_sys) {
  TH1F* h_new = (TH1F*)h->Clone(TString(h->GetName()) + "_withsys");
  std::vector<Double_t> x(h->GetNbinsX(),0);
  std::vector<Double_t> x_err(h->GetNbinsX(),0);
  std::vector<Double_t> y(h->GetNbinsX(),0);
  std::vector<Double_t> y_err_low(h->GetNbinsX(),0);
  std::vector<Double_t> y_err_high(h->GetNbinsX(),0);
  
  for( int i = 1; i < h->GetNbinsX()+1; i++ ) {
    x[i-1] = h->GetBinCenter(i);
    x_err[i-1] = h->GetBinWidth(i)/2;

    y[i-1] = h->GetBinContent(i);
    Double_t v1 = h_sys[0]->GetBinContent(i);
    Double_t v2 = h_sys[1]->GetBinContent(i);
    Double_t err = h->GetBinError(i);
    y_err_low[i-1] = Sqrt(Power(v1 - y[i-1],2)/2 + Power(v2 - y[i-1],2)/2 + Power(err,2));
    y_err_high[i-1] = Sqrt(Power(v1 - y[i-1],2)/2 + Power(v2 - y[i-1],2)/2 + Power(err,2));

    h_new->SetBinContent(i,y[i-1]);
    h_new->SetBinError(i,y_err_low[i-1]);
  }
  
  /*
    TGraphAsymmErrors* g = new TGraphAsymmErrors(x.size(),&x[0],&y[0],&x_err[0],&x_err[0],
					       &y_err_low[0],&y_err_high[0]);
					       g->GetXaxis()->SetTitle(h->GetXaxis()->GetTitle());
					       g->GetYaxis()->SetTitle(h->GetYaxis()->GetTitle());
  return g;
  */
  return h_new;
}

void Normalize(TH1* h) {
  for( int i = 1; i < h->GetNbinsX()+1; i++ ) {
    h->SetBinContent(i,h->GetBinContent(i)/h->GetBinWidth(i));
    h->SetBinError(i,h->GetBinError(i)/h->GetBinWidth(i));
  }
}

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

void Background(TH1* h, Double_t rate) {
  for( int i = 1; i <= h->GetNbinsX(); i++ ) {
    Double_t dt = GetDt(h->GetBinLowEdge(i),h->GetBinLowEdge(i) + h->GetBinWidth(i));
    h->Fill(h->GetBinCenter(i), dt * rate);
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

