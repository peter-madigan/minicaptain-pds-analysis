
TObjArray* mean(){
  TObjArray* meanWaveform = new TObjArray(); 
  bool firstCall = true;
  int nfiles = 0;
  int nevents = 0;
  
  // low intensity
  for( int i = 235; i <= 254; i++ ) {
    for( int j = 0; j < 5; j++ ) {
      TString fiName = Form("data/pdsTree6%03d/pdsEvTree_%d.root",i,j);
      cout << "Accessing " << fiName << "..." << endl;
      if( gSystem->AccessPathName(fiName) ) continue;
      TFile* fi = new TFile(fiName,"READ");
      cout << "File found!" << endl;
      
      cout << "Looping over pmts..." << endl;
      for( int pmt = 0; pmt < 16; pmt++ ) {
	TString histName;
	if( pmt < 15 )
	  histName = Form("hMeanWaveform_%d",pmt);
	else
	  histName = "hMeanWaveform_sum";
	
	if( firstCall ) {
	  //cout << "Creating " << histName << "..." << endl;
	  meanWaveform->Add(fi->Get(histName)->Clone(histName+"_all"));
	  TH1F* h_curr = (TH1F*)(meanWaveform->At(pmt));
	  h_curr->SetDirectory(0);
	} else {
	  //cout << "Adding " << histName << "..." << endl;
	  TH1F* h_curr = (TH1F*)(fi->Get(histName));
	  TH1F* h_prev = (TH1F*)(meanWaveform->At(pmt));
	  h_prev->Add(h_curr);
	}
      }
      
      nfiles++;
      //nevents += pdsEvTree->GetEntries();
      
      fi->Close();
      firstCall = false;
    }
  }

  cout << "Drawing..." << endl;
  TCanvas* c1 = new TCanvas();
  c1->Divide(4,4);
  for( int pmt = 0; pmt < 16; pmt++ ) {
    TH1F* h_curr = (TH1F*)(meanWaveform->At(pmt));
    h_curr->Scale(1./nfiles);
    h_curr->GetXaxis()->SetRangeUser(600,1400);

    c1->cd(pmt+1);
    h_curr->Draw("l");
  }
  
  TFile* fo = new TFile("MeanWaveforms.root","RECREATE");
  for( int pmt = 0; pmt < 16; pmt++ ) {
    TH1F* h_curr = (TH1F*)(meanWaveform->At(pmt));
    h_curr->Write();
  }
}

