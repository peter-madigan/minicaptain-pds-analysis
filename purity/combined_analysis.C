#include <vector>
#include <TCanvas.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>
#include <TFile.h>
#include <TMath.h>

vector<Double_t> GetConcentrations(Double_t te, Double_t tsc, Double_t H2O);
vector<Double_t> GetErrors(Double_t te, Double_t ete, Double_t tsc, Double_t etsc, Double_t H2O, 
			   Double_t eH2O);

using namespace TMath;

void combined_analysis(){
  TCanvas* c1 = new TCanvas();
  c1->Divide(3,1);

  TGraphErrors* e_LT = new TGraphErrors("e_lifetime.txt","%lg %lg %lg %lg","");
  c1->cd(1)->Update();
  e_LT->Draw("AP");

  TFile* scint_fi = new TFile("scint_lifetime.root","READ");
  TGraphAsymmErrors* scint_LT = (TGraphAsymmErrors*)scint_fi->Get("Graph");
  c1->cd(2)->Update();
  scint_LT->Draw("AP");

  TGraph* H2O_conc = new TGraph("H2O_conc.txt","%lg %lg","");
  H2O_conc->SetMarkerStyle(20);
  c1->cd(3)->Update();
  H2O_conc->Draw("AP");

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
  Double_t t_arr[n_scint];
  Double_t etl_arr[n_scint];
  Double_t eth_arr[n_scint];

  Double_t eLT_arr[n_scint];
  Double_t eeLT_arr[n_scint];
  Double_t tLT_arr[n_scint];
  Double_t etLT_arr[n_scint];
  Double_t H2O_arr[n_scint];
  Double_t eH2O_arr[n_scint];
  
  Double_t O2_arr[n_scint];
  Double_t eO2_arr[n_scint];
  Double_t N2_arr[n_scint];
  Double_t eN2_arr[n_scint];

  // Loop over points
  for( Int_t i_scint = 0; i_scint < n_scint; i_scint++ ) {
    // Convert times
    if( i_scint == 0 ) {
      for( Int_t i_e = 0; i_e < n_e; i_e++ )
	t_e[i_e] = (t_e[i_e] + 60) * 24 * 3600; // sec since jan 1
      for( Int_t i_H2O = 0; i_H2O < n_H2O; i_H2O++ )
	t_H2O[i_H2O] = (t_H2O[i_H2O] - 40908) * 24 * 3600; // sec since jan 1
    }
    // Store mean scintillation lifetime
    t_arr[i_scint] = t_scint[i_scint];
    etl_arr[i_scint] = etl_scint[i_scint];
    eth_arr[i_scint] = eth_scint[i_scint];

    tLT_arr[i_scint] = y_scint[i_scint] / 1000;
    etLT_arr[i_scint] = eyl_scint[i_scint] / 1000;

    // Calculate mean electron lifetime
    Int_t n_temp = 0;
    Double_t y_temp = 0;
    Double_t ey_temp = 0;
    for( Int_t i_e = 0; i_e < n_e; i_e++ )
      if( t_e[i_e] <= t_scint[i_scint] + eth_scint[i_scint] && 
	  t_e[i_e] >= t_scint[i_scint] - etl_scint[i_scint] ) {
	n_temp++;
	y_temp += y_e[i_e];
	ey_temp += Power(ey_e[i_e],2);
      }
    if( n_temp != 0 ) {
      y_temp  = y_temp/n_temp;
      ey_temp = Sqrt(ey_temp/n_temp);
    }

    eLT_arr[i_scint] = y_temp;
    eeLT_arr[i_scint] = ey_temp;

    // Calculate mean H2O conc.
    n_temp = 0;
    y_temp = 0;
    ey_temp = 0.2;
    for( Int_t i_H2O = 0; i_H2O < n_H2O; i_H2O++ )
      if( t_H2O[i_H2O] <= t_scint[i_scint] + eth_scint[i_scint] &&
          t_H2O[i_H2O] >= t_scint[i_scint] - etl_scint[i_scint] ) {
	n_temp++;
        y_temp += y_H2O[i_H2O];
      } 
    if(n_temp != 0 )
      y_temp  = y_temp/n_temp;

    H2O_arr[i_scint] = y_temp;
    eH2O_arr[i_scint] = ey_temp;

    // Calculate O2 and N2 concentration
    vector<Double_t> concentrations = 
      GetConcentrations(eLT_arr[i_scint], tLT_arr[i_scint], H2O_arr[i_scint]);
    vector<Double_t> econcentrations = 
      GetErrors(eLT_arr[i_scint], eeLT_arr[i_scint], tLT_arr[i_scint], etLT_arr[i_scint], 
		H2O_arr[i_scint], eH2O_arr[i_scint]);
    O2_arr[i_scint] = concentrations[0];
    eO2_arr[i_scint] = econcentrations[0];
    N2_arr[i_scint] = concentrations[1];
    eO2_arr[i_scint] = econcentrations[1];
  }

  // Create new plots
  TGraphAsymmErrors* gElectronLifetime = 
    new TGraphAsymmErrors(n_scint,t_arr,eLT_arr,etl_arr,eth_arr,eeLT_arr,eeLT_arr);
  TGraphAsymmErrors* gScintillationLifetime = 
    new TGraphAsymmErrors(n_scint,t_arr,tLT_arr,etl_arr,eth_arr,etLT_arr,etLT_arr);
  
  TGraphAsymmErrors* gH2OConcentration = 
    new TGraphAsymmErrors(n_scint,t_arr,H2O_arr);
  TGraphAsymmErrors* gO2Concentration = 
    new TGraphAsymmErrors(n_scint,t_arr,O2_arr);
  TGraphAsymmErrors* gN2Concentration = 
    new TGraphAsymmErrors(n_scint,t_arr,N2_arr);

  // Draw plots
  TCanvas* c2 = new TCanvas();
  c2->cd();
  gElectronLifetime->SetMarkerStyle(20);
  gElectronLifetime->SetMarkerColor(kBlue+2);
  gElectronLifetime->SetLineColor(kBlack);
  gScintillationLifetime->SetMarkerStyle(21);
  gScintillationLifetime->SetMarkerColor(kRed+2);
  gScintillationLifetime->SetLineColor(kBlack);
  
  gElectronLifetime->Draw("AP");
  gScintillationLifetime->Draw("P same");

  TCanvas* c3 = new TCanvas();
  c3->cd();
  gH2OConcentration->SetMarkerStyle(20);
  gH2OConcentration->SetMarkerColor(kBlue+2);
  gH2OConcentration->SetLineColor(kBlack);
  gO2Concentration->SetMarkerStyle(21);
  gO2Concentration->SetMarkerColor(kRed+2);
  gO2Concentration->SetLineColor(kBlack);
  gN2Concentration->SetMarkerStyle(22);
  gN2Concentration->SetMarkerColor(kGreen+2);
  gN2Concentration->SetLineColor(kBlack);
  
  gH2OConcentration->Draw("AP");
  gO2Concentration->Draw("P same");
  gN2Concentration->Draw("P same");
}

vector<Double_t> GetConcentrations(Double_t te, Double_t tsc, Double_t H2O)
{
  vector<Double_t> v(2,0.0);
  return v;
}

vector<Double_t> GetErrors(Double_t te, Double_t ete, Double_t tsc, Double_t etsc, Double_t H2O,
                           Double_t eH2O)
{
  vector<Double_t> v(2,0.0);
  return v;
}
