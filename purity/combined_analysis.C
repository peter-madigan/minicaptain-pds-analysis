#include <iostream>
#include <vector>
#include <TCanvas.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>
#include <TFile.h>
#include <TMath.h>
#include <TAxis.h>

vector<Double_t> GetConcentrations(Double_t te, Double_t tsc, Double_t H2O);
vector<Double_t> GetErrors(Double_t te, Double_t ete, Double_t tsc, Double_t etsc, Double_t H2O, 
			   Double_t eH2O);
TGraphAsymmErrors* CleanUpPlot(TGraphAsymmErrors* g0);

using namespace TMath;

static const Double_t keO2  = 1.0/0.32; // 1 / usec ppm
static const Double_t keN2  = 1.0/320.0;// 1 / usec ppm
static const Double_t keH2O = 1.0/13.0; // 1 / usec ppm

static const Double_t kscO2 = 0.540; // 1 / usec ppm
static const Double_t kscN2 = 0.110; // 1 / usec ppm
static const Double_t tsc0  = 1.540; // usec

void combined_analysis(){
  TCanvas* c1 = new TCanvas();
  c1->Divide(3,1);

  TGraphErrors* e_LT = new TGraphErrors("e_lifetime.txt","%lg %lg %lg %lg","");
  c1->cd(1);
  e_LT->Draw("AP");

  TFile* scint_fi = new TFile("scint_lifetime.root","READ");
  TGraphAsymmErrors* scint_LT = (TGraphAsymmErrors*)scint_fi->Get("Graph");
  c1->cd(2);
  scint_LT->Draw("AP");

  TGraph* H2O_conc = new TGraph("H2O_conc.txt","%lg %lg","");
  H2O_conc->SetMarkerStyle(20);
  c1->cd(3);
  H2O_conc->Draw("AP");
  
  c1->Update();

  // Number of points
  UInt_t n_e = e_LT->GetN();
  UInt_t n_scint = scint_LT->GetN();
  UInt_t n_H2O = H2O_conc->GetN();

  // Values
  Double_t* t_e = e_LT->GetX();
  Double_t* y_e = e_LT->GetY();
  Double_t* et_e = e_LT->GetEX();
  Double_t* ey_e = e_LT->GetEY();

  Double_t* t_scint = scint_LT->GetX();
  Double_t* y_scint = scint_LT->GetY();
  Double_t* etl_scint = scint_LT->GetEXlow();
  Double_t* eth_scint = scint_LT->GetEXhigh();
  Double_t* eyl_scint = scint_LT->GetEYlow();
  Double_t* eyh_scint = scint_LT->GetEYhigh();

  Double_t* t_H2O = H2O_conc->GetX();
  Double_t* y_H2O = H2O_conc->GetY();

  // New arrays
  vector<Double_t> t_arr;
  vector<Double_t> et_arr;

  vector<Double_t> eLT_arr;
  vector<Double_t> eeLT_arr;
  vector<Double_t> tLT_arr;
  vector<Double_t> etLT_arr;
  vector<Double_t> H2O_arr;
  vector<Double_t> eH2O_arr;
  
  vector<Double_t> O2_arr;
  vector<Double_t> eO2_arr;
  vector<Double_t> N2_arr;
  vector<Double_t> eN2_arr;
  
  // Convert times
  for( Int_t i_e = 0; i_e < n_e; i_e++ )
    t_e[i_e] = (t_e[i_e] + 60) * 24 * 3600; // sec since jan 1                                  
  for( Int_t i_H2O = 0; i_H2O < n_H2O; i_H2O++ )
    t_H2O[i_H2O] = (t_H2O[i_H2O] - 40908) * 24 * 3600; // sec since jan 1
  
  // Loop over points
  Double_t window = 14*24*3600;
  for( Double_t current_time = t_scint[0]; current_time < t_scint[n_scint-1] + window; current_time+= window ) {
    // Store time window
    Int_t i_curr = t_arr.size();
    t_arr.push_back(current_time + window/2);
    et_arr.push_back(window/2);
    cout << "Loop " << i_curr + 1 << " started!" << endl;

    // Calculate mean scintillation lifetime                                         
    cout << "Calc scintillation lifetime!" << endl;
    Double_t n_temp = 0;
    Double_t neff_temp = 0;
    Double_t y_temp = 0;
    Double_t ey_temp = 0;
    for( Int_t i_scint = 0; i_scint < n_scint; i_scint++ )
      if( t_scint[i_scint] <= t_arr[i_curr] + et_arr[i_curr] &&
          t_scint[i_scint] >= t_arr[i_curr] - et_arr[i_curr] ) {
        n_temp++;
	neff_temp += eyl_scint[i_scint];
        y_temp += y_scint[i_scint] * eyl_scint[i_scint];
        ey_temp += Power(eyl_scint[i_scint]/y_scint[i_scint],2);
      }
    if( n_temp != 0 ) {
      y_temp  = y_temp/neff_temp;
      ey_temp = Sqrt(ey_temp/n_temp) * y_temp;
    }

    tLT_arr.push_back(y_temp / 1000);
    etLT_arr.push_back(ey_temp / 1000);
    
    // Calculate mean electron lifetime
    cout << "Calc electron lifetime!" << endl;
    n_temp = 0;
    neff_temp = 0;
    y_temp = 0;
    ey_temp = 0;
    for( Int_t i_e = 0; i_e < n_e; i_e++ )
      if( t_e[i_e] <= t_arr[i_curr] + et_arr[i_curr] &&
	  t_e[i_e] >= t_arr[i_curr] - et_arr[i_curr] ) {
	n_temp++;
	neff_temp += ey_e[i_e];
	y_temp += y_e[i_e] * ey_e[i_e];
	ey_temp += Power(ey_e[i_e]/y_e[i_e],2);
      }
    if( n_temp != 0 ) {
      y_temp  = y_temp/neff_temp;
      ey_temp = Sqrt(ey_temp/n_temp) * y_temp;
    }

    eLT_arr.push_back(y_temp);
    eeLT_arr.push_back(ey_temp);
    
    // Calculate mean H2O conc.
    cout << "Calc H2O concentration!" << endl;
    n_temp = 0;
    y_temp = 0;
    ey_temp = 0.2;
    for( Int_t i_H2O = 0; i_H2O < n_H2O; i_H2O++ )
      if( t_H2O[i_H2O] <= t_arr[i_curr] + et_arr[i_curr] &&
          t_H2O[i_H2O] >= t_arr[i_curr] - et_arr[i_curr] ) {
	n_temp++;
        y_temp += y_H2O[i_H2O];
      } 
    if(n_temp != 0 )
      y_temp  = y_temp/n_temp;
    
    H2O_arr.push_back(y_temp / 1000);
    eH2O_arr.push_back(ey_temp / 1000 / Sqrt(n_temp));

    // Calculate O2 and N2 concentration
    cout << "Calc O2 and N2 concentration!" << endl;
    vector<Double_t> concentrations = 
      GetConcentrations(eLT_arr[i_curr], tLT_arr[i_curr], H2O_arr[i_curr]);
    vector<Double_t> econcentrations = 
      GetErrors(eLT_arr[i_curr], eeLT_arr[i_curr], tLT_arr[i_curr], etLT_arr[i_curr], 
		H2O_arr[i_curr], eH2O_arr[i_curr]);
    H2O_arr[i_curr] *= 1000;
    eH2O_arr[i_curr] *= 1000;
    O2_arr.push_back(concentrations[0] * 1000);
    eO2_arr.push_back(econcentrations[0] * 1000);
    N2_arr.push_back(concentrations[1]);
    eN2_arr.push_back(econcentrations[1]);

    cout << "H2O: " << H2O_arr[i_curr] << "+/-" << eH2O_arr[i_curr] << "ppb" << endl;
    cout << "O2: " << O2_arr[i_curr] << "+/-" << eO2_arr[i_curr] << "ppb" << endl;
    cout << "N2: " << N2_arr[i_curr] << "+/-" << eN2_arr[i_curr] << "ppm" << endl;

    // Scale triplet lifetime (for plotting)
    tLT_arr[i_curr] *= 10;
    etLT_arr[i_curr] *= 10;

    cout << "Loop " << i_curr+1 << " done!" << endl;
  }

  // Create new plots
  TGraphAsymmErrors* gElectronLifetime = 
    new TGraphAsymmErrors(t_arr.size(),&t_arr[0],&eLT_arr[0],&et_arr[0],&et_arr[0],&eeLT_arr[0],&eeLT_arr[0]);
  gElectronLifetime = CleanUpPlot(gElectronLifetime);
  TGraphAsymmErrors* gScintillationLifetime = 
    new TGraphAsymmErrors(t_arr.size(),&t_arr[0],&tLT_arr[0],&et_arr[0],&et_arr[0],&etLT_arr[0],&etLT_arr[0]);
  gScintillationLifetime = CleanUpPlot(gScintillationLifetime);
  
  TGraphAsymmErrors* gH2OConcentration = 
    new TGraphAsymmErrors(t_arr.size(),&t_arr[0],&H2O_arr[0],&et_arr[0],&et_arr[0],&eH2O_arr[0],&eH2O_arr[0]);
  gH2OConcentration = CleanUpPlot(gH2OConcentration);
  TGraphAsymmErrors* gO2Concentration = 
    new TGraphAsymmErrors(t_arr.size(),&t_arr[0],&O2_arr[0],&et_arr[0],&et_arr[0],&eO2_arr[0],&eO2_arr[0]);
  gO2Concentration = CleanUpPlot(gO2Concentration);
  TGraphAsymmErrors* gN2Concentration = 
    new TGraphAsymmErrors(t_arr.size(),&t_arr[0],&N2_arr[0],&et_arr[0],&et_arr[0],&eN2_arr[0],&eN2_arr[0]);
  gN2Concentration = CleanUpPlot(gN2Concentration);

  // Draw plots
  TCanvas* c2 = new TCanvas();
  c2->cd()->SetLogy();
  gElectronLifetime->SetMarkerStyle(20);
  gElectronLifetime->SetMarkerColor(kBlue+2);
  gElectronLifetime->SetLineColor(kBlack);
  gElectronLifetime->SetFillColor(kWhite);
  gElectronLifetime->SetTitle("electron lifetime");
  gScintillationLifetime->SetMarkerStyle(21);
  gScintillationLifetime->SetMarkerColor(kRed+2);
  gScintillationLifetime->SetLineColor(kBlack);
  gScintillationLifetime->SetFillColor(kWhite);
  gScintillationLifetime->SetTitle("triplet lifetime (x10)");
  
  gElectronLifetime->GetYaxis()->SetRangeUser(5,50);
  gElectronLifetime->GetYaxis()->SetMoreLogLabels(1);
  gElectronLifetime->GetYaxis()->SetNoExponent(1);
  gElectronLifetime->GetYaxis()->SetTitle("Lifetime (#musec)");
  gElectronLifetime->GetXaxis()->SetTimeDisplay(1);
  gElectronLifetime->GetXaxis()->SetTimeFormat("%m/%d");
  gElectronLifetime->GetXaxis()->SetNdivisions(205);

  gElectronLifetime->Draw("AP");
  gScintillationLifetime->Draw("P same");

  c2->BuildLegend(0.7,0.8,0.9,0.9);
  c2->Update();

  TCanvas* c3 = new TCanvas();
  c3->cd();//->SetLogy();
  gH2OConcentration->SetMarkerStyle(20);
  gH2OConcentration->SetMarkerColor(kBlue+2);
  gH2OConcentration->SetLineColor(kBlack);
  gH2OConcentration->SetFillColor(kWhite);
  gH2OConcentration->SetTitle("[H_{2}O] (ppb)");
  gO2Concentration->SetMarkerStyle(21);
  gO2Concentration->SetMarkerColor(kRed+2);
  gO2Concentration->SetLineColor(kBlack);
  gO2Concentration->SetFillColor(kWhite);
  gO2Concentration->SetTitle("[O_{2}] (ppb)");
  gN2Concentration->SetMarkerStyle(22);
  gN2Concentration->SetMarkerColor(kGreen+2);
  gN2Concentration->SetLineColor(kBlack);
  gN2Concentration->SetFillColor(kWhite);
  gN2Concentration->SetTitle("[N_{2}] (ppm)");

  gH2OConcentration->GetYaxis()->SetRangeUser(0,30);
  gH2OConcentration->GetYaxis()->SetMoreLogLabels(1);
  gH2OConcentration->GetYaxis()->SetNoExponent(1);
  gH2OConcentration->GetYaxis()->SetTitle("Concentration (ppm [N_{2}] OR ppb [O_{2}] OR ppb [H_{2}O])");
  gH2OConcentration->GetXaxis()->SetTimeDisplay(1);
  gH2OConcentration->GetXaxis()->SetTimeFormat("%m/%d");
  gH2OConcentration->GetXaxis()->SetNdivisions(205);

  gH2OConcentration->Draw("AP");
  gO2Concentration->Draw("P same");
  gN2Concentration->Draw("P same");

  c3->BuildLegend(0.7,0.8,0.9,0.9);
  c3->Update();
}

vector<Double_t> GetConcentrations(Double_t te, Double_t tsc, Double_t H2O)
{
  vector<Double_t> v(2,0.0);
  if( te == 0 || tsc == 0 || H2O == 0 ) return v;

  // O2
  v[0] = - (keN2 * te * (tsc0 - tsc) - kscN2 * tsc * tsc0 + H2O * keH2O * kscN2 * te * tsc * tsc0);
  v[0] = v[0] / ((keO2 * kscN2 - keN2 * kscO2) * te * tsc * tsc0 );

  // N2
  v[1] = - (keO2 * te * (tsc - tsc0) + kscO2 * tsc * tsc0 - H2O * keH2O * kscO2 * te *tsc * tsc0);
  v[1] = v[1] / ((keO2 * kscN2 - keN2 * kscO2) * te * tsc * tsc0);

  return v;
}

vector<Double_t> GetErrors(Double_t te, Double_t ete, Double_t tsc, Double_t etsc, Double_t H2O,
                           Double_t eH2O)
{
  vector<Double_t> v(2,0.0);
  if( te == 0 || tsc == 0 || H2O == 0 ) return v;

  vector<Double_t> c = GetConcentrations(te, tsc, H2O);
  Double_t O2 = c[0];
  Double_t N2 = c[1];

  // O2 *only assuming fractional error*
  v[0] = O2 * Sqrt(Power(ete/te,2) + Power(etsc/tsc,2));
  
  // N2 *only assuming fraction error*  
  v[1] = N2 * Sqrt(Power(ete/te,2) + Power(etsc/tsc,2));

  return v;
}

TGraphAsymmErrors* CleanUpPlot(TGraphAsymmErrors* g0) 
{
  UInt_t n = g0->GetN();
  Double_t* x0 = g0->GetX();
  Double_t* y0 = g0->GetY();

  vector<Double_t> x;
  vector<Double_t> exl;
  vector<Double_t> exh;
  vector<Double_t> y;
  vector<Double_t> ey;
  for( Int_t i = 0; i < n; i++ )
    if( y0[i] != 0 ) {
      x.push_back(x0[i]);
      exl.push_back(g0->GetErrorXlow(i));
      exh.push_back(g0->GetErrorXhigh(i));
      y.push_back(y0[i]);
      ey.push_back(g0->GetErrorY(i));
    }

  g0->Delete();
  return new TGraphAsymmErrors(x.size(),&x[0],&y[0],&exl[0],&exh[0],&ey[0],&ey[0]);
}
