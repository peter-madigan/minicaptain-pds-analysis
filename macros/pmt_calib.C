static const Int_t kNPMTs = 15;
static const Int_t kMaxNHits = 200;

void pmt_calib() {
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1);
  gStyle->SetStatX(0.4);
  gStyle->SetStatY(1.0);
  gStyle->SetStatW(0.2);
  gStyle->SetStatH(0.3);

  TCanvas* c_int = new TCanvas("c_int","PMT calib (integral)",1200,800);
  c_int->Divide(4,4);
  TCanvas* c_peak = new TCanvas("c_peak","PMT calib (peak)",1200,800);
  c_peak->Divide(4,4);
  TCanvas* c_bl = new TCanvas("c_bl","PMT calib (baseline)",1200,800);
  c_bl->Divide(4,4);
  TCanvas* c_noise = new TCanvas("c_noise","PMT calib (integral)");
  
  TChain* ch = new TChain("pdsEvTree","pdsEvTree");
  ch->Add("calib/pdsTree9989/pdsEvTree*");
  //ch->Add("calib/pdsTree9990/pdsEvTree*");
  //ch->Add("calib/pdsTree9991/pdsEvTree*");
  //ch->Add("calib/pdsTree9992/pdsEvTree*");
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
    h_int->Add(new TH1F(name_int,name_int+";integral;count",75,-75,0));
    ((TH1F*)h_int->At(pmt))->Sumw2();

    TString name_peak = Form("PMT%d_height",pmt+1);
    h_peak->Add(new TH1F(name_peak,name_peak+";height;count",20,-20,0));
    ((TH1F*)h_peak->At(pmt))->Sumw2();

    TString name_bl = Form("PMT%d_baseline",pmt+1);
    h_bl->Add(new TH1F(name_bl,name_bl+";baseline;count",100,3880,3920));
    ((TH1F*)h_bl->At(pmt))->Sumw2();
  }
  
  TF1* fit  = new TF1("fit","gaus(0) + gaus(3)",-1e2,0);
  ch->GetEntry(0);
  for( Int_t i = 0; ch->GetEntry(i); i++ ) {
    if( i%(ch->GetEntries()/10) == 0 ) cout << i << " of " << ch->GetEntriesFast() << endl;
    
    Double_t occu = 0;
    for( Int_t pmt = 0; pmt < kNPMTs; pmt++ ) {
      h_all->Fill(pmt_peak[pmt][0]);
      if( pmt_flag[pmt] ) h_notNoise->Fill(pmt_peak[pmt][0]);
      else                h_noise->Fill(pmt_peak[pmt][0]);
      
      if( pmt_flag  && pmt_hits[pmt] > 0 )
      for( Int_t hit = 0; hit < pmt_hits[pmt]; hit++ ) {
	((TH1F*)h_peak->At(pmt))->Fill(pmt_peak[pmt][hit]);
	((TH1F*)h_int->At(pmt))->Fill(pmt_integral[pmt][hit]);
	occu += 1;
      }
            
      ((TH1F*)h_bl->At(pmt))->Fill(pmt_offset[pmt]);
    }
    h_occu->Fill(occu);
  }
  
  for( Int_t pmt = 0; pmt < kNPMTs; pmt++ ) {
    // peak fit
    Double_t norm = 1./((TH1F*)h_peak->At(pmt))->Integral();
    ((TH1F*)h_peak->At(pmt))->Scale(norm);
    Double_t low_edge = -12;
    Double_t high_edge = 0;
    Double_t max = ((TH1F*)h_peak->At(pmt))->GetMaximum();
    Double_t pe_peak = ((TH1F*)h_peak->At(pmt))->GetBinContent(((TH1F*)h_peak->At(pmt))->FindBin(-8));

    fit->SetRange(low_edge, high_edge);
    fit->SetParameters(max,-2,0.5,pe_peak,-8,3);
    fit->SetParLimits(0,0,2);
    fit->SetParLimits(1,-4,0);
    fit->SetParLimits(2,low_edge,high_edge);
    fit->SetParLimits(3,0,2);
    fit->SetParLimits(4,low_edge,high_edge);
    fit->SetParLimits(5,low_edge,high_edge);

    c_peak->cd(1+pmt)->SetLogy();
    std::cout << std::endl;
    std::cout << "vvv\t" << "Fit results for " << pmt+1 << "\tvvv" << std::endl;
    std::cout << "~~~\t" << "Height #" << pmt+1 << "\t~~~" << std::endl;
    ((TH1F*)h_peak->At(pmt))->Fit("fit","r");
    ((TH1F*)h_peak->At(pmt))->Draw("e");
    c_peak->Update();
    
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
    Double_t norm = 1./((TH1F*)h_int->At(pmt))->Integral();
    ((TH1F*)h_int->At(pmt))->Scale(norm);
    low_edge = -50;
    high_edge =  0;
    fit -> SetRange(low_edge, high_edge);
    fit->SetParameters(1,0,1,0.01,-20,1);
    fit->SetParLimits(0,0,2);
    fit->SetParLimits(1,-15,0);
    fit->SetParLimits(2,low_edge,high_edge);
    fit->SetParLimits(3,0,2);
    fit->SetParLimits(4,low_edge,high_edge);
    fit->SetParLimits(5,low_edge,high_edge);

    c_int->cd(1+pmt)->SetLogy();
    ((TH1F*)h_int->At(pmt))->Fit("fit","r");
    ((TH1F*)h_int->At(pmt))->Draw("e");
    c_int->Update();

    // baseline fit
    /*                
    std::cout << "~~~\t" << "Baseline #" << pmt+1 << "\t~~~" << std::endl;
    low_edge =  3880;
    high_edge = 3920;
    gaus -> SetRange(low_edge, high_edge);
    gaus -> SetRange(low_edge, high_edge);
    gaus->SetParameters(1e3,0,5,1e2,-7,5);
    gaus->SetParLimits(0,0,1e10);
    gaus->SetParLimits(1,low_edge,high_edge);
    gaus->SetParLimits(3,0,1e10);
    gaus->SetParLimits(4,low_edge,high_edge);
    */  
    c_bl->cd(1+pmt)->SetLogy();
    //((TH1F*)h_bl->At(pmt))->Fit("gaussian","r");
    //((TH1F*)h_bl->At(pmt))->Draw("e");
  }

  // Occupancy
  c_peak -> cd(16);
  //h_occu -> Draw("e");

  // Noise
  //c_noise -> cd() -> SetLogy();
  //h_all -> Draw("ep");
  //h_noise -> Draw("ep same");
  //h_notNoise -> Draw("ep same");

  //c_int -> Update();
  //c_noise -> Update();
  //c_peak -> Update();
  //c_bl -> Update();

  c_int  -> SaveAs("plots/pmt_calib-integral.C");
  c_peak -> SaveAs("plots/pmt_calib-peak.C");
  c_bl   -> SaveAs("plots/pmt_calib-baseline.C");
  c_noise-> SaveAs("plots/pmt_calib-noise.C");

  std::cout << "Total triggers: " << ch->GetEntries() << std::endl;

}
