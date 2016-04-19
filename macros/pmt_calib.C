static const Int_t kNPMTs = 15;
static const Int_t kMaxNHits = 200;

void pmt_calib() {
  gStyle->SetOptStat(1110);
  gStyle->SetOptFit(1);
  gStyle->SetStatX(1.0);
  gStyle->SetStatY(1.0);
  gStyle->SetStatW(0.15);
  gStyle->SetStatH(0.3);

  TCanvas* c_int = new TCanvas("c_int","PMT calib (integral)",1200,800);
  c_int->Divide(4,4);
  TCanvas* c_peak = new TCanvas("c_peak","PMT calib (peak)",1200,800);
  c_peak->Divide(4,4);
  TCanvas* c_bl = new TCanvas("c_bl","PMT calib (baseline)",1200,800);
  c_bl->Divide(4,4);
  TCanvas* c_noise = new TCanvas("c_noise","PMT calib (integral)");
  
  TChain* ch = new TChain("pdsEvTree","pdsEvTree");
  //ch->Add("calib/pdsTree9990/pdsEvTree*");
  ch->Add("calib/pdsTree9991/pdsEvTree*");
  ch->Add("calib/pdsTree9992/pdsEvTree*");
  ch->Add("calib/pdsTree9994/pdsEvTree*");
  ch->Add("calib/pdsTree9995/pdsEvTree*");
  ch->Add("calib/pdsTree9996/pdsEvTree*");

  Int_t    pmt_hits[kNPMTs];
  Double_t pmt_peak[kNPMTs][kMaxNHits];
  Double_t pmt_integral[kNPMTs][kMaxNHits];
  Double_t pmt_time[kNPMTs][kMaxNHits];
  Double_t pmt_ratio[kNPMTs];
  Bool_t   pmt_flag[kNPMTs];
  Double_t pmt_offset[kNPMTs];
  ch->SetBranchStatus("pmt_dtime",kFALSE);
  ch->SetBranchAddress("pmt_hits",pmt_hits);
  ch->SetBranchAddress("pmt_peak",pmt_peak);
  ch->SetBranchAddress("pmt_integral",pmt_integral);
  ch->SetBranchAddress("pmt_time",pmt_time);
  ch->SetBranchAddress("pmt_ratio",pmt_ratio);
  ch->SetBranchAddress("pmt_flag",pmt_flag);
  ch->SetBranchAddress("pmt_offset",pmt_offset);

  TH1F* h_all      = new TH1F("hAll",";ratio;count",250,-1,1);
  TH1F* h_noise    = new TH1F("hNoise",";ratio;count",250,-1,1);
  TH1F* h_notNoise = new TH1F("hNotNoise",";ratio;count",250,-1,1);
  TH1F* h_occu     = new TH1F("h_occu","occupancy;# of pmts;counts",15,0,15);
  TObjArray* h_int  = new TObjArray();
  TObjArray* h_peak = new TObjArray();
  TObjArray* h_bl   = new TObjArray();

  h_all->SetLineColor(kGreen+2);
  h_noise->SetLineColor(kBlue+2);
  h_notNoise->SetLineColor(kRed+2);
 
  for( Int_t pmt = 0; pmt < kNPMTs; pmt++ ) {
    TString name_int = Form("PMT%d_integral",pmt+1);
    h_int->Add(new TH1F(name_int,name_int+";integral;count",200,-50,0));
    ((TH1F*)h_int->At(pmt))->Sumw2();

    TString name_peak = Form("PMT%d_height",pmt+1);
    h_peak->Add(new TH1F(name_peak,name_peak+";height;count",20,-20,0));
    ((TH1F*)h_peak->At(pmt))->Sumw2();

    TString name_bl = Form("PMT%d_baseline",pmt+1);
    h_bl->Add(new TH1F(name_bl,name_bl+";baseline;count",100,3880,3920));
    ((TH1F*)h_bl->At(pmt))->Sumw2();
  }
  
  TF1* gaus = new TF1("gaussian","gaus",-1e3,1e3);
  ch->GetEntry(0);
  for( Int_t i = 0; ch->GetEntry(i); i++ ) {
    if( i%(ch->GetEntries()/10) == 0 ) cout << i << " of " << ch->GetEntriesFast() << endl;
    
    Int_t occu = 0;
    for( Int_t pmt = 0; pmt < kNPMTs; pmt++ ) {
      h_all->Fill(pmt_ratio[pmt]);
      if( pmt_flag[pmt] ) h_notNoise->Fill(pmt_ratio[pmt]);
      else                h_noise->Fill(pmt_ratio[pmt]);
      
      if( pmt_flag[pmt] ) {
	for( Int_t hit = 0; hit < pmt_hits[pmt]; hit++ ) {
	  if( TMath::Abs(pmt_time[pmt][hit] - 900*4) < 200 ) {
	    ((TH1F*)h_peak->At(pmt))->Fill(pmt_peak[pmt][hit]);
	    ((TH1F*)h_int->At(pmt))->Fill(pmt_integral[pmt][hit]);
	    occu++;
	  }
	}
      }
      
      ((TH1F*)h_bl->At(pmt))->Fill(pmt_offset[pmt]);
    }
    h_occu->Fill(occu);
  }
  
  for( Int_t pmt = 0; pmt < kNPMTs; pmt++ ) {
    // peak fit
    Double_t low_edge = -11;
    Double_t high_edge = -5;
    gaus -> SetRange(low_edge, high_edge);

    c_peak->cd(1+pmt)->SetLogy();
    std::cout << std::endl;
    std::cout << "vvv\t" << "Fit results for " << pmt+1 << "\tvvv" << std::endl;
    std::cout << "~~~\t" << "Height #" << pmt+1 << "\t~~~" << std::endl;
    ((TH1F*)h_peak->At(pmt))->Fit("gaussian","r");
    ((TH1F*)h_peak->At(pmt))->Draw("e");

    Double_t mean = 0;
    Double_t n = 0;
    for( Double_t ADC = low_edge; ADC < high_edge+1; ADC++ ) {
      TH1F* h = ((TH1F*)h_peak->At(pmt));
      mean += ADC * h->GetBinContent(h->FindBin(ADC));
      n += h->GetBinContent(h->FindBin(ADC));
    }
    mean = mean/n;
    std::cout << "Mean across (" << low_edge << ", " << high_edge << "):\n"
	      << mean << std::endl;

    // integral fit
    std::cout << "~~~\t" << "Integral #" << pmt+1 << "\t~~~" << std::endl;
    low_edge =  -50;
    high_edge = -10;
    gaus -> SetRange(low_edge, high_edge);

    c_int->cd(1+pmt)->SetLogy();
    ((TH1F*)h_int->At(pmt))->Fit("gaussian","r");
    ((TH1F*)h_int->At(pmt))->Draw("e");

    // baseline fit                
    std::cout << "~~~\t" << "Baseline #" << pmt+1 << "\t~~~" << std::endl;
    low_edge =  3880;
    high_edge = 3920;
    gaus -> SetRange(low_edge, high_edge);
        
    c_bl->cd(1+pmt)->SetLogy();
    ((TH1F*)h_bl->At(pmt))->Fit("gaussian","r");
    ((TH1F*)h_bl->At(pmt))->Draw("e");
  }

  // Occupancy
  c_peak -> cd(16);
  h_occu -> Draw("e");

  // Noise
  c_noise -> cd() -> SetLogy();
  h_all -> Draw("ep");
  h_noise -> Draw("ep same");
  h_notNoise -> Draw("ep same");

  c_int -> Update();
  c_noise -> Update();
  c_peak -> Update();
  c_bl -> Update();

  c_int  -> SaveAs("plots/pmt_calib-integral.C");
  c_peak -> SaveAs("plots/pmt_calib-peak.C");
  c_bl   -> SaveAs("plots/pmt_calib-baseline.C");
  c_noise-> SaveAs("plots/pmt_calib-noise.C");

  std::cout << "Total triggers: " << ch->GetEntries() << std::endl;

}
