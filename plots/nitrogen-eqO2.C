{
//========= Macro generated from object: Graph/
//========= by ROOT version5.34/34
   
   grae = new TGraphAsymmErrors(7);
   grae->SetName("Graph");
   grae->SetTitle("");
   grae->SetFillColor(1);
   grae->SetMarkerStyle(20);
   grae->SetPoint(0,1.25177e+07,0.016597);
   grae->SetPointError(0,0.109857,-0.109857,0.127799,0.127799);
   grae->SetPoint(1,1.26448e+07,0.00272377);
   grae->SetPointError(1,-0.197087,0.197087,0.126463,0.126463);
   grae->SetPoint(2,1.2769e+07,-0.186569);
   grae->SetPointError(2,0.370767,-0.370767,0.126522,0.126522);
   grae->SetPoint(3,1.27393e+07,-0.0642489);
   grae->SetPointError(3,-0.455377,0.455377,0.124447,0.124447);
   grae->SetPoint(4,1.27875e+07,0.00995186);
   grae->SetPointError(4,-0.286652,0.286652,0.135173,0.135173);
   grae->SetPoint(5,1.28096e+07,-0.136871);
   grae->SetPointError(5,-0.211554,0.211554,0.125363,0.125363);
   grae->SetPoint(6,1.28281e+07,-0.0385036);
   grae->SetPointError(6,0.0596037,-0.0596037,0.124562,0.124562);
   
   TH1F *Graph_Graph3 = new TH1F("Graph_Graph3","",100,1.24866e+07,1.28591e+07);
   Graph_Graph3->SetMinimum(-0.358913);
   Graph_Graph3->SetMaximum(0.190946);
   Graph_Graph3->SetDirectory(0);
   Graph_Graph3->SetStats(0);

   ci = TColor::GetColor("#000099");
   Graph_Graph3->SetLineColor(ci);
   Graph_Graph3->GetXaxis()->SetTimeDisplay(1);
   Graph_Graph3->GetXaxis()->SetTimeFormat("%m/%d/%y%F1995-01-01 00:00:00s0");
   Graph_Graph3->GetXaxis()->SetNdivisions(205);
   Graph_Graph3->GetXaxis()->SetLabelFont(42);
   Graph_Graph3->GetXaxis()->SetLabelSize(0.035);
   Graph_Graph3->GetXaxis()->SetTitleSize(0.035);
   Graph_Graph3->GetXaxis()->SetTitleFont(42);
   Graph_Graph3->GetYaxis()->SetTitle("Eq. [O_{2}] (ppm)");
   Graph_Graph3->GetYaxis()->SetLabelFont(42);
   Graph_Graph3->GetYaxis()->SetLabelSize(0.035);
   Graph_Graph3->GetYaxis()->SetTitleSize(0.035);
   Graph_Graph3->GetYaxis()->SetTitleFont(42);
   Graph_Graph3->GetZaxis()->SetLabelFont(42);
   Graph_Graph3->GetZaxis()->SetLabelSize(0.035);
   Graph_Graph3->GetZaxis()->SetTitleSize(0.035);
   Graph_Graph3->GetZaxis()->SetTitleFont(42);
   grae->SetHistogram(Graph_Graph3);
   
   grae->Draw("");
}
