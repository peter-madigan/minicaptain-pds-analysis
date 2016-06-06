{
//========= Macro generated from object: Graph/
//========= by ROOT version5.34/34
   
   grae = new TGraphAsymmErrors(7);
   grae->SetName("Graph");
   grae->SetTitle("");
   grae->SetFillColor(1);
   grae->SetMarkerStyle(20);
   grae->SetPoint(0,1.25177e+07,1.2419);
   grae->SetPointError(0,0.109857,-0.109857,0.0118783,0.0118783);
   grae->SetPoint(1,1.26448e+07,1.2804);
   grae->SetPointError(1,-0.197087,0.197087,0.0100828,0.0100828);
   grae->SetPoint(2,1.2769e+07,1.22102);
   grae->SetPointError(2,0.370767,-0.370767,0.00997396,0.00997396);
   grae->SetPoint(3,1.27393e+07,1.14012);
   grae->SetPointError(3,-0.455377,0.455377,0.00897494,0.00897494);
   grae->SetPoint(4,1.27875e+07,1.2392);
   grae->SetPointError(4,-0.286652,0.286652,0.0129082,0.0129082);
   grae->SetPoint(5,1.28096e+07,1.24234);
   grae->SetPointError(5,-0.211554,0.211554,0.00866903,0.00866903);
   grae->SetPoint(6,1.28281e+07,1.14112);
   grae->SetPointError(6,0.0596037,-0.0596037,0.00987692,0.00987692);
   
   TH1F *Graph_Graph4 = new TH1F("Graph_Graph4","",100,1.24866e+07,1.28591e+07);
   Graph_Graph4->SetMinimum(1.11521);
   Graph_Graph4->SetMaximum(1.30642);
   Graph_Graph4->SetDirectory(0);
   Graph_Graph4->SetStats(0);

   ci = TColor::GetColor("#000099");
   Graph_Graph4->SetLineColor(ci);
   Graph_Graph4->GetXaxis()->SetTimeDisplay(1);
   Graph_Graph4->GetXaxis()->SetTimeFormat("%m/%d/%y%F1995-01-01 00:00:00s0");
   Graph_Graph4->GetXaxis()->SetNdivisions(205);
   Graph_Graph4->GetXaxis()->SetLabelFont(42);
   Graph_Graph4->GetXaxis()->SetLabelSize(0.035);
   Graph_Graph4->GetXaxis()->SetTitleSize(0.035);
   Graph_Graph4->GetXaxis()->SetTitleFont(42);
   Graph_Graph4->GetYaxis()->SetTitle("Triplet/singlet ratio");
   Graph_Graph4->GetYaxis()->SetLabelFont(42);
   Graph_Graph4->GetYaxis()->SetLabelSize(0.035);
   Graph_Graph4->GetYaxis()->SetTitleSize(0.035);
   Graph_Graph4->GetYaxis()->SetTitleFont(42);
   Graph_Graph4->GetZaxis()->SetLabelFont(42);
   Graph_Graph4->GetZaxis()->SetLabelSize(0.035);
   Graph_Graph4->GetZaxis()->SetTitleSize(0.035);
   Graph_Graph4->GetZaxis()->SetTitleFont(42);
   grae->SetHistogram(Graph_Graph4);
   
   grae->Draw("");
}
