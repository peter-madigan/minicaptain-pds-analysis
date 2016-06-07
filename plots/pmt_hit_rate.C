{
//=========Macro generated from canvas: c2/
//=========  (Tue Jun  7 08:19:19 2016) by ROOT version5.34/34
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
   gre->SetPoint(1,2,196.4969);
   gre->SetPointError(1,0,70.84795);
   gre->SetPoint(2,3,1709.521);
   gre->SetPointError(2,0,221.0044);
   gre->SetPoint(3,4,687.739);
   gre->SetPointError(3,0,380.2608);
   gre->SetPoint(4,5,412.6434);
   gre->SetPointError(4,0,136.8442);
   gre->SetPoint(5,6,1336.178);
   gre->SetPointError(5,0,388.5474);
   gre->SetPoint(6,7,1198.63);
   gre->SetPointError(6,0,591.9417);
   gre->SetPoint(7,8,785.9872);
   gre->SetPointError(7,0,237.4281);
   gre->SetPoint(8,9,2063.217);
   gre->SetPointError(8,0,810.0581);
   gre->SetPoint(9,10,668.0893);
   gre->SetPointError(9,0,191.5214);
   gre->SetPoint(10,11,353.6943);
   gre->SetPointError(10,0,137.5478);
   gre->SetPoint(11,12,5835.958);
   gre->SetPointError(11,0,289.7912);
   gre->SetPoint(12,13,334.0446);
   gre->SetPointError(12,0,141.013);
   gre->SetPoint(13,14,4263.992);
   gre->SetPointError(13,0,263.9941);
   gre->SetPoint(14,15,1198.631);
   gre->SetPointError(14,0,392.2562);
   
   TH1F *Graph_Graph2 = new TH1F("Graph_Graph2","",100,0,16.4);
   Graph_Graph2->SetMinimum(0);
   Graph_Graph2->SetMaximum(6738.324);
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
