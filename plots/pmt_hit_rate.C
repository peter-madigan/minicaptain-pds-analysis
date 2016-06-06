{
//=========Macro generated from canvas: c2/
//=========  (Thu May 19 10:29:30 2016) by ROOT version5.34/34
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
   gre->SetPoint(1,2,9025.445);
   gre->SetPointError(1,0,869.9392);
   gre->SetPoint(2,3,5960.196);
   gre->SetPointError(2,0,507.1144);
   gre->SetPoint(3,4,6258.206);
   gre->SetPointError(3,0,550.6186);
   gre->SetPoint(4,5,3434.202);
   gre->SetPointError(4,0,359.2854);
   gre->SetPoint(5,6,24777.38);
   gre->SetPointError(5,0,1972.844);
   gre->SetPoint(6,7,7592.153);
   gre->SetPointError(6,0,619.4625);
   gre->SetPoint(7,8,3079.429);
   gre->SetPointError(7,0,406.9851);
   gre->SetPoint(8,9,4243.085);
   gre->SetPointError(8,0,566.4826);
   gre->SetPoint(9,10,4654.624);
   gre->SetPointError(9,0,439.9185);
   gre->SetPoint(10,11,9805.947);
   gre->SetPointError(10,0,808.5711);
   gre->SetPoint(11,12,4768.155);
   gre->SetPointError(11,0,398.3581);
   gre->SetPoint(12,13,7847.585);
   gre->SetPointError(12,0,729.0736);
   gre->SetPoint(13,14,11494.67);
   gre->SetPointError(13,0,903.216);
   gre->SetPoint(14,15,8670.674);
   gre->SetPointError(14,0,786.0924);
   
   TH1F *Graph_Graph2 = new TH1F("Graph_Graph2","",100,0,16.4);
   Graph_Graph2->SetMinimum(0);
   Graph_Graph2->SetMaximum(29425.25);
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
