{
//========= Macro generated from object: Graph/
//========= by ROOT version5.34/34
   
   TGraphAsymmErrors *grae = new TGraphAsymmErrors(23);
   grae->SetName("Graph");
   grae->SetTitle("");
   grae->SetFillColor(1);
   grae->SetMarkerStyle(20);
   grae->SetPoint(0,4.91674e+06,7.68912);
   grae->SetPointError(0,9483.75,12687.3,0.908433,0.908433);
   grae->SetPoint(1,4.93506e+06,8.80774);
   grae->SetPointError(1,5539.84,5715.16,1.06175,1.06175);
   grae->SetPoint(2,4.95666e+06,7.75259);
   grae->SetPointError(2,7817.07,8183.93,0.929549,0.929549);
   grae->SetPoint(3,4.97649e+06,8.27066);
   grae->SetPointError(3,11530.2,14983.8,0.924786,0.924786);
   grae->SetPoint(4,5.00119e+06,6.98048);
   grae->SetPointError(4,9589.53,9205.47,0.823619,0.823619);
   grae->SetPoint(5,5.01768e+06,9.94689);
   grae->SetPointError(5,7214.69,6909.31,1.15978,1.15978);
   grae->SetPoint(6,5.03116e+06,8.68286);
   grae->SetPointError(6,6446.76,7753.24,0.998199,0.998199);
   grae->SetPoint(7,5.04456e+06,8.41721);
   grae->SetPointError(7,5547.64,4955.36,0.878704,0.878704);
   grae->SetPoint(8,5.0627e+06,8.21453);
   grae->SetPointError(8,13087.4,5627.57,0.81943,0.81943);
   grae->SetPoint(9,5.06985e+06,14.3451);
   grae->SetPointError(9,1315.44,3634.56,2.93381,2.93381);
   grae->SetPoint(10,5.08949e+06,6.61071);
   grae->SetPointError(10,6256.37,8036.63,0.9391,0.9391);
   grae->SetPoint(11,5.1032e+06,9.9616);
   grae->SetPointError(11,5595.03,5367.97,1.20772,1.20772);
   grae->SetPoint(12,5.11366e+06,9.29998);
   grae->SetPointError(12,4970.29,10001.7,1.41114,1.41114);
   grae->SetPoint(13,5.12517e+06,6.92125);
   grae->SetPointError(13,423.678,455.322,1.44903,1.44903);
   grae->SetPoint(14,5.527e+06,5.56127);
   grae->SetPointError(14,13144.3,13270.7,0.518482,0.518482);
   grae->SetPoint(15,5.72401e+06,5.46488);
   grae->SetPointError(15,183455,155367,0.501457,0.501457);
   grae->SetPoint(16,5.91978e+06,5.59267);
   grae->SetPointError(16,39701,113706,0.559186,0.559186);
   grae->SetPoint(17,6.03648e+06,5.43473);
   grae->SetPointError(17,2655.9,714.096,0.842874,0.842874);
   grae->SetPoint(18,6.20034e+06,7.31282);
   grae->SetPointError(18,386.225,501.775,1.52467,1.52467);
   grae->SetPoint(19,6.23174e+06,5.34541);
   grae->SetPointError(19,3627.5,17301.5,0.598014,0.598014);
   grae->SetPoint(20,6.28789e+06,3.61122);
   grae->SetPointError(20,38666.9,8560.12,0.506673,0.506673);
   grae->SetPoint(21,6.30005e+06,5.81918);
   grae->SetPointError(21,3473.28,13928.7,0.682534,0.682534);
   grae->SetPoint(22,1.08735e+07,3.74889);
   grae->SetPointError(22,771908,9635.98,0.413597,0.413597);
   
   TH1F *Graph_Graph1 = new TH1F("Graph_Graph1","",100,4.30967e+06,1.14807e+07);
   Graph_Graph1->SetMinimum(1.68711);
   Graph_Graph1->SetMaximum(18.6964);
   Graph_Graph1->SetDirectory(0);
   Graph_Graph1->SetStats(0);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000099");
   Graph_Graph1->SetLineColor(ci);
   Graph_Graph1->GetXaxis()->SetTimeDisplay(1);
   Graph_Graph1->GetXaxis()->SetTimeFormat("%m/%d/%y%F1995-01-01 00:00:00s0");
   Graph_Graph1->GetXaxis()->SetNdivisions(205);
   Graph_Graph1->GetXaxis()->SetLabelFont(42);
   Graph_Graph1->GetXaxis()->SetLabelSize(0.035);
   Graph_Graph1->GetXaxis()->SetTitleSize(0.035);
   Graph_Graph1->GetXaxis()->SetTitleFont(42);
   Graph_Graph1->GetYaxis()->SetTitle("Eq. [N_{2}] (ppm)");
   Graph_Graph1->GetYaxis()->SetLabelFont(42);
   Graph_Graph1->GetYaxis()->SetLabelSize(0.035);
   Graph_Graph1->GetYaxis()->SetTitleSize(0.035);
   Graph_Graph1->GetYaxis()->SetTitleFont(42);
   Graph_Graph1->GetZaxis()->SetLabelFont(42);
   Graph_Graph1->GetZaxis()->SetLabelSize(0.035);
   Graph_Graph1->GetZaxis()->SetTitleSize(0.035);
   Graph_Graph1->GetZaxis()->SetTitleFont(42);
   grae->SetHistogram(Graph_Graph1);
   
   grae->Draw("");
}
