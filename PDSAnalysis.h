// Adapted from macro PDSAnalyze.C to be a compiled program

#include <sstream>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <complex>
#include <valarray>

#include <Rtypes.h>
#include <TTree.h>
#include <TString.h>
#include <TH1.h>
#include <TCanvas.h>

typedef std::complex<double> Complex;
typedef std::valarray<Complex> CArray;

class PDSAnalysis 
{
 public:
  // Analysis constants
  //   Described in .cc file
  static const Int_t  kMaxNHits    = 200;
  static const size_t kMaxNSamples = 2048; 
  static const size_t kNBoards     = 3;
  static const size_t kNChannels   = 6;
  static const size_t kNPMTs       = 15;

  static const Double_t kPi;
  
  static const Int_t kTrigger;
  static const Double_t kSampleRate;
  
  static const Double_t kSumThreshold;
  static const Double_t kRFThreshold;
  static const Double_t kPMTThreshold;
  static const Double_t kRatioThreshold;

  static const Int_t kPeakSearchWindow_pre;
  static const Int_t kPeakSearchWindow_post;
  static const Int_t kHitSearchWindow_pre;
  static const Int_t kHitSearchWindow_post;
  static const Int_t kBeamSearchWindow_post;
  static const Int_t kBeamSearchWindow_pre;

  static const Double_t kBeamPulseWidth;
  static const Double_t kTPCGateWidth;
  
  std::vector<Double_t> kADC_to_pe;
  std::vector<Double_t> kADCtick_to_pe;
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
  Int_t    runno;
  Int_t    tpc_trigno;
  Int_t    pds_ntrig;
  UShort_t gps_yr, gps_d;
  UInt_t   gps_s,  gps_ns;
  
  Int_t    pds_trigno;
  Int_t    tpc_subtrigno;
  Int_t    pds_nevent;
  Int_t    pds_evno;
  Int_t    pds_hits;
  Double_t pds_ratio;
  Double_t pds_time[kMaxNHits];
  Double_t pds_peak[kMaxNHits];
  Double_t pds_FWHM[kMaxNHits];
  Double_t pds_integral[kMaxNHits];
  Double_t pds_offset;
  Bool_t   pds_flag;

  Int_t    pmt_hits[kNPMTs];
  Double_t pmt_ratio[kNPMTs];
  Double_t pmt_time[kNPMTs][kMaxNHits];
  Double_t pmt_peak[kNPMTs][kMaxNHits];
  Double_t pmt_FWHM[kNPMTs][kMaxNHits];
  Double_t pmt_integral[kNPMTs][kMaxNHits];
  Double_t pmt_dtime[kNPMTs][kNPMTs];
  Double_t pmt_offset[kNPMTs];
  Bool_t   pmt_flag[kNPMTs];

  Double_t rf_time;
  Bool_t   inBeamWindow;
  Bool_t   isBeamTrigger;

  Double_t timeWt;

  // Other member variables
  TCanvas* fCanvas;
  Bool_t   fCalibration;
  Bool_t   fViewerMode;
  Bool_t   fStoreAll;
  Bool_t   fRateMode;
  std::vector<TH1F*>  fWaveform;
  std::vector<TH1F*>  fMeanWaveform;
  std::vector< std::vector<TH1F*> > fCalibrationFFT;

  // functions
  PDSAnalysis(TString fiName="outFile_1.root", UInt_t runNum=1, TString foName="pdsEvTree_.root", TString opt="");
  ~PDSAnalysis();

  void InitializeADC_to_pe();
  void InitializeADCtick_to_pe();
  TTree* ImportTree(TString fiName);
  TTree* SetupNewTree(TString foName);
  void   LoadCalibrationFFT();
  
  void Loop();
  void DoTrigAnalysis(Int_t start, Int_t end);
  void DoPMTAnalysis(Int_t pmt, Double_t rf_pulse=848);
  void DoPDSAnalysis(Double_t rf_pulse=848);

  Int_t QuickCheckMult(std::vector<Double_t> &rf_pulse);
  std::vector<Int_t> CheckHits(TH1F* h, Int_t pmt, std::vector<Int_t> &peak_time);
  Bool_t IsPMTEvent(TH1F* h, Int_t pmt, std::vector<Int_t> &peak_time);
  Bool_t IsPDSEvent(TH1F* h, std::vector<Int_t> &peak_time);

  void  InitializeWaveforms();
  TH1F* GetPMT(Int_t pmt, Bool_t first=false);
  TH1F* GetPMTSum(TString s="", Bool_t first=false);
  TH1F* GetRFMean(Bool_t first=false);
  Double_t RemoveADCOffset(TH1F* h, Double_t left_offset=0.0);
  TH1F* GausFilter(TH1F* h);
  TH1F* FFTFilter(TH1F* h, Int_t pmt);

  std::vector<Int_t> FindPeaks(TH1F* h, Int_t pmt=-1);
  Double_t FindEvTime(TH1F* h, Int_t peak_time);
  std::vector<Double_t> FindRFTime();
  Double_t FWHM(TH1F* h, Int_t peak_time);
  Double_t SumHits(TH1F* h, std::vector<Int_t> &peak_time);
  Double_t NegativeIntegral(TH1F* h, std::vector<Int_t> &peak_time);
  Double_t TotalIntegral(TH1F* h, std::vector<Int_t> &peak_time);
  Double_t FixedIntegral(TH1F* h, Int_t peak_time);
  Double_t Integral(TH1F* h, Int_t peak_time);

  Double_t QuadraticYInterpolate(Double_t x[3], Double_t y[3], Double_t p);
  Double_t QuadraticXInterpolate(Double_t y[3], Double_t x[3], Double_t p);

  void ConvertUnits();

  void FFT(CArray &x);
  void IFFT(CArray &x);

  void PrintEvent();
  void DrawEvent();
  void DrawEvent(Int_t number);
};
