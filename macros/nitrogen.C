#include <vector>
#include <TStyle.h>
#include <TObjArray.h>
#include <TChain.h>
#include <iostream>
#include <TF1.h>
#include <TObject.h>
#include <TH1F.h>
#include <TGraphAsymmErrors.h>
#include <TFitResult.h>
#include <TMath.h>
#include <TCanvas.h>

using namespace TMath;
using namespace std;

void nitrogen();
vector<Double_t> GetN2AndError(TFitResultPtr& f);
vector<Double_t> GetO2AndError(TFitResultPtr& f);
vector<Double_t> GettTAndError(TFitResultPtr& f);
vector<Double_t> GetRatioAndError(TFitResultPtr& f);

void nitrogen() {
  TCanvas* c1 = new TCanvas();

  static const Int_t kNPMTs = 15;
  static const Int_t kMaxNHits = 200;
  static const Double_t kDelay = -671.25 - 77.267;
  static const Double_t kIntToPE = 0.07084;
  
  UShort_t gps_yr, gps_d;
  UInt_t   gps_s,  gps_ns;
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
  
  gStyle->SetStatX(0.9);
  gStyle->SetStatY(0.9);
  gStyle->SetOptFit(1111);
  gStyle->SetOptStat(0);
  
  // Set up chains for runs
  static const Int_t start_run = 6166;
  static const Int_t end_run   = 6403;
  static const Int_t nch = (end_run - start_run)/10 + 1;
  static const Int_t nrun = 10;
  TObjArray* ch = new TObjArray();
  for( Int_t ich = 0; ich < nch; ich++ )
    ch->Add(new TChain("pdsEvTree","pdsEvTree"));
  
  int ich = 0;
  int i = 0;
  cout << nrun << " runs per chain." << endl;
  for( i = start_run; i <= end_run; i+=nrun ) {
    if( ich == nch-1 ) break;
    cout << "Chain #" << ich << " contains runs ( ";
    for( int j = 0; j < nrun && i+j <= end_run && ich < nch-1; j++ ) {
      cout << i+j << " ";
      ((TChain*)ch->At(ich))->Add(Form("data/pdsTree%04d/pds*",i+j));
    }
    cout << ")..." << endl;
    ich++;
  }
  cout << "Chain #" << ich << " contains runs ( ";
  for( int j = i; j <= end_run; j++ ) {
    cout << j << " ";
    ((TChain*)ch->At(ich))->Add(Form("data/pdsTree%04d/pds*",j));
  }
  cout << ")..." << endl;
  
  // Set up plots
  Int_t nbins = 25;
  Double_t tmin = 0;
  Double_t tmax = 3.2e3;
  Double_t rmin = 0.2;			  
  Double_t rmax = 5;
  TObjArray* hist_lifetime = new TObjArray();
  TObjArray* hist_ratio    = new TObjArray();
  for( Int_t ich = 0; ich < nch; ich++ ) {
    hist_lifetime->Add(new TH1F(Form("hist_lifetime%d",ich),";dt (ns);triplet pe",nbins,tmin,tmax));
    hist_ratio->Add(new TH1F(Form("hist_ratio%d",ich),";triplet/singlet;count",nbins*2,rmin,rmax));
  }

  // Set up times
  vector<Double_t> time;
  vector<Float_t>  timeBins;

  // Set up fit
  vector<TFitResultPtr> fit;
  vector<TFitResultPtr> fit_ratio;
  TF1* fit_func = new TF1("fit","exp([0]-x/[1])",500,2500);
  fit_func->SetParameters(2,1000);
  fit_func->SetParLimits(1,0.1,1e5);

  // Set branches
  for( int ich = 0; ich < nch; ich++ ) {
    if(	start_run + ich == 6318 ||
	start_run + ich == 6379 ||
	start_run + ich == 6380 ||
	start_run + ich == 6381 ||
	start_run + ich == 6353 ||
	start_run + ich == 6352 )
      continue;

    if( ((TChain*)ch->At(ich))->GetEntries() == 0 ) continue;
    ((TChain*)ch->At(ich))->SetBranchStatus("*", kFALSE);

    ((TChain*)ch->At(ich))->SetBranchAddress("gps_yr",&gps_yr);
    ((TChain*)ch->At(ich))->SetBranchAddress("gps_d",&gps_d);
    ((TChain*)ch->At(ich))->SetBranchAddress("gps_s",&gps_s);
    ((TChain*)ch->At(ich))->SetBranchAddress("gps_ns",&gps_ns);

    ((TChain*)ch->At(ich))->SetBranchAddress("pds_nevent",&pds_nevent);
    ((TChain*)ch->At(ich))->SetBranchAddress("pds_hits",&pds_hits);
    ((TChain*)ch->At(ich))->SetBranchAddress("pmt_hits",pmt_hits);
    ((TChain*)ch->At(ich))->SetBranchAddress("pds_time",pds_time);
    ((TChain*)ch->At(ich))->SetBranchAddress("pmt_time",pmt_time);
    ((TChain*)ch->At(ich))->SetBranchAddress("pds_peak",pds_peak);
    ((TChain*)ch->At(ich))->SetBranchAddress("pmt_peak",pmt_peak);
    ((TChain*)ch->At(ich))->SetBranchAddress("pds_integral",pds_integral);
    ((TChain*)ch->At(ich))->SetBranchAddress("pmt_integral",pmt_integral);
    ((TChain*)ch->At(ich))->SetBranchAddress("pds_flag",&pds_flag);
    ((TChain*)ch->At(ich))->SetBranchAddress("pmt_flag",pmt_flag);
    ((TChain*)ch->At(ich))->SetBranchAddress("pds_FWHM",pds_FWHM);
    ((TChain*)ch->At(ich))->SetBranchAddress("pmt_FWHM",pmt_FWHM);
    ((TChain*)ch->At(ich))->SetBranchAddress("inBeamWindow",&inBeamWindow);
    ((TChain*)ch->At(ich))->SetBranchAddress("isBeamTrigger",&isBeamTrigger);
    ((TChain*)ch->At(ich))->SetBranchAddress("rf_time",&rf_time);

    ((TChain*)ch->At(ich))->SetBranchStatus("pds_nevent",kTRUE);
    ((TChain*)ch->At(ich))->SetBranchStatus("pds_hits",kTRUE);
    ((TChain*)ch->At(ich))->SetBranchStatus("pmt_hits",kTRUE);
    ((TChain*)ch->At(ich))->SetBranchStatus("pds_time",kTRUE);
    ((TChain*)ch->At(ich))->SetBranchStatus("pmt_time",kTRUE);
    ((TChain*)ch->At(ich))->SetBranchStatus("pds_peak",kTRUE);
    ((TChain*)ch->At(ich))->SetBranchStatus("pmt_peak",kTRUE);
    ((TChain*)ch->At(ich))->SetBranchStatus("pds_integral",kTRUE);
    ((TChain*)ch->At(ich))->SetBranchStatus("pmt_integral",kTRUE);
    ((TChain*)ch->At(ich))->SetBranchStatus("pds_flag",kTRUE);
    ((TChain*)ch->At(ich))->SetBranchStatus("pmt_flag",kTRUE);
    ((TChain*)ch->At(ich))->SetBranchStatus("pds_FWHM",kTRUE);
    ((TChain*)ch->At(ich))->SetBranchStatus("pmt_FWHM",kTRUE);
    ((TChain*)ch->At(ich))->SetBranchStatus("inBeamWindow",kTRUE);
    ((TChain*)ch->At(ich))->SetBranchStatus("isBeamTrigger",kTRUE);
    ((TChain*)ch->At(ich))->SetBranchStatus("rf_time",kTRUE);
  
    // Loop over events
    Double_t meanTime = 0.;
    Double_t minTime = 0;
    Double_t maxTime = 0;
    for( Int_t i = 0; ((TChain*)ch->At(ich))->GetEntry(i); i++ ) {
      meanTime += gps_ns * 1e-9 + gps_s + gps_d * 3600 * 24;
      if( i == 0 ) 
	minTime = gps_ns * 1e-9 + gps_s + gps_d * 3600 * 24;
      if( i == ((TChain*)ch->At(ich))->GetEntries()-1 )
	maxTime = gps_ns * 1e-9 + gps_s + gps_d * 3600 * 24;
      // Status update 
      if( ((TChain*)ch->At(ich))->GetEntries()/10 > 0 && i%(((TChain*)ch->At(ich))->GetEntries()/10) == 0 )
	cout << "Chain #" << ich << ": " 
	     << i << " of " << ((TChain*)ch->At(ich))->GetEntriesFast() << "\n"
	     << "Time: " << gps_ns * 1e-9 + gps_s + gps_d * 3600 * 24 << endl;
      
      // Cut events
      if( !inBeamWindow && pds_flag && pds_nevent == 1 && pds_integral[0] > 5 ) {
	// Loop over PDS
	//Double_t TOF = pds_time[0] - rf_time - kDelay;
	Double_t TOF_hit = 0;
	Double_t triplet = 0;
	Double_t singlet = 0;
	for( Int_t pmt = 0; pmt < kNPMTs; pmt++ ) {
	  if( pmt_flag[pmt] ) {
	    if( Abs(pmt_time[pmt][0]-pds_time[0]) < 30 )
	      singlet += pmt_integral[pmt][0];
	    for( Int_t j = 0; j < pmt_hits[pmt]; j++) {
	      TOF_hit = pmt_time[pmt][j] - pds_time[0];
	      if( pmt_time[pmt][j] > pds_time[0] && 1)
		//TOF < 103.28+77.267 && TOF > 14.54+77.267) ) // approx 100MeV - 800MeV
		((TH1F*)hist_lifetime->At(ich))->Fill(TOF_hit,pmt_integral[pmt][j]);
	      if( pmt_time[pmt][j] > pds_time[0] &&
		  Abs(pmt_time[pmt][j]-pds_time[0]) > 30)
		triplet += pmt_integral[pmt][j];
	    }
	  }
	}
	if( singlet != 0 || triplet != 0 )
	  ((TH1F*)hist_ratio->At(ich))->Fill(triplet/singlet);
      }
    }
    
    meanTime = meanTime/((TChain*)ch->At(ich))->GetEntries();
    timeBins.push_back(minTime);
    timeBins.push_back(maxTime);
    time.push_back(meanTime);
    cout << "Average run time: " << time[time.size()-1] << endl;
    
    fit.push_back( ((TH1F*)hist_lifetime->At(ich))->Fit("fit","srq") );
    cout << "Lifetime fit: " << (Int_t)fit[fit.size()-1] << endl;
    cout << "tT: " << GettTAndError(fit[fit.size()-1])[0] << "ns" << endl;
    cout << "N2: " << GetN2AndError(fit[fit.size()-1])[0] << "ppm" << endl;
    cout << "O2: " << GetO2AndError(fit[fit.size()-1])[0] << "ppm" << endl;

    fit_ratio.push_back( ((TH1F*)hist_ratio->At(ich))->Fit("gaus","sq") );
    cout << "Ratio fit: " << (Int_t)fit_ratio[fit_ratio.size()-1] << endl;
    
    //(TH1F*)hist_ratio->At(ich)->TH1F::Draw("e");    
    //c1->SetLogy();
    //c1->Update();

    hist_lifetime->At(ich)->Draw("e");
    c1->SetLogy();
    c1->Update();
  }
  cout << "Creating plots..." << endl;

  vector<Double_t> N2;
  vector<Double_t> N2_err;
  vector<Double_t> tT;
  vector<Double_t> tT_err;
  vector<Double_t> O2;
  vector<Double_t> O2_err;
  vector<Double_t> r;
  vector<Double_t> r_err;
  vector<Double_t> time_low;
  vector<Double_t> time_high;

  for( Int_t i = 0; i < time.size(); i++ ) {
    cout << "Fit: " << i << ", " << fit[i] << ", " << fit_ratio[i] << endl;
    cout << "Time: " << time[i] << " from -" << time[i] - timeBins[i*2] << " to +" << timeBins[i*2+1] - time[i] << endl;

    vector<Double_t> N2_curr = GetN2AndError(fit[i]);
    vector<Double_t> tT_curr = GettTAndError(fit[i]);
    vector<Double_t> O2_curr = GetO2AndError(fit[i]);
    vector<Double_t> ratio_curr = GetRatioAndError(fit_ratio[i]);

    cout << "tT: " << tT_curr[0] << "+/-"<< tT_curr[1] << "ns" << "\n"
	 << "r:  " << ratio_curr[0] << "+/-"<< ratio_curr[1] << "\n"
	 << "N2: " << N2_curr[0] << "+/-"<< N2_curr[1] << "ppm" << "\n"
	 << "O2: " << O2_curr[0] << "+/-"<< O2_curr[1] << "ppm" << endl;

    if( tT_curr[0] > 0 ) {
      N2.push_back( N2_curr[0] );
      N2_err.push_back( N2_curr[1] );
      tT.push_back( tT_curr[0] );
      tT_err.push_back( tT_curr[1] );
      O2.push_back( O2_curr[0] );
      O2_err.push_back( O2_curr[1] );
      r.push_back( ratio_curr[0] );
      r_err.push_back( ratio_curr[1] );

      Double_t tl = time[i] - timeBins[i*2];
      Double_t th = timeBins[i*2+1] - time[i];
      time_low.push_back( tl );
      time_high.push_back( th );
    }
    cout << "Done." << endl;
  }

  cout << "Graphing..." << endl;
  TGraphAsymmErrors* hr = new TGraphAsymmErrors(time.size(),&(time[0]),&(r[0]),&(time_low[0]),&(time_high[0]),&(r_err[0]),&(r_err[0]));
  TGraphAsymmErrors* hN2 = new TGraphAsymmErrors(time.size(),&(time[0]),&(N2[0]),&(time_low[0]),&(time_high[0]),&(N2_err[0]),&(N2_err[0]));
  TGraphAsymmErrors* hO2 = new TGraphAsymmErrors(time.size(),&(time[0]),&(O2[0]),&(time_low[0]),&(time_high[0]),&(O2_err[0]),&(O2_err[0]));
  TGraphAsymmErrors* htT = new TGraphAsymmErrors(time.size(),&(time[0]),&(tT[0]),&(time_low[0]),&(time_high[0]),&(tT_err[0]),&(tT_err[0]));
  
  hN2->SetTitle("");
  hN2->SetLineColor(kBlack);
  hN2->SetMarkerStyle(20);
  hN2->GetXaxis()->SetTimeDisplay(1);
  hN2->GetXaxis()->SetTimeFormat("%m/%d%p");
  hN2->GetYaxis()->SetTitle("Eq. [N_{2}] (ppm)");
  //hN2->GetXaxis()->SetNdivisions(205);
  hN2->Draw("AP");

  c1->SetLogy(0);
  c1->SetGridx(); c1->SetGridy();
  hN2->SaveAs("plots/nitrogen-eqN2.C");

  c1->DrawClone();
  htT->SetTitle("");
  htT->SetLineColor(kBlack);
  htT->SetMarkerStyle(20);
  htT->GetXaxis()->SetTimeDisplay(1);
  htT->GetXaxis()->SetTimeFormat("%m/%d%p");
  htT->GetYaxis()->SetTitle("#tau_{T} (ns)");
  //htT->GetXaxis()->SetNdivisions(205);                    
  htT->Draw("AP");

  c1->SetLogy(0);
  c1->SetGridx(); c1->SetGridy();
  htT->SaveAs("plots/nitrogen-tT.C");

  c1->DrawClone();
  hO2->SetTitle("");
  hO2->SetLineColor(kBlack);
  hO2->SetMarkerStyle(20);
  hO2->GetXaxis()->SetTimeDisplay(1);
  hO2->GetXaxis()->SetTimeFormat("%m/%d%p");
  hO2->GetYaxis()->SetTitle("Eq. [O_{2}] (ppm)");
  //h02->GetXaxis()->SetNdivisions(205);                                             
  hO2->Draw("AP");

  c1->SetLogy(0);
  c1->SetGridx(); c1->SetGridy();
  hO2->SaveAs("plots/nitrogen-eqO2.C");

  c1->DrawClone();
  hr->SetTitle("");
  hr->SetLineColor(kBlack);
  hr->SetMarkerStyle(20);
  hr->GetXaxis()->SetTimeDisplay(1);
  hr->GetXaxis()->SetTimeFormat("%m/%d%p");
  hr->GetYaxis()->SetTitle("Triplet/singlet ratio");
  //hr->GetXaxis()->SetNdivisions(205);                                                            
  hr->Draw("AP");

  c1->SetLogy(0);
  c1->SetGridx(); c1->SetGridy();
  hr->SaveAs("plots/nitrogen-ratio.C");

}

vector<Double_t> GetN2AndError(TFitResultPtr& f) {
  vector<Double_t> N2(2,0.0);
  if( f->IsEmpty() ) return N2;

  Double_t tau0 = 1453;
  Double_t tau0_err = 10;
  Double_t tau1 = f->Parameter(1);
  Double_t tau1_err = f->ParError(1);
  Double_t kQ = 0.000110;
  Double_t kQ_err = 0.000010;

  N2[0] = (1 / tau1 - 1 / tau0) / kQ;
  N2[1] = Sqrt(Power(kQ_err * (1 / tau1 - 1 / tau0) / kQ / kQ, 2) +
	       Power(tau0_err / kQ / tau0 / tau0, 2) + 
	       Power(tau1_err / kQ / tau1 / tau1, 2) );
  
  return N2;
}

vector<Double_t> GetO2AndError(TFitResultPtr& f) {
  vector<Double_t> O2(2,0.0);
  if( f->IsEmpty() ) return O2;

  Double_t tau0 = 1453;
  Double_t tau0_err = 10;
  Double_t tau1 = f->Parameter(1);
  Double_t tau1_err = f->ParError(1);
  Double_t kQ = 0.000540;
  Double_t kQ_err = 0.000003;

  Double_t kQN2 = 0.000110;
  Double_t kQN2_err = 0.000010;
  Double_t N2 = 4.3;
  Double_t N2_err = 0.5;

  O2[0] = (1 / tau1 - 1 / tau0 - kQN2 * N2) / kQ;
  O2[1] = Sqrt(Power(kQ_err * (1 / tau1 - 1 / tau0) / kQ / kQ, 2) +
               Power(tau0_err / kQ / tau0 / tau0, 2) +
               Power(tau1_err / kQ / tau1 / tau1, 2) + 
	       Power(N2_err * kQN2 / kQ, 2) +
	       Power(kQN2_err * N2 / kQ, 2) );

  return O2;
}

vector<Double_t> GettTAndError(TFitResultPtr& f) {
  vector<Double_t> tT(2,0.0);
  if( f->IsEmpty() ) return tT;

  Double_t tau1 = f->Parameter(1);
  Double_t tau1_err = f->ParError(1);
  
  tT[0] = tau1;
  tT[1] = tau1_err;

  return tT;
}		

vector<Double_t> GetRatioAndError(TFitResultPtr& f) {
  vector<Double_t> r(2,0.0);
  if( f->IsEmpty() ) return r;

  Double_t ratio = f->Parameter(1);
  Double_t ratio_err = f->ParError(1);

  r[0] = ratio;
  r[1] = ratio_err;

  return r;
}
