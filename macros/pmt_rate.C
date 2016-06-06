void pmt_rate(){
  gStyle->SetOptStat(0);

  static const Int_t kNPMTs = 15;
  static const Int_t kMaxNHits = 200;

  Int_t    pmt_hits[kNPMTs];
  Double_t pmt_peak[kNPMTs][kMaxNHits];
  Bool_t   pmt_flag[kNPMTs];
  
  TChain* ch = new TChain("pdsEvTree","pdsEvTree");
  ch->Add("calib/pdsTree90007/pdsEvTree*");
  //ch->Add("calib/pdsTree90005/pdsEvTree*");
  //ch->Add("calib/pdsTree9993/pdsEvTree*");
  //ch->Add("calib/pdsTree9988/pdsEvTree*");
  //ch->Add("calib/pdsTree9987/pdsEvTree*");
  //ch->Add("calib/pdsTree9986/pdsEvTree*");

  ch->SetBranchStatus("*", kFALSE);

  ch->SetBranchAddress("pmt_hits",pmt_hits);
  ch->SetBranchAddress("pmt_peak",pmt_peak);
  ch->SetBranchAddress("pmt_flag",pmt_flag);

  ch->SetBranchStatus("pmt_hits",kTRUE);
  ch->SetBranchStatus("pmt_peak",kTRUE);
  ch->SetBranchStatus("pmt_flag",kTRUE);

  TH1F* h_pe =  new TH1F("h_pe",";pmt;pe rate (pe/s)",15,1,16);
  TH1F* h_hit = new TH1F("h_hit",";pmt;hit rate (1/s)",15,1,16);
  h_pe->Sumw2();
  h_hit->Sumw2();

  Double_t n = (Double_t)ch->GetEntries();
  ch->GetEntry(0);
  for( Int_t i = 0; ch->GetEntry(i); i++ ) 
    for( Int_t pmt = 0; pmt < kNPMTs; pmt++ ) 
      if( pmt_flag[pmt] ) {
	Double_t rate_hit = 0.25e9 * pmt_hits[pmt] / n / (250 + 800);
	h_hit->Fill(pmt+1, rate_hit);

	Double_t pe = 0;
	for( Int_t hit = 0; hit < pmt_hits[pmt]; hit++ )
	  pe += pmt_peak[pmt][hit];
	Double_t rate_pe = pe / n / (250 + 800) / 4e-9;
	h_pe->Fill(pmt+1, rate_pe);
      }

  vector<Double_t> x(15,0);
  vector<Double_t> x_err(15,0);
  vector<Double_t> pe_rate(15,0);
  vector<Double_t> pe_rate_err(15,0);
  vector<Double_t> hit_rate(15,0);
  vector<Double_t> hit_rate_err(15,0);

  for( Int_t i = 1; i < 15 + 1; i++ ) {
    x[i-1] = i;
    pe_rate[i-1] = h_pe->GetBinContent(i);
    pe_rate_err[i-1] = h_pe->GetBinError(i);
    hit_rate[i-1] = h_hit->GetBinContent(i);
    hit_rate_err[i-1] = h_hit->GetBinError(i);
  }

  TGraphErrors* g_pe = new TGraphErrors(x.size(),&x[0],&pe_rate[0],&x_err[0],&pe_rate_err[0]);
  TGraphErrors* g_hit = new TGraphErrors(x.size(),&x[0],&hit_rate[0],&x_err[0],&hit_rate_err[0]);

  TCanvas* c1 = new TCanvas("c1","");
  g_pe->GetXaxis()->SetTitle("pmt");
  g_pe->GetYaxis()->SetTitle("pe rate (Hz)");
  g_pe->SetTitle("");
  g_pe->SetMarkerStyle(20);
  g_pe->SetMarkerColor(kBlack);
  g_pe->SetLineColor(kBlack);
  
  g_pe->Draw("AP");
  
  c1->SetLogy();
  c1->SetGridy();
  
  c1->SaveAs("plots/pmt_pe_rate.C");

  TCanvas* c2 =new TCanvas("c2","");
  g_hit->GetXaxis()->SetTitle("pmt");
  g_hit->GetYaxis()->SetTitle("hit rate (Hz)");
  g_hit->SetTitle("");
  g_hit->SetMarkerStyle(20);
  g_hit->SetMarkerColor(kBlack);
  g_hit->SetLineColor(kBlack);

  g_hit->Draw("AP");

  c2->SetLogy();
  c2->SetGridy();

  c2->SaveAs("plots/pmt_hit_rate.C");
}
