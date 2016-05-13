{
//=========Macro generated from canvas: c1/c1
//=========  (Thu May 12 15:50:17 2016) by ROOT version5.34/34
   TCanvas *c1 = new TCanvas("c1", "c1",10,32,700,500);
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
   gre->SetTitle("Background rate");
   gre->SetFillColor(1);
   gre->SetMarkerStyle(20);
   gre->SetPoint(0,1,0);
   gre->SetPointError(0,0,0);
   gre->SetPoint(1,2,2981.821);
   gre->SetPointError(1,0,105.0306);
   gre->SetPoint(2,3,2471.295);
   gre->SetPointError(2,0,95.61728);
   gre->SetPoint(3,4,1339.236);
   gre->SetPointError(3,0,70.38862);
   gre->SetPoint(4,5,702.9129);
   gre->SetPointError(4,0,50.99469);
   gre->SetPoint(5,6,5120.178);
   gre->SetPointError(5,0,137.631);
   gre->SetPoint(6,7,2412.104);
   gre->SetPointError(6,0,94.46523);
   gre->SetPoint(7,8,510.5377);
   gre->SetPointError(7,0,43.4598);
   gre->SetPoint(8,9,769.5043);
   gre->SetPointError(8,0,53.35558);
   gre->SetPoint(9,10,4202.651);
   gre->SetPointError(9,0,124.6916);
   gre->SetPoint(10,11,2878.236);
   gre->SetPointError(10,0,103.1901);
   gre->SetPoint(11,12,2042.157);
   gre->SetPointError(11,0,86.9196);
   gre->SetPoint(12,13,1198.652);
   gre->SetPointError(12,0,66.59179);
   gre->SetPoint(13,14,2819.045);
   gre->SetPointError(13,0,102.1235);
   gre->SetPoint(14,15,1642.602);
   gre->SetPointError(14,0,77.95422);
   
   TH1F *Graph_Graph1 = new TH1F("Graph_Graph1","Background rate",100,0,16.4);
   Graph_Graph1->SetMinimum(0);
   Graph_Graph1->SetMaximum(5783.59);
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
   Graph_Graph1->GetYaxis()->SetTitle("rate (Hz)");
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
