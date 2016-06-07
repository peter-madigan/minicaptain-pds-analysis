{
//=========Macro generated from canvas: c2/
//=========  (Mon Jun  6 14:06:10 2016) by ROOT version5.34/34
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
   gre->SetPoint(1,2,687.7388);
   gre->SetPointError(1,0,106.7251);
   gre->SetPoint(2,3,9274.658);
   gre->SetPointError(2,0,375.9206);
   gre->SetPoint(3,4,1159.331);
   gre->SetPointError(3,0,392.2562);
   gre->SetPoint(4,5,1120.031);
   gre->SetPointError(4,0,175.202);
   gre->SetPoint(5,6,2593.76);
   gre->SetPointError(5,0,419.1421);
   gre->SetPoint(6,7,1513.025);
   gre->SetPointError(6,0,594.5451);
   gre->SetPoint(7,8,1120.032);
   gre->SetPointError(7,0,246.6016);
   gre->SetPoint(8,9,2652.709);
   gre->SetPointError(8,0,815.0475);
   gre->SetPoint(9,10,982.4838);
   gre->SetPointError(9,0,200.3883);
   gre->SetPoint(10,11,1139.681);
   gre->SetPointError(10,0,184.3304);
   gre->SetPoint(11,12,51677.02);
   gre->SetPointError(11,0,796.2367);
   gre->SetPoint(12,13,609.1401);
   gre->SetPointError(12,0,150.2912);
   gre->SetPoint(13,14,28000.46);
   gre->SetPointError(13,0,575.7391);
   gre->SetPoint(14,15,1748.821);
   gre->SetPointError(14,0,400.0533);
   
   TH1F *Graph_Graph2 = new TH1F("Graph_Graph2","",100,0,16.4);
   Graph_Graph2->SetMinimum(0);
   Graph_Graph2->SetMaximum(57720.59);
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
