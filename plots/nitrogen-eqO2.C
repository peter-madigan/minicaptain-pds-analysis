{
//========= Macro generated from object: Graph/
//========= by ROOT version5.34/34
   
   grae = new TGraphAsymmErrors(23);
   grae->SetName("Graph");
   grae->SetTitle("");
   grae->SetFillColor(1);
   grae->SetMarkerStyle(20);
   grae->SetPoint(0,4.91674e+06,0.87371);
   grae->SetPointError(0,9483.75,12687.3,0.168471,0.168471);
   grae->SetPoint(1,4.93506e+06,1.10158);
   grae->SetPointError(1,5539.84,5715.16,0.186042,0.186042);
   grae->SetPoint(2,4.95666e+06,0.886639);
   grae->SetPointError(2,7817.07,8183.93,0.172215,0.172215);
   grae->SetPoint(3,4.97649e+06,0.992171);
   grae->SetPointError(3,11530.2,14983.8,0.162652,0.162652);
   grae->SetPoint(4,5.00119e+06,0.729357);
   grae->SetPointError(4,9589.53,9205.47,0.160744,0.160744);
   grae->SetPoint(5,5.01768e+06,1.33363);
   grae->SetPointError(5,7214.69,6909.31,0.190656,0.190656);
   grae->SetPoint(6,5.03116e+06,1.07614);
   grae->SetPointError(6,6446.76,7753.24,0.172991,0.172991);
   grae->SetPoint(7,5.04456e+06,1.02202);
   grae->SetPointError(7,5547.64,4955.36,0.1489,0.1489);
   grae->SetPoint(8,5.0627e+06,0.980738);
   grae->SetPointError(8,13087.4,5627.57,0.13837,0.13837);
   grae->SetPoint(9,5.06985e+06,2.22956);
   grae->SetPointError(9,1315.44,3634.56,0.548808,0.548808);
   grae->SetPoint(10,5.08949e+06,0.654034);
   grae->SetPointError(10,6256.37,8036.63,0.189743,0.189743);
   grae->SetPoint(11,5.1032e+06,1.33662);
   grae->SetPointError(11,5595.03,5367.97,0.202382,0.202382);
   grae->SetPoint(12,5.11366e+06,1.20185);
   grae->SetPointError(12,4970.29,10001.7,0.259651,0.259651);
   grae->SetPoint(13,5.12517e+06,0.717291);
   grae->SetPointError(13,423.678,455.322,0.291717,0.291717);
   grae->SetPoint(14,5.527e+06,0.440258);
   grae->SetPointError(14,13144.3,13270.7,0.122174,0.122174);
   grae->SetPoint(15,5.72401e+06,0.420625);
   grae->SetPointError(15,183455,155367,0.120702,0.120702);
   grae->SetPoint(16,5.91978e+06,0.446655);
   grae->SetPointError(16,39701,113706,0.128945,0.128945);
   grae->SetPoint(17,6.03648e+06,0.414481);
   grae->SetPointError(17,2655.9,714.096,0.183648,0.183648);
   grae->SetPoint(18,6.20034e+06,0.797055);
   grae->SetPointError(18,386.225,501.775,0.304184,0.304184);
   grae->SetPoint(19,6.23174e+06,0.396288);
   grae->SetPointError(19,3627.5,17301.5,0.139339,0.139339);
   grae->SetPoint(20,6.28789e+06,0.0430262);
   grae->SetPointError(20,38666.9,8560.12,0.143301,0.143301);
   grae->SetPoint(21,6.30005e+06,0.492795);
   grae->SetPointError(21,3473.28,13928.7,0.148659,0.148659);
   grae->SetPoint(22,1.08735e+07,0.0710699);
   grae->SetPointError(22,771908,9635.98,0.128976,0.128976);
   
   TH1F *Graph_Graph3 = new TH1F("Graph_Graph3","",100,4.30967e+06,1.14807e+07);
   Graph_Graph3->SetMinimum(-0.38814);
   Graph_Graph3->SetMaximum(3.06624);
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
