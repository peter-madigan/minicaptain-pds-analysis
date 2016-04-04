void tof_charge() {
  Double_t ymin = 0;
  Double_t ymax = 1e2;
  Double_t xmin = -1e3;
  Double_t xmax = -0;
  Int_t nbinsx = (xmax - xmin);
  Int_t nbinsy = 100;

  TString run_cut = "*1";//" && ( (runno >= 6235 && runno >= 6254)"; // low
  //  run_cut += " || (runno >= 6139 && runno <= 6234)"; // med1
  //  run_cut += " || (runno >= 6260 && runno <= 6299) )"; // med2 

  gROOT -> ProcessLine( ".X chainFiles.C" );
  cout << "#Ev: " << pdsEvTree -> GetEntries() << endl;
  pdsEvTree->Draw("pds_integral:pds_time-rf_time>>h_integral",
		  //pdsEvTree->Draw(Form("pds_integral:pds_time-rf_time>>h_integral(%d,%f,%f,%d,%f,%f)",
		  //nbinsx,xmin,xmax,nbinsy,ymin,ymax*1e2),
		  "pds_flag && 1 && inBeamWindow"+run_cut, "colz");
  c1->DrawClone();
  c1->cd();
  pdsEvTree->Draw("pds_peak:pds_time-rf_time>>h_peak",
		  //pdsEvTree->Draw(Form("pds_peak:pds_time-rf_time>>h_peak(%d,%f,%f,%d,%f,%f)",
		  //nbinsx,xmin,xmax,nbinsy,ymin,ymax),
		  "pds_flag && 1 && inBeamWindow"+run_cut, "colz");
  c1->DrawClone();
  c1->cd()->SetLogy();
  pdsEvTree->Draw("pds_time-rf_time>>h",
		  //pdsEvTree->Draw(Form("pds_time-rf_time>>h(%d,%f,%f)", nbinsx,xmin,xmax),
		  "pds_flag && 1 && inBeamWindow"+run_cut,
		  "goff");
  h->Draw();
}
