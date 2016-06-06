{
//=========Macro generated from canvas: c1/
//=========  (Thu May 19 10:29:30 2016) by ROOT version5.34/34
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
   gre->SetPoint(1,2,17616.66);
   gre->SetPointError(1,0,1993.591);
   gre->SetPoint(2,3,10155.3);
   gre->SetPointError(2,0,887.1923);
   gre->SetPoint(3,4,9742.186);
   gre->SetPointError(3,0,905.1219);
   gre->SetPoint(4,5,7622.42);
   gre->SetPointError(4,0,817.8249);
   gre->SetPoint(5,6,37801.01);
   gre->SetPointError(5,0,2965.8);
   gre->SetPoint(6,7,12051.47);
   gre->SetPointError(6,0,1005.279);
   gre->SetPoint(7,8,6698.011);
   gre->SetPointError(7,0,959.221);
   gre->SetPoint(8,9,10278.14);
   gre->SetPointError(8,0,1617.08);
   gre->SetPoint(9,10,7287.917);
   gre->SetPointError(9,0,776.8871);
   gre->SetPoint(10,11,17963.05);
   gre->SetPointError(10,0,1663.99);
   gre->SetPoint(11,12,7668.114);
   gre->SetPointError(11,0,722.7452);
   gre->SetPoint(12,13,13229.65);
   gre->SetPointError(12,0,1437.76);
   gre->SetPoint(13,14,19153.93);
   gre->SetPointError(13,0,1555.543);
   gre->SetPoint(14,15,14385.78);
   gre->SetPointError(14,0,1419.297);
   
   TH1F *Graph_Graph1 = new TH1F("Graph_Graph1","",100,0,16.4);
   Graph_Graph1->SetMinimum(0);
   Graph_Graph1->SetMaximum(44843.49);
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
