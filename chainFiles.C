void chainFiles() {
  TChain* ch = new TChain("pdsEvTree","pdsEvTree");

  // all
  //for( int i = 166; i <= 297; i++ ) {
  // low intensity
  for( int i = 235; i <= 254; i++ ) {
    ch->Add(Form("data/pdsTree6%03d/pds*",i));
  }
}
