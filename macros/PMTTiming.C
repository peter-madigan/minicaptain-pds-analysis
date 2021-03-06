
void PMTTiming(Int_t runno, Int_t evno){
  gROOT->ProcessLine(".X chainFiles.C");
  gROOT->ProcessLine(".L EventViewer.C");

  ViewEvent(runno, evno);

  TString name = Form("hDt_run%d_ev%d", evno);

  TH2F* h = new TH2F(name,name+";pmt ##;pmt ##;dt (ns)",15,1,16,15,1,16);
  
  Int_t runnum;
  Int_t pds_evno[1000];
  UInt_t nevent;
  Bool_t pmt_noise[1000][15];
  Double_t dt[1000][15][15];

  ch->SetBranchAddress("pds_evno",    pds_evno);
  ch->SetBranchAddress("runno",      &runnum);
  ch->SetBranchAddress("pds_nevent", &nevent);
  ch->SetBranchAddress("pmt_dtime",   dt);
  ch->SetBranchAddress("pmt_noise",   pmt_noise);

  TCanvas* c = new TCanvas();

  for( UInt_t i = 0; i < ch->GetEntries(); i++ ) {
    if( ch->GetEntry(i) != 0 ) {
      for( UInt_t j = 0; j < nevent; j++ ) {
	if( runnum == runno && pds_evno[j] == evno ) {
	  for( Int_t pmt_i = 0; pmt_i < 15; pmt_i++ ) {
	    for( Int_t pmt_j = 0; pmt_j < 15; pmt_j++ ) {
	      if( TMath::Abs(dt[j][pmt_i][pmt_j]) < 100 && !pmt_noise[j][pmt_j] && !pmt_noise[j][pmt_i])
		h->Fill(pmt_i+1, pmt_j+1, dt[j][pmt_i][pmt_j]);
	    }
	  }
	  h->Draw("colz");
	  return;
	}
      }  
    }
  }
}
