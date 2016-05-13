{
//========= Macro generated from object: Graph/
//========= by ROOT version5.34/34
   
   grae = new TGraphAsymmErrors(23);
   grae->SetName("Graph");
   grae->SetTitle("");
   grae->SetFillColor(1);
   grae->SetMarkerStyle(20);
   grae->SetPoint(0,4.91674e+06,651.876);
   grae->SetPointError(0,9483.75,12687.3,27.0459,27.0459);
   grae->SetPoint(1,4.93506e+06,603.47);
   grae->SetPointError(1,5539.84,5715.16,27.8787,27.8787);
   grae->SetPoint(2,4.95666e+06,648.922);
   grae->SetPointError(2,7817.07,8183.93,28.0037,28.0037);
   grae->SetPoint(3,4.97649e+06,625.781);
   grae->SetPointError(3,11530.2,14983.8,23.1193,23.1193);
   grae->SetPoint(4,5.00119e+06,686.774);
   grae->SetPointError(4,9589.53,9205.47,27.1475,27.1475);
   grae->SetPoint(5,5.01768e+06,561.045);
   grae->SetPointError(5,7214.69,6909.31,25.1013,25.1013);
   grae->SetPoint(6,5.03116e+06,608.515);
   grae->SetPointError(6,6446.76,7753.24,24.8255,24.8255);
   grae->SetPoint(7,5.04456e+06,619.531);
   grae->SetPointError(7,5547.64,4955.36,18.1464,18.1464);
   grae->SetPoint(8,5.0627e+06,628.208);
   grae->SetPointError(8,13087.4,5627.57,14.524,14.524);
   grae->SetPoint(9,5.06985e+06,441.268);
   grae->SetPointError(9,1315.44,3634.56,56.282,56.282);
   grae->SetPoint(10,5.08949e+06,706.509);
   grae->SetPointError(10,6256.37,8036.63,39.5516,39.5516);
   grae->SetPoint(11,5.1032e+06,560.536);
   grae->SetPointError(11,5595.03,5367.97,27.5764,27.5764);
   grae->SetPoint(12,5.11366e+06,584.375);
   grae->SetPointError(12,4970.29,10001.7,42.4106,42.4106);
   grae->SetPoint(13,5.12517e+06,689.86);
   grae->SetPointError(13,423.678,455.322,68.2948,68.2948);
   grae->SetPoint(14,5.527e+06,769.248);
   grae->SetPointError(14,13144.3,13270.7,6.94027,6.94027);
   grae->SetPoint(15,5.72401e+06,775.573);
   grae->SetPointError(15,183455,155367,3.49297,3.49297);
   grae->SetPoint(16,5.91978e+06,767.209);
   grae->SetPointError(16,39701,113706,14.8126,14.8126);
   grae->SetPoint(17,6.03648e+06,777.574);
   grae->SetPointError(17,2655.9,714.096,45.3273,45.3273);
   grae->SetPoint(18,6.20034e+06,669.953);
   grae->SetPointError(18,386.225,501.775,67.7102,67.7102);
   grae->SetPoint(19,6.23174e+06,783.56);
   grae->SetPointError(19,3627.5,17301.5,23.3584,23.3584);
   grae->SetPoint(20,6.28789e+06,921.264);
   grae->SetPointError(20,38666.9,8560.12,35.8053,35.8053);
   grae->SetPoint(21,6.30005e+06,752.819);
   grae->SetPointError(21,3473.28,13928.7,26.7515,26.7515);
   grae->SetPoint(22,1.08735e+07,908.588);
   grae->SetPointError(22,771908,9635.98,20.9172,20.9172);
   
   TH1F *Graph_Graph2 = new TH1F("Graph_Graph2","",100,4.30967e+06,1.14807e+07);
   Graph_Graph2->SetMinimum(327.778);
   Graph_Graph2->SetMaximum(1014.28);
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
