{
//=========Macro generated from canvas: c1/c1
//=========  (Mon Mar 21 08:09:51 2016) by ROOT version5.34/34
   TCanvas *c1 = new TCanvas("c1", "c1",202,30,700,500);
   gStyle->SetOptStat(0);
   c1->Range(-1.925,-3.212179,17.325,28.90961);
   c1->SetFillColor(0);
   c1->SetBorderMode(0);
   c1->SetBorderSize(2);
   c1->SetFrameBorderMode(0);
   c1->SetFrameBorderMode(0);
   
   TH1F *htemp__1 = new TH1F("htemp__1","Noise rate for PDS trigger at 1kHz",100,0,15.4);
   htemp__1->SetBinContent(1,2.697841);
   htemp__1->SetBinContent(7,2.323142);
   htemp__1->SetBinContent(13,3.072541);
   htemp__1->SetBinContent(20,3.597121);
   htemp__1->SetBinContent(26,2.173262);
   htemp__1->SetBinContent(33,5.395685);
   htemp__1->SetBinContent(39,23.15642);
   htemp__1->SetBinContent(46,5.395685);
   htemp__1->SetBinContent(52,15.9622);
   htemp__1->SetBinContent(59,3.147481);
   htemp__1->SetBinContent(65,0.2248201);
   htemp__1->SetBinContent(72,17.23618);
   htemp__1->SetBinContent(78,8.767989);
   htemp__1->SetBinContent(85,4.571342);
   htemp__1->SetBinContent(91,3.522181);
   htemp__1->SetBinError(1,0.4496403);
   htemp__1->SetBinError(7,0.4172485);
   htemp__1->SetBinError(13,0.4798504);
   htemp__1->SetBinError(20,0.5191999);
   htemp__1->SetBinError(26,0.4035645);
   htemp__1->SetBinError(33,0.6358874);
   htemp__1->SetBinError(39,1.317326);
   htemp__1->SetBinError(46,0.6358874);
   htemp__1->SetBinError(52,1.093714);
   htemp__1->SetBinError(59,0.485667);
   htemp__1->SetBinError(65,0.1298);
   htemp__1->SetBinError(72,1.136522);
   htemp__1->SetBinError(78,0.8106006);
   htemp__1->SetBinError(85,0.5853005);
   htemp__1->SetBinError(91,0.5137631);
   htemp__1->SetEntries(1351);
   htemp__1->SetDirectory(0);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   htemp__1->SetLineColor(ci);
   htemp__1->GetXaxis()->SetTitle("PMT #");
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
   
   TPaveText *pt = new TPaveText(0.1604871,0.9317089,0.7306304,0.995,"blNDC");
   pt->SetName("title");
   pt->SetBorderSize(0);
   pt->SetFillColor(0);
   pt->SetFillStyle(0);
   pt->SetTextFont(42);
   TText *text = pt->AddText("Noise rate for PDS trigger at 1kHz");
   pt->Draw();
   c1->Modified();
   c1->cd();
   c1->SetSelected(c1);
}
