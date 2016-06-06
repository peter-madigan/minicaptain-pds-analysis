{
//========= Macro generated from object: Graph/
//========= by ROOT version5.34/34
   
   TGraphAsymmErrors *grae = new TGraphAsymmErrors(7);
   grae->SetName("Graph");
   grae->SetTitle("");
   grae->SetFillColor(1);
   grae->SetMarkerStyle(20);
   grae->SetPoint(0,1.25177e+07,3.48148);
   grae->SetPointError(0,0.109857,-0.109857,0.384528,0.384528);
   grae->SetPoint(1,1.26448e+07,3.41337);
   grae->SetPointError(1,-0.197087,0.197087,0.36852,0.36852);
   grae->SetPoint(2,1.2769e+07,2.48412);
   grae->SetPointError(2,0.370767,-0.370767,0.301743,0.301743);
   grae->SetPoint(3,1.27393e+07,3.0846);
   grae->SetPointError(3,-0.455377,0.455377,0.325625,0.325625);
   grae->SetPoint(4,1.27875e+07,3.44885);
   grae->SetPointError(4,-0.286652,0.286652,0.43901,0.43901);
   grae->SetPoint(5,1.28096e+07,2.72809);
   grae->SetPointError(5,-0.211554,0.211554,0.307379,0.307379);
   grae->SetPoint(6,1.28281e+07,3.21098);
   grae->SetPointError(6,0.0596037,-0.0596037,0.336558,0.336558);
   
   TH1F *Graph_Graph1 = new TH1F("Graph_Graph1","",100,1.24866e+07,1.28591e+07);
   Graph_Graph1->SetMinimum(2.01182);
   Graph_Graph1->SetMaximum(4.05841);
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
