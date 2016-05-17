{
//=========Macro generated from canvas: c2/
//=========  (Mon May 16 12:45:17 2016) by ROOT version5.34/34
   TCanvas *c2 = new TCanvas("c2", "",10,32,700,500);
   gStyle->SetOptStat(0);
   c2->Range(0,0,1,1);
   c2->SetFillColor(0);
   c2->SetBorderMode(0);
   c2->SetBorderSize(2);
   c2->SetLogy();
   c2->SetGridy();
   c2->SetFrameBorderMode(0);
   
   TGraphErrors *gre = new TGraphErrors(15);
   gre->SetName("Graph0");
   gre->SetTitle("");
   gre->SetFillColor(1);
   gre->SetMarkerStyle(20);
   gre->SetPoint(0,1,0);
   gre->SetPointError(0,0,0);
   gre->SetPoint(1,2,2886.066);
   gre->SetPointError(1,0,437.9084);
   gre->SetPoint(2,3,2094.221);
   gre->SetPointError(2,0,344.6945);
   gre->SetPoint(3,4,2250.506);
   gre->SetPointError(3,0,389.8437);
   gre->SetPoint(4,5,1469.081);
   gre->SetPointError(4,0,201.8977);
   gre->SetPoint(5,6,5199.082);
   gre->SetPointError(5,0,675.1889);
   gre->SetPoint(6,7,1906.678);
   gre->SetPointError(6,0,252.755);
   gre->SetPoint(7,8,1125.253);
   gre->SetPointError(7,0,197.9612);
   gre->SetPoint(8,9,1979.61);
   gre->SetPointError(8,0,317.5663);
   gre->SetPoint(9,10,771.0073);
   gre->SetPointError(9,0,117.8777);
   gre->SetPoint(10,11,2094.22);
   gre->SetPointError(10,0,365.6316);
   gre->SetPoint(11,12,823.1021);
   gre->SetPointError(11,0,137.2386);
   gre->SetPoint(12,13,1344.052);
   gre->SetPointError(12,0,213.9071);
   gre->SetPoint(13,14,2021.286);
   gre->SetPointError(13,0,310.829);
   gre->SetPoint(14,15,2333.857);
   gre->SetPointError(14,0,305.3677);
   
   TH1F *Graph_Graph2 = new TH1F("Graph_Graph2","",100,0,16.4);
   Graph_Graph2->SetMinimum(0);
   Graph_Graph2->SetMaximum(6461.697);
   Graph_Graph2->SetDirectory(0);
   Graph_Graph2->SetStats(0);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   Graph_Graph2->SetLineColor(ci);
   Graph_Graph2->GetXaxis()->SetTitle("pmt");
   Graph_Graph2->GetXaxis()->SetLabelFont(42);
   Graph_Graph2->GetXaxis()->SetLabelSize(0.035);
   Graph_Graph2->GetXaxis()->SetTitleSize(0.035);
   Graph_Graph2->GetXaxis()->SetTitleFont(42);
   Graph_Graph2->GetYaxis()->SetTitle("hit rate (Hz)");
   Graph_Graph2->GetYaxis()->SetLabelFont(42);
   Graph_Graph2->GetYaxis()->SetLabelSize(0.035);
   Graph_Graph2->GetYaxis()->SetTitleSize(0.035);
   Graph_Graph2->GetYaxis()->SetTitleFont(42);
   Graph_Graph2->GetZaxis()->SetLabelFont(42);
   Graph_Graph2->GetZaxis()->SetLabelSize(0.035);
   Graph_Graph2->GetZaxis()->SetTitleSize(0.035);
   Graph_Graph2->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_Graph2);
   
   gre->Draw("ap");
   c2->Modified();
   c2->cd();
   c2->SetSelected(c2);
}
