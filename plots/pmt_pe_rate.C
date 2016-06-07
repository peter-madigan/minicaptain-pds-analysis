{
//=========Macro generated from canvas: c1/
//=========  (Tue Jun  7 08:19:19 2016) by ROOT version5.34/34
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
   gre->SetPoint(1,2,344.7134);
   gre->SetPointError(1,0,128.3059);
   gre->SetPoint(2,3,6354.665);
   gre->SetPointError(2,0,717.2041);
   gre->SetPoint(3,4,1450.094);
   gre->SetPointError(3,0,820.0676);
   gre->SetPoint(4,5,728.5796);
   gre->SetPointError(4,0,250.7273);
   gre->SetPoint(5,6,2506.791);
   gre->SetPointError(5,0,693.3725);
   gre->SetPoint(6,7,3476.277);
   gre->SetPointError(6,0,1987.553);
   gre->SetPoint(7,8,1829.895);
   gre->SetPointError(7,0,561.9755);
   gre->SetPoint(8,9,6355.675);
   gre->SetPointError(8,0,3253.676);
   gre->SetPoint(9,10,1251.983);
   gre->SetPointError(9,0,354.5688);
   gre->SetPoint(10,11,594.6019);
   gre->SetPointError(10,0,258.1951);
   gre->SetPoint(11,12,28631.86);
   gre->SetPointError(11,0,1508.625);
   gre->SetPoint(12,13,644.9628);
   gre->SetPointError(12,0,299.0179);
   gre->SetPoint(13,14,21042.71);
   gre->SetPointError(13,0,1344.024);
   gre->SetPoint(14,15,2246.468);
   gre->SetPointError(14,0,746.8917);
   
   TH1F *Graph_Graph1 = new TH1F("Graph_Graph1","",100,0,16.4);
   Graph_Graph1->SetMinimum(0);
   Graph_Graph1->SetMaximum(33154.53);
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
