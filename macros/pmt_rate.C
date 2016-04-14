void pmt_rate() {
  TChain* ch = new TChain("pdsEvTree","pdsEvTree");
  ch->Add("calib/pdsTree9993/pdsEvTree*");
  
  ch->Draw("TMath::Floor(Iteration$)+1>>h","(pmt_flag * pmt_hits)/Entries$/(250+800)/4e-9","e");
  
  h->GetXaxis()->SetTitle("pmt");
  h->GetYaxis()->SetTitle("rate (Hz)");
  h->SetTitle("Background rate");  

  c1->SetLogy();
  c1->SetGridy();
}
