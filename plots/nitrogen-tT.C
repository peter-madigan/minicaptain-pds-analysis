{
//========= Macro generated from object: Graph/
//========= by ROOT version5.34/34
   
   grae = new TGraphAsymmErrors(7);
   grae->SetName("Graph");
   grae->SetTitle("");
   grae->SetFillColor(1);
   grae->SetMarkerStyle(20);
   grae->SetPoint(0,1.25177e+07,933.538);
   grae->SetPointError(0,0.109857,-0.109857,20.524,20.524);
   grae->SetPoint(1,1.26448e+07,940.113);
   grae->SetPointError(1,-0.197087,0.197087,18.8672,18.8672);
   grae->SetPoint(2,1.2769e+07,1040.06);
   grae->SetPointError(2,0.370767,-0.370767,23.2551,23.2551);
   grae->SetPoint(3,1.27393e+07,973.201);
   grae->SetPointError(3,-0.455377,0.455377,16.6507,16.6507);
   grae->SetPoint(4,1.27875e+07,936.676);
   grae->SetPointError(4,-0.286652,0.286652,29.364,29.364);
   grae->SetPoint(5,1.28096e+07,1011.82);
   grae->SetPointError(5,-0.211554,0.211554,19.8661,19.8661);
   grae->SetPoint(6,1.28281e+07,960.21);
   grae->SetPointError(6,0.0596037,-0.0596037,16.4186,16.4186);
   
   TH1F *Graph_Graph2 = new TH1F("Graph_Graph2","",100,1.24866e+07,1.28591e+07);
   Graph_Graph2->SetMinimum(891.712);
   Graph_Graph2->SetMaximum(1078.91);
   Graph_Graph2->SetDirectory(0);
   Graph_Graph2->SetStats(0);

   ci = TColor::GetColor("#000099");
   Graph_Graph2->SetLineColor(ci);
   Graph_Graph2->GetXaxis()->SetTimeDisplay(1);
   Graph_Graph2->GetXaxis()->SetTimeFormat("%m/%d/%y%F1995-01-01 00:00:00s0");
   Graph_Graph2->GetXaxis()->SetNdivisions(205);
   Graph_Graph2->GetXaxis()->SetLabelFont(42);
   Graph_Graph2->GetXaxis()->SetLabelSize(0.035);
   Graph_Graph2->GetXaxis()->SetTitleSize(0.035);
   Graph_Graph2->GetXaxis()->SetTitleFont(42);
   Graph_Graph2->GetYaxis()->SetTitle("#tau_{T} (ns)");
   Graph_Graph2->GetYaxis()->SetLabelFont(42);
   Graph_Graph2->GetYaxis()->SetLabelSize(0.035);
   Graph_Graph2->GetYaxis()->SetTitleSize(0.035);
   Graph_Graph2->GetYaxis()->SetTitleFont(42);
   Graph_Graph2->GetZaxis()->SetLabelFont(42);
   Graph_Graph2->GetZaxis()->SetLabelSize(0.035);
   Graph_Graph2->GetZaxis()->SetTitleSize(0.035);
   Graph_Graph2->GetZaxis()->SetTitleFont(42);
   grae->SetHistogram(Graph_Graph2);
   
   grae->Draw("");
}
