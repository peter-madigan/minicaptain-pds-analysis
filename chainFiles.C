void chainFiles() {
  TChain* ch = new TChain("pdsEvTree","pdsEvTree");

  // all
  for( int i = 132; i <= 297; i++ ) {
    if( // exclude runs: 
       i == 287 || i == 279 || i == 273 || i == 201 || i == 202 || 
       (i <= 175 && i >= 147) ||(i <= 187 && i >= 181 ) || (i <= 219 && i >= 218) ||// exclude no elog
       (i >= 260) || // exclude 2nd med intensity
       (i <= 259 && i >= 255) || // exclude high intensity
       //(i <= 254 && i >= 235) || // exclude low intensity
       (i <= 234 && i >= 139) || // exclude 1st med intensity
       (i <= 138 && i >= 132) || // exclude bias/HV off
       false )
      continue;
    ch->Add(Form("data/pdsTree6%03d/pds*",i));
  }
}
