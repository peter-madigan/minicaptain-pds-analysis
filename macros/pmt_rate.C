void pmt_rate() {
  TChain* ch = new TChain("pdsEvTree","pdsEvTree");
  ch->Add("calib/pdsTree9993/pdsEvTree*");
  
  ch->Draw("Iteration$%15+1>>h","pmt_flag/Entries$/650/4e-9","e");
  
  h->GetXaxis()->SetTitle("pmt");
  h->GetYaxis()->SetTitle("rate (Hz)");
  h->SetTitle("Background rate");
  
}
