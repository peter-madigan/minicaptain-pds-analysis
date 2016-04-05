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
  // Analysis constants
  static const UInt_t kMaxNEvents  = 100;
  static const size_t kMaxNSamples = 2048; 
  static const size_t kNBoards     = 3;
  static const size_t kNChannels   = 6;
  static const size_t kNPMTs       = 15;
  
  static const Int_t kTrigger;
  static const Double_t kSampleRate;
  
  static const Double_t kSumThreshold;
  static const Double_t kRFThreshold;
  static const Double_t kPMTThreshold;
  static const Double_t kIntegralThreshold_pmt;
  static const Double_t kIntegralThreshold_pds;
  static const Double_t kWidthThreshold;

  static const Int_t kPeakSearchWindow_pre;
  static const Int_t kPeakSearchWindow_post;
  static const Int_t kBeamSearchWindow_post;
  static const Int_t kBeamSearchWindow_pre;

  static const Double_t kBeamPulseWidth;
  static const Double_t kTPCGateWidth;
  
  std::vector<Double_t> kADC_to_pe;
  Double_t              kMeanADC_to_pe;
  static const Double_t kTick_to_ns;
  
  // Input tree
  TTree* fPMTTree;
  // Input tree variables
  UShort_t fDigitizerWaveform[kNBoards][8][kMaxNSamples];
  UInt_t   fEventNumber;
  UInt_t   fNSamples;
  UShort_t fGPS_y, fGPS_d;
  UInt_t   fGPS_s, fGPS_ns;
  Int_t    fComp_s; 
  Long64_t fComp_ns;
  UShort_t fGPS_flag;
  UInt_t   fDigitizerTime[kNBoards];
  
  // Output tree
  TTree* fAnalysisTree;
  // analysis variables
  UInt_t   runno;
  Int_t    tpc_evno;
  UInt_t   pds_nevent;
  UShort_t gps_yr, gps_d;
  UInt_t   gps_s,  gps_ns;
  
  Int_t    pds_evno[kMaxNEvents];
  Double_t pds_time[kMaxNEvents];
  Double_t pds_peak[kMaxNEvents];
  Double_t pds_FWHM[kMaxNEvents];
  Double_t pds_hits[kMaxNEvents];
  Double_t pds_integral[kMaxNEvents];
  Double_t pds_offset[kMaxNEvents];
  Bool_t   pds_flag[kMaxNEvents];

  Double_t pmt_time[kMaxNEvents][kNPMTs];
  Double_t pmt_peak[kMaxNEvents][kNPMTs];
  Double_t pmt_FWHM[kMaxNEvents][kNPMTs];
  Double_t pmt_hits[kMaxNEvents][kNPMTs];
  Double_t pmt_integral[kMaxNEvents][kNPMTs];
  Double_t pmt_dtime[kMaxNEvents][kNPMTs][kNPMTs];
  Double_t pmt_offset[kMaxNEvents][kNPMTs];
  Bool_t   pmt_flag[kMaxNEvents][kNPMTs];

  Double_t rf_time[kMaxNEvents];
  Bool_t   inBeamWindow[kMaxNEvents];
  Bool_t   isBeamTrigger[kMaxNEvents];

  Double_t timeWt[kMaxNEvents];

  // Other member variables
  TCanvas* fCanvas;
  Bool_t   fCalibration;
  Bool_t   fViewerMode;

  // functions
  PDSAnalysis(TString fiName="outFile_1.root", UInt_t runNum=1, TString foName="pdsEvTree_.root", Bool_t CalibrationMode=false, Bool_t ViewerMode=false);
  ~PDSAnalysis();

  void InitializeADC_to_pe();
  TTree* ImportTree(TString fiName);
  TTree* SetupNewTree(TString foName);
  
  void Loop();
  void DoEventAnalysis(Int_t start, Int_t end);
  void DoPMTAnalysis(Int_t subevent, Int_t pmt);
  void DoPDSAnalysis(Int_t subevent);
  
  Bool_t IsPMTEvent(TH1F* h, Int_t subevent, Int_t pmt, std::vector<Int_t> peak_time);
  Bool_t IsPDSEvent(TH1F* h, Int_t subevent, std::vector<Int_t> peak_time);

  TH1F* GetPMT(Int_t pmt);
  TH1F* GetPMTSum();
  TH1F* GetRFMean();
  Double_t RemoveADCOffset(TH1F* h, Double_t left_offset=0.0);

  std::vector<Int_t> FindPeaks(TH1F* h, Int_t pmt);
  Double_t FindEvTime(TH1F* h, Int_t peak_time);
  Double_t FindRFTime(TH1F* h, Int_t ev_time);
  Double_t FWHM(TH1F* h, Int_t peak_time);
  Double_t SumHits(TH1F* h, std::vector<Int_t> peak_time);
  Double_t NegativeIntegral(TH1F* h, std::vector<Int_t> peak_time);
  Double_t TotalIntegral(TH1F* h, std::vector<Int_t> peak_time);
  Double_t Integral(TH1F* h, Int_t peak_time);

  Double_t QuadraticYInterpolate(Double_t x[3], Double_t y[3], Double_t p);
  Double_t QuadraticXInterpolate(Double_t y[3], Double_t x[3], Double_t p);

  void ConvertUnits(Int_t subevent);

  void PrintEvent(Int_t subevent);
  void DrawEvent(Int_t subevent);
};
