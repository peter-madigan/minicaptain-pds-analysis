{
//=========Macro generated from canvas: c1/c1
//=========  (Mon Mar 21 09:42:28 2016) by ROOT version5.34/34
   TCanvas *c1 = new TCanvas("c1", "c1",968,51,700,500);
   gStyle->SetOptStat(0);
   c1->Range(-1.925,-191628.6,17.325,1724657);
   c1->SetFillColor(0);
   c1->SetBorderMode(0);
   c1->SetBorderSize(2);
   c1->SetFrameBorderMode(0);
   c1->SetFrameBorderMode(0);
   
   TH1F *htemp__1 = new TH1F("htemp__1","Noise with 10Hz pulser",100,0,15.4);
   htemp__1->SetBinContent(1,133502.9);
   htemp__1->SetBinContent(7,133502.9);
   htemp__1->SetBinContent(13,19071.84);
   htemp__1->SetBinContent(20,50858.23);
   htemp__1->SetBinContent(26,69930.08);
   htemp__1->SetBinContent(33,89001.92);
   htemp__1->SetBinContent(39,1366811);
   htemp__1->SetBinContent(46,89001.92);
   htemp__1->SetBinContent(52,381436.8);
   htemp__1->SetBinContent(59,44500.95);
   htemp__1->SetBinContent(72,705657.3);
   htemp__1->SetBinContent(78,178003.9);
   htemp__1->SetBinContent(85,31786.4);
   htemp__1->SetBinContent(91,44500.95);
   htemp__1->SetBinError(1,29132.71);
   htemp__1->SetBinError(7,29132.71);
   htemp__1->SetBinError(13,11011.13);
   htemp__1->SetBinError(20,17981.1);
   htemp__1->SetBinError(26,21084.71);
   htemp__1->SetBinError(33,23786.76);
   htemp__1->SetBinError(39,93216.01);
   htemp__1->SetBinError(46,23786.76);
   htemp__1->SetBinError(52,49243.27);
   htemp__1->SetBinError(59,16819.78);
   htemp__1->SetBinError(72,66978.09);
   htemp__1->SetBinError(78,33639.56);
   htemp__1->SetBinError(85,14215.31);
   htemp__1->SetBinError(91,16819.78);
   htemp__1->SetEntries(525);
   htemp__1->SetDirectory(0);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   htemp__1->SetLineColor(ci);
   htemp__1->GetXaxis()->SetTitle("PMT");
   htemp__1->GetXaxis()->SetRange(1,100);
   htemp__1->GetXaxis()->SetLabelFont(42);
   htemp__1->GetXaxis()->SetLabelSize(0.035);
   htemp__1->GetXaxis()->SetTitleSize(0.035);
   htemp__1->GetXaxis()->SetTitleFont(42);
   htemp__1->GetYaxis()->SetTitle("Rate (Hz)");
   htemp__1->GetYaxis()->SetLabelFont(42);
   htemp__1->GetYaxis()->SetLabelSize(0.035);
   htemp__1->GetYaxis()->SetTitleSize(0.035);
   htemp__1->GetYaxis()->SetTitleFont(42);
   htemp__1->GetZaxis()->SetLabelFont(42);
   htemp__1->GetZaxis()->SetLabelSize(0.035);
   htemp__1->GetZaxis()->SetTitleSize(0.035);
   htemp__1->GetZaxis()->SetTitleFont(42);
   htemp__1->Draw("e");
   
   TPaveText *pt = new TPaveText(0.15,0.9319748,0.5439255,0.995,"blNDC");
   pt->SetName("title");
   pt->SetBorderSize(0);
   pt->SetFillColor(0);
   pt->SetFillStyle(0);
   pt->SetTextFont(42);
   TText *text = pt->AddText("Noise with 10Hz pulser");
   pt->Draw();
   c1->Modified();
   c1->cd();
   c1->SetSelected(c1);
}
