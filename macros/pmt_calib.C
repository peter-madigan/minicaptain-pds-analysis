void pmt_calib() {
  gStyle->SetOptStat(1110);
  gStyle->SetOptFit(1);
  gStyle->SetStatX(1.0);
  gStyle->SetStatY(1.0);
  gStyle->SetStatW(0.2);
  gStyle->SetStatH(0.2);

  TCanvas* c_peak = new TCanvas("c_peak","PMT calib (peak)",1200,800);
  c_peak->Divide(4,4);
  TCanvas* c_bl = new TCanvas("c_bl","PMT calib (baseline)",1200,800);
  c_bl->Divide(4,4);
  TCanvas* c_noise = new TCanvas("c_noise","PMT calib (integral)");
  
  TChain* ch = new TChain("pdsEvTree","pdsEvTree");
  ch->Add("calib/pdsTree9990/pdsEvTree*");
  ch->Add("calib/pdsTree9991/pdsEvTree*");
  ch->Add("calib/pdsTree9992/pdsEvTree*");
  ch->Add("calib/pdsTree9994/pdsEvTree*");
  ch->Add("calib/pdsTree9995/pdsEvTree*");
  ch->Add("calib/pdsTree9996/pdsEvTree*");

  TH1F* h_all = new TH1F("hAll",";total integral;count",1500,-1000,500);
  TH1F* h_noise = new TH1F("hNoise",";total integral;count",1500,-1000,500);
  TH1F* h_notNoise = new TH1F("hNotNoise",";total integral;count",1000,-500,500);
  ch->Draw("pmt_integral>>hAll","","e goff");
  ch->Draw("pmt_integral>>hNoise","!pmt_flag","e goff");
  ch->Draw("pmt_integral>>hNotNoise","pmt_flag","e goff");
  c_noise -> cd();
  h_all->SetLineColor(kBlack);
  h_noise->SetLineColor(kBlue+2);
  h_notNoise->SetLineColor(kRed+2);
  h_noise->Draw("e");
  h_notNoise->Draw("e same");
  h_all->Draw("e same");


  TF1* gaus = new TF1("gaussian","gaus",0,20);
  for( int pmt = 0; pmt < 15; pmt++ ) {
    TString name_peak = Form("hPeak_%d",pmt+1);
    TString name_bl = Form("hBL_%d",pmt+1);

    TH1F* h_peak = new TH1F(name_peak,name_peak+";height;count",15,-15,0);
    TH1F* h_bl = new TH1F(name_bl,name_bl+";baseline;count",40,3880,3920);
    ch->Draw("pmt_peak>>"+name_peak,Form("Iteration$%%15==%d && pmt_flag",pmt),"e goff");
    ch->Draw("pmt_offset>>"+name_bl,Form("Iteration$%%15==%d",pmt),"e goff");

    // peak fit
    Double_t low_edge = -10;
    Double_t high_edge = -5;
    gaus -> SetRange(low_edge, high_edge);
    gaus -> SetParLimits(1, -20, 0);

    c_peak->cd(1+pmt)->SetLogy();
    std::cout << "vvv\t" << "Fit results for " << pmt << "\tvvv" << std::endl;
    h_peak->Fit("gaussian","r");
    h_peak->Draw("e");
    //    c_peak->Update();

    // baseline fit                                                               
    Double_t low_edge =  3880;
    Double_t high_edge = 3920;
    gaus -> SetRange(low_edge, high_edge);
    gaus -> SetParLimits(1, 3880, 3920);

    c_bl->cd(1+pmt)->SetLogy();
    h_bl->Fit("gaussian","r");
    h_bl->Draw("e");
    //    c_bl->Update();
  }

  // Occupancy
  Bool_t pmt_flag[1000][15];
  UInt_t  nevent;
  ch->SetBranchAddress("pmt_flag", pmt_flag);
  ch->SetBranchAddress("pds_nevent", &nevent);

  TH1F* h_occu = new TH1F("h_occu","occupancy;# of pmts;counts",15,0,15);
  for( int i = 0; i < ch->GetEntries(); i++ ) {
    ch->GetEntry(i);

    for( int j = 0; j < nevent; j++ ) {
      Int_t sum = 0;
      for( int pmt = 0; pmt < 15; pmt++ ) {
	sum += pmt_flag[j][pmt];
      }
      h_occu -> Fill(sum);
    }
  }
  c_peak -> cd(16);
  h_occu -> Draw("e");
  c_peak -> Update();

  // PDS baseline
  TH1F* hBL_PDS = new TH1F("hBL_PDS","hBL_PDS;baseline;count",500,3900*15-250,3900*15+250);
  ch->Draw("pds_offset>>hBL_PDS","","e goff");
  c_bl -> cd(16);
  hBL_PDS -> Draw("e");
  c_bl -> Update();

  c_peak -> SaveAs("plots/pmt_calib-peak.C");
  c_bl   -> SaveAs("plots/pmt_calib-baseline.C");
  c_noise-> SaveAs("plots/pmt_calib-noise.C");

  std::cout << "Total triggers: " << ch->GetEntries() << std::endl;

}
