void chainFiles() {
  TChain* ch = new TChain("ch","pdsEvTree");

  for( int i = 0; i < 300; i++ ) {
    ch->Add(Form("data/pdsTree6%03d/pds*",i));
  }
}
