{
//=========Macro generated from canvas: c1/
//=========  (Mon Jun  6 14:06:10 2016) by ROOT version5.34/34
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
   gre->SetPoint(1,2,981.6855);
   gre->SetPointError(1,0,169.4691);
   gre->SetPoint(2,3,18140.5);
   gre->SetPointError(2,0,873.5663);
   gre->SetPoint(3,4,2072.081);
   gre->SetPointError(3,0,830.4791);
   gre->SetPoint(4,5,1660.522);
   gre->SetPointError(4,0,289.4645);
   gre->SetPoint(5,6,4187.625);
   gre->SetPointError(5,0,727.6248);
   gre->SetPoint(6,7,3869.998);
   gre->SetPointError(6,0,1988.809);
   gre->SetPoint(7,8,2290.571);
   gre->SetPointError(7,0,569.8564);
   gre->SetPoint(8,9,7092.415);
   gre->SetPointError(8,0,3255.73);
   gre->SetPoint(9,10,1665.976);
   gre->SetPointError(9,0,363.4173);
   gre->SetPoint(10,11,1744.109);
   gre->SetPointError(10,0,320.4379);
   gre->SetPoint(11,12,102814.5);
   gre->SetPointError(11,0,2016.167);
   gre->SetPoint(12,13,969.2694);
   gre->SetPointError(12,0,305.3143);
   gre->SetPoint(13,14,59116.01);
   gre->SetPointError(13,0,1626.446);
   gre->SetPoint(14,15,2931.401);
   gre->SetPointError(14,0,753.4799);
   
   TH1F *Graph_Graph1 = new TH1F("Graph_Graph1","",100,0,16.4);
   Graph_Graph1->SetMinimum(0);
   Graph_Graph1->SetMaximum(115313.7);
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
