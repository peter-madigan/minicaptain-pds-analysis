// Adapted from macro PDSAnalyze.C to be a compiled program

#include <sstream>
#include <iostream>
#include <unistd.h>
#include <vector>

#include <Rtypes.h>
#include <TTree.h>
#include <TString.h>
#include <TH1.h>
#include <TCanvas.h>

class PDSAnalysis 
{
 public:
  static const size_t maxTime = 2048; 
  static const size_t nBoards = 3;
  static const size_t nPMTs   = 6;
  static const size_t totPMTs  = 15;
  static const Int_t trigger;
  static const Double_t sampleRate;
  static const Double_t rfThreshold;
  static const Double_t pmtSumThreshold;
  static const Double_t pmtSingleThreshold;
  static const Int_t beamWindow;
  static const Int_t triggerWindow;
  static const Double_t beamPulseWidth;
  static const Double_t tpcGateWidth;
  static const Double_t ADC_to_pe;
  static const Double_t tick_to_ns;

  Double_t tpcTriggerTime;

  TTree* pdsTree;
  // pmt tree variables
  UShort_t waveforms[nBoards][8][maxTime];
  UInt_t   number;
  UInt_t   nSamples;
  struct pmt_time {
    UShort_t gps_y, gps_d;
    UInt_t   gps_s, gps_ns;
    Int_t comp_s; 
    Long64_t comp_ns;
    UShort_t gps_flag;
    UInt_t digit_time[nBoards];
  } time;

  // analysis variables
  UInt_t   runno;
  Int_t    evno_tpc;
  UInt_t   nevent;
  Int_t    evno_pds[1000];
  Double_t time_interp[1000];
  UShort_t gps_yr, gps_d;
  UInt_t   gps_s,  gps_ns;
  Double_t peak[1000], integral[1000];
  Double_t peak_time[1000];
  Double_t pmt_peak[1000][totPMTs];
  Double_t pmt_integral[1000][totPMTs];
  Double_t pmt_peak_time[1000][totPMTs];
  Double_t pmt_time_interp[1000][totPMTs];
  Double_t pmt_dtime[1000][totPMTs][totPMTs];
  Double_t rf_time[1000];
  Bool_t   inBeamWindow[1000];
  Bool_t   isBeamTrigger[1000];
  Double_t pds_time[1000];
  Double_t timeWt[1000];

  TCanvas* c;

  // functions
  PDSAnalysis(TString fiName="outFile_1.root", UInt_t runNum=1, TString foName="pdsEvTree_.root");
  ~PDSAnalysis();
  
  TTree* ImportTree(TString fiName, TString treeName);
  TTree* SetupNewTree();
  
  void Loop(TTree* pdsTree, TTree* outTree);
  void DoEventAnalysis(Int_t i);
  void RemoveADCOffset(TH1F* h);
  Bool_t IsEvent();
  TH1F* PMTSum();
  std::vector<TH1F*> PMTHists();
  Double_t Integral(TH1F* h, Int_t peak, Double_t pmtThreshold);
  Int_t FindPeak(TH1F* h);
  Double_t TriggerOffset(TH1F* h, Int_t peak_time, Double_t pmtThreshold);
  Double_t RecursiveQuadInter(TH1F* h, UInt_t time[3], Double_t p);
  Double_t CheckBeamWindow(Int_t peak);

  void PrintInfo();
  void DrawEvent(Int_t i);
};
