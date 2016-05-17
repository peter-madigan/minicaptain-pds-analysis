{
//=========Macro generated from canvas: c1/
//=========  (Mon May 16 12:45:17 2016) by ROOT version5.34/34
   TCanvas *c1 = new TCanvas("c1", "",10,32,700,500);
   gStyle->SetOptStat(0);
   c1->Range(0,0,1,1);
   c1->SetFillColor(0);
   c1->SetBorderMode(0);
   c1->SetBorderSize(2);
   c1->SetLogy();
   c1->SetGridy();
   c1->SetFrameBorderMode(0);
   
   TGraphErrors *gre = new TGraphErrors(15);
   gre->SetName("Graph0");
   gre->SetTitle("");
   gre->SetFillColor(1);
   gre->SetMarkerStyle(20);
   gre->SetPoint(0,1,0);
   gre->SetPointError(0,0,0);
   gre->SetPoint(1,2,5356.146);
   gre->SetPointError(1,0,988.4823);
   gre->SetPoint(2,3,3000.415);
   gre->SetPointError(2,0,501.1742);
   gre->SetPoint(3,4,3003.909);
   gre->SetPointError(3,0,512.9727);
   gre->SetPoint(4,5,2384.768);
   gre->SetPointError(4,0,381.394);
   gre->SetPoint(5,6,8094.901);
   gre->SetPointError(5,0,1109.48);
   gre->SetPoint(6,7,2990.313);
   gre->SetPointError(6,0,470.5714);
   gre->SetPoint(7,8,1792.452);
   gre->SetPointError(7,0,378.2325);
   gre->SetPoint(8,9,3071.96);
   gre->SetPointError(8,0,597.7296);
   gre->SetPoint(9,10,1091.846);
   gre->SetPointError(9,0,167.7118);
   gre->SetPoint(10,11,4246.651);
   gre->SetPointError(10,0,1161.896);
   gre->SetPoint(11,12,1339.951);
   gre->SetPointError(11,0,237.0759);
   gre->SetPoint(12,13,1892.105);
   gre->SetPointError(12,0,318.52);
   gre->SetPoint(13,14,3326.738);
   gre->SetPointError(13,0,564.0589);
   gre->SetPoint(14,15,3550.851);
   gre->SetPointError(14,0,501.8881);
   
   TH1F *Graph_Graph1 = new TH1F("Graph_Graph1","",100,0,16.4);
   Graph_Graph1->SetMinimum(0);
   Graph_Graph1->SetMaximum(10124.82);
   Graph_Graph1->SetDirectory(0);
   Graph_Graph1->SetStats(0);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   Graph_Graph1->SetLineColor(ci);
   Graph_Graph1->GetXaxis()->SetTitle("pmt");
   Graph_Graph1->GetXaxis()->SetLabelFont(42);
   Graph_Graph1->GetXaxis()->SetLabelSize(0.035);
   Graph_Graph1->GetXaxis()->SetTitleSize(0.035);
   Graph_Graph1->GetXaxis()->SetTitleFont(42);
   Graph_Graph1->GetYaxis()->SetTitle("pe rate (Hz)");
   Graph_Graph1->GetYaxis()->SetLabelFont(42);
   Graph_Graph1->GetYaxis()->SetLabelSize(0.035);
   Graph_Graph1->GetYaxis()->SetTitleSize(0.035);
   Graph_Graph1->GetYaxis()->SetTitleFont(42);
   Graph_Graph1->GetZaxis()->SetLabelFont(42);
   Graph_Graph1->GetZaxis()->SetLabelSize(0.035);
   Graph_Graph1->GetZaxis()->SetTitleSize(0.035);
   Graph_Graph1->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_Graph1);
   
   gre->Draw("ap");
   c1->Modified();
   c1->cd();
   c1->SetSelected(c1);
}
