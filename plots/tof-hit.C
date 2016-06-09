{
//========= Macro generated from object: h_tof/
//========= by ROOT version5.34/34
   
   TH1F *h_tof = new TH1F("h_tof","",3200,0,3200);
   h_tof->SetBinContent(1,6);
   h_tof->SetBinContent(2,2);
   h_tof->SetBinContent(3,1);
   h_tof->SetBinContent(10,1);
   h_tof->SetBinContent(104,1);
   h_tof->SetBinContent(155,1);
   h_tof->SetBinContent(302,1);
   h_tof->SetBinContent(310,1);
   h_tof->SetBinContent(354,1);
   h_tof->SetBinContent(355,1);
   h_tof->SetBinContent(377,1);
   h_tof->SetBinContent(534,1);
   h_tof->SetBinContent(625,2);
   h_tof->SetBinContent(663,1);
   h_tof->SetBinContent(673,1);
   h_tof->SetBinContent(720,1);
   h_tof->SetBinContent(734,1);
   h_tof->SetBinContent(785,1);
   h_tof->SetBinContent(807,1);
   h_tof->SetBinContent(961,1);
   h_tof->SetBinContent(1005,1);
   h_tof->SetBinContent(1106,1);
   h_tof->SetBinContent(1141,1);
   h_tof->SetBinContent(1181,1);
   h_tof->SetBinContent(1456,1);
   h_tof->SetBinContent(1630,1);
   h_tof->SetBinContent(2046,1);
   h_tof->SetBinContent(2221,1);
   h_tof->SetBinContent(3119,1);
   h_tof->SetEntries(36);

   ci = TColor::GetColor("#000099");
   h_tof->SetLineColor(ci);
   h_tof->GetXaxis()->SetTitle("dt (ns)");
   h_tof->GetXaxis()->SetLabelFont(42);
   h_tof->GetXaxis()->SetLabelSize(0.035);
   h_tof->GetXaxis()->SetTitleSize(0.035);
   h_tof->GetXaxis()->SetTitleFont(42);
   h_tof->GetYaxis()->SetTitle("hits");
   h_tof->GetYaxis()->SetLabelFont(42);
   h_tof->GetYaxis()->SetLabelSize(0.035);
   h_tof->GetYaxis()->SetTitleSize(0.035);
   h_tof->GetYaxis()->SetTitleFont(42);
   h_tof->GetZaxis()->SetLabelFont(42);
   h_tof->GetZaxis()->SetLabelSize(0.035);
   h_tof->GetZaxis()->SetTitleSize(0.035);
   h_tof->GetZaxis()->SetTitleFont(42);
   h_tof->Draw("");
}
