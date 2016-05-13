{
//========= Macro generated from object: Graph/
//========= by ROOT version5.34/34
   
   grae = new TGraphAsymmErrors(23);
   grae->SetName("Graph");
   grae->SetTitle("");
   grae->SetFillColor(1);
   grae->SetMarkerStyle(20);
   grae->SetPoint(0,4.91674e+06,-0.668303);
   grae->SetPointError(0,9483.75,12687.3,0.338855,0.338855);
   grae->SetPoint(1,4.93506e+06,-0.548705);
   grae->SetPointError(1,5539.84,5715.16,0.355733,0.355733);
   grae->SetPoint(2,4.95666e+06,-0.841762);
   grae->SetPointError(2,7817.07,8183.93,0.362158,0.362158);
   grae->SetPoint(3,4.97649e+06,-0.204149);
   grae->SetPointError(3,11530.2,14983.8,0.149724,0.149724);
   grae->SetPoint(4,5.00119e+06,-0.0985534);
   grae->SetPointError(4,9589.53,9205.47,0.167315,0.167315);
   grae->SetPoint(5,5.01768e+06,-0.268596);
   grae->SetPointError(5,7214.69,6909.31,0.242124,0.242124);
   grae->SetPoint(6,5.03116e+06,-0.676429);
   grae->SetPointError(6,6446.76,7753.24,0.322152,0.322152);
   grae->SetPoint(7,5.04456e+06,0.33614);
   grae->SetPointError(7,5547.64,4955.36,0.0790911,0.0790911);
   grae->SetPoint(8,5.0627e+06,0.302366);
   grae->SetPointError(8,13087.4,5627.57,0.0595428,0.0595428);
   grae->SetPoint(9,5.06985e+06,-0.00359539);
   grae->SetPointError(9,1315.44,3634.56,0.800604,0.800604);
   grae->SetPoint(10,5.08949e+06,0.066619);
   grae->SetPointError(10,6256.37,8036.63,0.17353,0.17353);
   grae->SetPoint(11,5.1032e+06,0.0176024);
   grae->SetPointError(11,5595.03,5367.97,0.179162,0.179162);
   grae->SetPoint(12,5.11366e+06,0.190417);
   grae->SetPointError(12,4970.29,10001.7,0.209062,0.209062);
   grae->SetPoint(13,5.12517e+06,-0.463214);
   grae->SetPointError(13,423.678,455.322,2.37364,2.37364);
   grae->SetPoint(14,5.527e+06,0.345671);
   grae->SetPointError(14,13144.3,13270.7,0.0216654,0.0216654);
   grae->SetPoint(15,5.72401e+06,0.25377);
   grae->SetPointError(15,183455,155367,0.0136161,0.0136161);
   grae->SetPoint(16,5.91978e+06,0.0123939);
   grae->SetPointError(16,39701,113706,0.0805949,0.0805949);
   grae->SetPoint(17,6.03648e+06,0.366013);
   grae->SetPointError(17,2655.9,714.096,0.13671,0.13671);
   grae->SetPoint(18,6.20034e+06,0.0291849);
   grae->SetPointError(18,386.225,501.775,0.451384,0.451384);
   grae->SetPoint(19,6.23174e+06,0.158909);
   grae->SetPointError(19,3627.5,17301.5,0.0854651,0.0854651);
   grae->SetPoint(20,6.28789e+06,0.00958027);
   grae->SetPointError(20,38666.9,8560.12,0.124352,0.124352);
   grae->SetPoint(21,6.30005e+06,0.146867);
   grae->SetPointError(21,3473.28,13928.7,0.119484,0.119484);
   grae->SetPoint(22,1.08735e+07,1.18698);
   grae->SetPointError(22,771908,9635.98,0.0113289,0.0113289);
   
   TH1F *Graph_Graph4 = new TH1F("Graph_Graph4","",100,4.30967e+06,1.14807e+07);
   Graph_Graph4->SetMinimum(-3.31158);
   Graph_Graph4->SetMaximum(2.38515);
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
