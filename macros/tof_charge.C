void tof_charge() {
  Double_t ymin = 0;
  Double_t ymax = 1e2;
  Double_t xmin = -1.7e3;
  Double_t xmax = 100;
  Int_t nbinsx = (xmax - xmin)/2;
  Int_t nbinsy = 150;

  TString run_cut = " && ( 0"; // low
  run_cut += " || (runno >= 6235 && runno >= 6254)"; // low
  //  run_cut += " || (runno >= 6139 && runno <= 6234)"; // med1
  //  run_cut += " || (runno >= 6260 && runno <= 6299)"; // med2 
  run_cut += " )";

  TString int_cut = " && !(pds_integral < 75)";
  TString time_cut = "&& 1"; //" && !(abs(pds_time - 3100) < 300)";

  gROOT -> ProcessLine( ".X chainFiles.C" );
  cout << "#Ev: " << pdsEvTree -> GetEntries() << endl;
  //pdsEvTree->Draw("pds_integral:pds_time-rf_time>>h_integral",
  pdsEvTree->Draw(Form("pds_integral:pds_time-rf_time>>h_integral(%d,%f,%f,%d,%f,%f)",
		       nbinsx,xmin,xmax,nbinsy,ymin,ymax*1e1),
		  "pds_flag && inBeamWindow"+run_cut+int_cut+time_cut, "colz");
  c1->DrawClone();
  c1->cd();
  //pdsEvTree->Draw("pds_peak:pds_time-rf_time>>h_peak",
  pdsEvTree->Draw(Form("pds_peak:pds_time-rf_time>>h_peak(%d,%f,%f,%d,%f,%f)",
		       nbinsx,xmin,xmax,nbinsy,ymin,ymax),
		  "pds_flag && inBeamWindow"+run_cut+int_cut+time_cut, "colz");
  c1->DrawClone();
  c1->cd()->SetLogy();
  //pdsEvTree->Draw("pds_time-rf_time>>h",
  pdsEvTree->Draw(Form("pds_time-rf_time>>h(%d,%f,%f)", nbinsx,xmin,xmax),
		  "pds_flag && inBeamWindow"+run_cut+int_cut+time_cut,
		  "goff");
  h->Draw();

  c1->DrawClone();
  c1->cd()->SetLogy(0);
  //pdsEvTree->Draw("pds_peak:pds_integral>>h_shape", 
  pdsEvTree->Draw(Form("pds_peak:pds_integral>>h_shape(%d,%f,%f,%d,%f,%f)",
                       250,20.,2500.,250,2.,100.),
                  "pds_flag && inBeamWindow"+run_cut+time_cut, "colz");
  c1->cd()->SetLogz();
  
  c1->DrawClone();
  pdsEvTree->Draw(Form("pds_peak:pds_integral>>h_shapeg(%d,%f,%f,%d,%f,%f)",
                       250,20.,2500.,250,2.,100.),
                  "(pds_time-rf_time < -680 && pds_time-rf_time > -760) && pds_flag && inBeamWindow"+run_cut+int_cut+time_cut, "colz");

  c1->DrawClone();
  pdsEvTree->Draw(Form("pds_peak:pds_integral>>h_shapen(%d,%f,%f,%d,%f,%f)",
                       250,20.,2500.,250,2.,100.),
                  "(pds_time-rf_time > -680 && pds_time-rf_time < -500) && pds_flag && inBeamWindo\
w"+run_cut+int_cut+time_cut, "colz");
  

  c1->DrawClone();
  pdsEvTree->Draw(Form("pds_peak:pds_integral>>h_shaped(%d,%f,%f,%d,%f,%f)",
                       250,20.,2500.,250,2.,100.),
                  "(pds_time-rf_time < -120 && pds_time-rf_time > -180) && pds_flag && inBeamWindow\
"+run_cut+int_cut+time_cut, "colz");
}
