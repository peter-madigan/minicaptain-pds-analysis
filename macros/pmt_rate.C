void pmt_rate(){
  gStyle->SetOptStat(0);

  static const Int_t kNPMTs = 15;
  
  Int_t    pmt_hits[kNPMTs];
  Bool_t   pmt_flag[kNPMTs];
  
  TChain* ch = new TChain("pdsEvTree","pdsEvTree");
  ch->Add("calib/pdsTree9993/pdsEvTree*");
  
  ch->SetBranchStatus("*", kFALSE);

  ch->SetBranchAddress("pmt_hits",pmt_hits);
  ch->SetBranchAddress("pmt_flag",pmt_flag);

  ch->SetBranchStatus("pmt_hits",kTRUE);
  ch->SetBranchStatus("pmt_flag",kTRUE);

  TH1F* h = new TH1F("h",";pmt;rate (Hz)",15,1,16);
  h->Sumw2();

  Double_t n = (Double_t)ch->GetEntries();
  ch->GetEntry(0);
  for( Int_t i = 0; ch->GetEntry(i); i++ ) 
    for( Int_t pmt = 0; pmt < kNPMTs; pmt++ ) 
      if( pmt_flag[pmt] ) {
	if( pmt_hits[pmt] > 200 ) {
	  cout << "Big event!\t" << i << "\t" << pmt_hits[pmt] << "hits!" << endl;
	  continue;
	}
	Double_t part_rate = 1. * pmt_hits[pmt] / n / (250 + 800) / 4e-9;
	h->Fill(pmt+1, part_rate);
      }

  vector<Double_t> x(15,0);
  vector<Double_t> x_err(15,0);
  vector<Double_t> rate(15,0);
  vector<Double_t> rate_err(15,0);

  for( Int_t i = 1; i < 15 + 1; i++ ) {
    x[i-1] = i;
    rate[i-1] = h->GetBinContent(i);
    rate_err[i-1] = h->GetBinError(i);
  }

  TGraphErrors* g = new TGraphErrors(x.size(),&x[0],&rate[0],&x_err[0],&rate_err[0]);
  
  g->GetXaxis()->SetTitle("pmt");
  g->GetYaxis()->SetTitle("rate (Hz)");
  g->SetTitle("Background rate");
  g->SetMarkerStyle(20);
  g->SetMarkerColor(kBlack);
  g->SetLineColor(kBlack);
  
  g->Draw("AP");
  
  c1->SetLogy();
  c1->SetGridy();
  
  c1->SaveAs("plots/pmt_rate.C");
}
