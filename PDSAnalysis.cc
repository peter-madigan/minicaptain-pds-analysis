#include <sstream>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <complex>
#include <valarray>

#include <Rtypes.h>
#include <TTree.h>
#include <TString.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TFile.h>
#include <THStack.h>
#include <TMath.h>
#include <TLine.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TLatex.h>

#include "PDSAnalysis.h"

using namespace TMath;

typedef std::complex<Double_t> Complex;
typedef std::valarray<Complex> CArray;

const Double_t PDSAnalysis::kPi = Pi();

const Int_t    PDSAnalysis::kTrigger        = 848;
const Double_t PDSAnalysis::kSampleRate     = 250000000.;

const Double_t PDSAnalysis::kSumThreshold      = 3.00; // pe
const Double_t PDSAnalysis::kRFThreshold       = 50.0; // ADC
const Double_t PDSAnalysis::kPMTThreshold      = 1.00; // pe
const Double_t PDSAnalysis::kIntegralThreshold_pmt = 7.0; // ADC ticks
const Double_t PDSAnalysis::kIntegralThreshold_pds = 2.0; // pe ns
const Double_t PDSAnalysis::kWidthThreshold    = 6.0/4.0; // ticks
const Double_t PDSAnalysis::kRatioThreshold    = 0.50;

const Int_t    PDSAnalysis::kPeakSearchWindow_pre  = 250; // ticks vvv
const Int_t    PDSAnalysis::kPeakSearchWindow_post = 400; 
const Int_t    PDSAnalysis::kHitSearchWindow_pre   = 250;
const Int_t    PDSAnalysis::kHitSearchWindow_post  = 800;
const Int_t    PDSAnalysis::kBeamSearchWindow_post = 100;
const Int_t    PDSAnalysis::kBeamSearchWindow_pre  = 350;

const Double_t PDSAnalysis::kBeamPulseWidth = 625e-6;
const Double_t PDSAnalysis::kTPCGateWidth   = 4e-3;

const Double_t PDSAnalysis::kTick_to_ns     =  4.0/1.0;
void PDSAnalysis::InitializeADC_to_pe() {
  kADC_to_pe.push_back(-1./99999); // #1 Dead pmt
  kADC_to_pe.push_back(-1./7.565); // #2
  kADC_to_pe.push_back(-1./7.667); // #3
  kADC_to_pe.push_back(-1./7.000); // #4
  kADC_to_pe.push_back(-1./8.200); // #5

  kADC_to_pe.push_back(-1./7.483); // #6
  kADC_to_pe.push_back(-1./7.688); // #7
  kADC_to_pe.push_back(-1./8.000); // #8 
  kADC_to_pe.push_back(-1./8.833); // #9
  kADC_to_pe.push_back(-1./8.625); // #10

  kADC_to_pe.push_back(-1./7.056); // #11
  kADC_to_pe.push_back(-1./7.418); // #12
  kADC_to_pe.push_back(-1./8.388); // #13
  kADC_to_pe.push_back(-1./7.432); // #14
  kADC_to_pe.push_back(-1./7.889); // #15

  if( fCalibration )
    for( UInt_t pmt = 0; pmt < kNPMTs; pmt++ )
      kADC_to_pe[pmt] = 1.0;
}
void PDSAnalysis::InitializeADCns_to_pe() {
  kADCns_to_pe.push_back(-1./14.111); // #1 Dead pmt?
  kADCns_to_pe.push_back(-1./13.47); // #2   
  kADCns_to_pe.push_back(-1./13.57); // #3   
  kADCns_to_pe.push_back(-1./13.56); // #4   
  kADCns_to_pe.push_back(-1./13.07); // #5   
  
  kADCns_to_pe.push_back(-1./12.95); // #6   
  kADCns_to_pe.push_back(-1./12.25); // #7   
  kADCns_to_pe.push_back(-1./12.80); // #8   
  kADCns_to_pe.push_back(-1./13.52); // #9                                                        
  kADCns_to_pe.push_back(-1./11.56); // #10  
  
  kADCns_to_pe.push_back(-1./13.86); // #11                                   
  kADCns_to_pe.push_back(-1./13.80); // #12  
  kADCns_to_pe.push_back(-1./12.71); // #13                                        
  kADCns_to_pe.push_back(-1./11.61); // #14  
  kADCns_to_pe.push_back(-1./10.23); // #15   

  if( fCalibration )
    for( UInt_t pmt = 0; pmt < kNPMTs; pmt++ )
      kADCns_to_pe[pmt] = 1.0;
}


PDSAnalysis::PDSAnalysis(TString fiName, UInt_t runNum, TString foName, TString opt) 
{
  std::cout << "New analysis: " << runNum << std::endl;
  fCalibration = opt.Contains('c');
  fViewerMode  = opt.Contains('v');
  fStoreAll    = opt.Contains('s');
  fRateMode    = opt.Contains('r');

  if( fStoreAll )
    std::cout << "All triggers will be stored!" << std::endl;
  
  if( fRateMode ) {
    fStoreAll = true;
    std::cout << "Running in rate mode..." << "\n"
	      << "All events will be stored using a fixed search window of size: "
	      << (kHitSearchWindow_pre + kHitSearchWindow_post) * kTick_to_ns << "ns" << std::endl;
  } else
    std::cout << "Only pds events >" << kSumThreshold << "pe will be flagged." << std::endl;

  if( fViewerMode ) {
    std::cout << "Running in viewer mode..." << std::endl;
    fCanvas = new TCanvas("fCanvas","Event viewer");
  }

  if( fCalibration )
    std::cout << "Running in calibration mode..." << std::endl;
  LoadCalibrationFFT();

  runno = runNum;
  std::cout << "Analyzing run number " << runno << "..." << std::endl;

  fPMTTree = ImportTree(fiName);
  fAnalysisTree = SetupNewTree(foName);
  InitializeADC_to_pe();
  InitializeADCns_to_pe();
  
  Loop();
  
  fAnalysisTree->Write();
  for( UInt_t pmt = 0; pmt < kNPMTs+1; pmt++ )
    fMeanWaveform[pmt]->Write();

  std::cout << "Analysis complete!" << std::endl;
  
  if( fCalibration ) {
    TFile* fo = new TFile("./CalibrationFFT.root","RECREATE");
    for( UInt_t pmt = 0; pmt < kNPMTs+1; pmt++ ) {
      fCalibrationFFT[pmt][0]->Write();
      fCalibrationFFT[pmt][1]->Write();
      fMeanWaveform[pmt]->Write();
    }
    fo->Close();
  }
}

PDSAnalysis::~PDSAnalysis()
{
  std::cout << "Cleaning up..." << std::endl;
  if( fViewerMode )
    fCanvas->Close();
  if( !fMeanWaveform.empty() )
    for( UInt_t i = 0; i < fMeanWaveform.size(); i++ )
      fMeanWaveform[i]->Delete();
}

TTree* PDSAnalysis::ImportTree(TString fiName)
{
  std::cout << "Importing tree from " << fiName << "..." << std::endl;
  TFile* fi = new TFile(fiName);
  if(!fi) return NULL;
  
  TTree* tree = (TTree*)(fi->Get("pmt_tree"));
  if(!tree) return NULL;
  
  tree->SetBranchAddress("event_number", &fEventNumber);
  tree->SetBranchAddress("digitizer_waveforms", &fDigitizerWaveform);
  tree->SetBranchAddress("gps_ctrlFlag",    &fGPS_flag);
  tree->SetBranchAddress("gps_Year",        &fGPS_y);
  tree->SetBranchAddress("gps_daysIntoYear",&fGPS_d);
  tree->SetBranchAddress("gps_secIntoDay",  &fGPS_s);
  tree->SetBranchAddress("gps_nsIntoSec",   &fGPS_ns);
  tree->SetBranchAddress("computer_secIntoEpoch",&fComp_s);
  tree->SetBranchAddress("computer_nsIntoSec",   &fComp_ns);
  tree->SetBranchAddress("nSamples", &fNSamples);
  tree->SetBranchAddress("digitizer_time", &fDigitizerTime);
  
  return tree;
}

TTree* PDSAnalysis::SetupNewTree(TString foName)
{
  std::cout << "Creating new tree in " << foName << "..." << std::endl;
  TFile* fo = new TFile(foName,"RECREATE");
  if(!fo) return NULL;
  
  TTree* tree = new TTree("pdsEvTree","pdsEvTree");
    
  tree->Branch("runno",    &runno,    "runno/I");
  tree->Branch("tpc_trigno", &tpc_trigno, "tpc_trigno/I");
  tree->Branch("pds_ntrig",  &pds_ntrig,  "pds_ntrig/I");
  tree->Branch("gps_yr",   &gps_yr,   "gps_yr/s");
  tree->Branch("gps_d",    &gps_d,    "gps_d/s");
  tree->Branch("gps_s",    &gps_s,    "gps_s/i");
  tree->Branch("gps_ns",   &gps_ns,   "gps_ns/i");

  tree->Branch("pds_trigno", &pds_trigno, "pds_trigno/I");
  tree->Branch("pds_nevent", &pds_nevent, "pds_nevent/I");
  tree->Branch("pds_hits", &pds_hits, "pds_hits/I");
  tree->Branch("pds_ratio",&pds_ratio,"pds_ratio/D");
  tree->Branch("pds_time", pds_time, "pds_time[pds_hits]/D");
  tree->Branch("pds_peak", pds_peak, "pds_peak[pds_hits]/D");
  tree->Branch("pds_FWHM", pds_FWHM, "pds_FWHM[pds_hits]/D");
  tree->Branch("pds_integral", pds_integral, "pds_integral[pds_hits]/D");
  tree->Branch("pds_offset",   &pds_offset,   "pds_offset/D");
  tree->Branch("pds_flag", &pds_flag, "pds_flag/O");

  tree->Branch("pmt_hits", pmt_hits, "pmt_hits[15]/I");
  tree->Branch("pmt_ratio",pmt_ratio,"pmt_ratio[15]/D");
  tree->Branch("pmt_time", pmt_time, "pmt_time[15][200]/D");
  tree->Branch("pmt_peak", pmt_peak, "pmt_peak[15][200]/D");
  tree->Branch("pmt_FWHM", pmt_FWHM, "pmt_FWHM[15][200]/D");
  tree->Branch("pmt_integral", pmt_integral, "pmt_integral[15][200]/D");
  tree->Branch("pmt_dtime",    pmt_dtime,    "pmt_dtime[15][15]/D");
  tree->Branch("pmt_offset",   pmt_offset,   "pmt_offset[15]/D");
  tree->Branch("pmt_flag", pmt_flag, "pmt_flag[15]/O");

  tree->Branch("rf_time", &rf_time);
  tree->Branch("isBeamTrigger", &isBeamTrigger, "isBeamTrigger/O");
  tree->Branch("inBeamWindow",  &inBeamWindow,  "inBeamWindow/O");
  
  tree->Branch("timeWt",    &timeWt,    "timeWt/D");
  
  return tree;
}

void PDSAnalysis::LoadCalibrationFFT()
{
  if( fCalibration ) {
    for( UInt_t pmt = 0; pmt < kNPMTs; pmt++ ) {
      std::vector<TH1F*> complexHist;
      complexHist.push_back(new TH1F(Form("fCalibrationFFT_%d_re",pmt), Form("fCalibrationFFT_%d_re",pmt),kMaxNSamples,0,kMaxNSamples));
      complexHist.push_back(new TH1F(Form("fCalibrationFFT_%d_im",pmt), Form("fCalibrationFFT_%d_re",pmt),kMaxNSamples,0,kMaxNSamples));
      fCalibrationFFT.push_back(complexHist);
    }
    std::vector<TH1F*> complexHist;
    complexHist.push_back(new TH1F("fCalibrationFFT_sum_re", "fCalibrationFFT_sum_re",kMaxNSamples,0,kMaxNSamples));
    complexHist.push_back(new TH1F("fCalibrationFFT_sum_im", "fCalibrationFFT_sum_im",kMaxNSamples,0,kMaxNSamples));
    fCalibrationFFT.push_back(complexHist);
  
  } else {
    TFile* fi = new TFile("./CalibrationFFT.root","READ");
    if( !fi ) return;

    for( UInt_t pmt = 0; pmt < kNPMTs; pmt++ ) {
      std::vector<TH1F*> complexHist;
      complexHist.push_back((TH1F*)fi->Get(Form("fCalibrationFFT_%d_re",pmt)));
      complexHist.push_back((TH1F*)fi->Get(Form("fCalibrationFFT_%d_im",pmt)));
      fCalibrationFFT.push_back(complexHist);
    }
    std::vector<TH1F*> complexHist;
    complexHist.push_back((TH1F*)fi->Get("fCalibrationFFT_sum_re"));
    complexHist.push_back((TH1F*)fi->Get("fCalibrationFFT_sum_im"));
    fCalibrationFFT.push_back(complexHist);
  }
}

void PDSAnalysis::Loop()
{
  std::cout << "Starting loop..." << std::endl;

  Bool_t TPC0 = false;
  Bool_t TPC1 = false;

  // Loop over all trigs in tree
  tpc_trigno = 0;
  UInt_t i = 0; // TPC start variable
  UInt_t j = 1; // TPC end variable
  if( !fCalibration ) {
    while( fPMTTree->GetEntry(i) ) {
      // Check if current trig has a gps signature
      TPC0 = (fGPS_s + fGPS_ns > 0);
      if( TPC0 ) {
	// Check if next trig has a gps signature
	j = i+1;
	fPMTTree->GetEntry(j);
	TPC1 = (fGPS_s + fGPS_ns > 0);
	if( TPC1 ) {
	  // New TPC trigger
	  tpc_trigno++;
	  j++;
	  while( fPMTTree->GetEntry(j) ) {
	  // Find end of TPC trigger
	    TPC1 = (fGPS_s + fGPS_ns > 0);
	    if( TPC1 ) {
	      // j - 1 is end of TPC trigger
	    j--;
	    break;
	    }
	    else j++;
	  }
	  if( !fPMTTree->GetEntry(j) ) j--;
	  // Do analysis between tpc start and end
	  if( j>i ) DoTrigAnalysis(i,j);
	  else { i++; continue; }
	  
	  i++;
	} else i++;
      } else i++;  
    }
  }
  // No tpc trigger found -> do cosmic analysis
  if( tpc_trigno == 0 ) {
    if( !fCalibration )
      std::cout << "No tpc triggers found. Re-analyzing without tpc trig grouping..." << std::endl;
    else
      std::cout << "Looping in calibration mode..." << std::endl;
    while( fPMTTree->GetEntry(i) ) {
      // Do single trig analysis
      DoTrigAnalysis(i, i);

      // Save
      fAnalysisTree->Fill();
      i++;
    }
  }

  // Create convolution FFT
  if( fCalibration )
    for( UInt_t pmt = 0; pmt < kNPMTs+1; pmt++ ) {
      RemoveADCOffset(fMeanWaveform[pmt]);
      //fMeanWaveform[pmt]->Scale(-1./fMeanWaveform[pmt]->GetMinimum());
      if( pmt > 0 )
	FFTFilter(fMeanWaveform[pmt], pmt-1);
      else
	FFTFilter(fMeanWaveform[pmt], -1);
    } 
  
  // Draw mean
  if( fViewerMode && !fCanvas->IsBatch() ) {
    fCanvas->cd();
    fMeanWaveform[0]->Draw("l");
    fCanvas->Update();
    std::cout << "Press any key to continue..." << std::endl;
    char* s = new char[1];
    gets(s);
  }
}

void PDSAnalysis::DoTrigAnalysis(Int_t start, Int_t end)
{
  fPMTTree->GetEntry(start);
  gps_yr = fGPS_y;
  gps_d  = fGPS_d;
  gps_s  = fGPS_s;
  gps_ns = fGPS_ns;

  pds_ntrig = end - start + 1;
  Int_t subtrig = 0;
  while( fPMTTree->GetEntry(start + subtrig) && subtrig < pds_ntrig ) {
    // Find RF pulses
    std::vector<Double_t> rf_pulse = FindRFTime();
    
    if( rf_pulse.size() == 0 ) {
      // Cosmic trigger
      //
      // Do single analysis
      tpc_subtrigno = subtrig;
      pds_trigno = start + subtrig;
      pds_evno = 0;
      pds_nevent = 1;

      DoPDSAnalysis();
    
      // Do individual pmt analysis
      for( UInt_t pmt = 0; pmt < kNPMTs; pmt++ ) {
	DoPMTAnalysis(pmt);
      }
      
      // Draw trig
      if( fViewerMode ) {
	PrintEvent();
	DrawEvent();
      }
      
      // Convert units
      ConvertUnits();
      
      // Save and ready for next trig
      if( pds_flag || fCalibration || fStoreAll ) fAnalysisTree->Fill();
      //
    } else {
      // Beam trigger
      //
      // Check for events
      pds_nevent = QuickCheckMult(rf_pulse);
      
      // Update counters
      tpc_subtrigno = subtrig;
      pds_trigno = start + subtrig;
      
      // Loop over rf pulses
      if( pds_nevent > 0 || fViewerMode )
	for( UInt_t rf_i = 0; rf_i < rf_pulse.size(); rf_i++ ) {
	  // Do single analysis      
	  pds_evno = rf_i;
	  DoPDSAnalysis(rf_pulse[rf_i]);
	  
	  // Do individual pmt analysis                
	  for( UInt_t pmt = 0; pmt < kNPMTs; pmt++ ) {
	    DoPMTAnalysis(pmt, rf_pulse[rf_i]);
	  }
	  
	  // Draw trig                                             
	  if( fViewerMode ) {
	    PrintEvent();
	    DrawEvent();
	  }
	  
	  // Convert units                                     
	  ConvertUnits();
	  
	  // Save and ready for next trig                      
	  if( pds_flag || fCalibration || fStoreAll ) fAnalysisTree->Fill();
	}
    } 
    
    // Update
    if( pds_trigno % (fPMTTree->GetEntries()/10)==0 )
      PrintEvent();
    subtrig++;
  }
}

void PDSAnalysis::DoPMTAnalysis(Int_t pmt, Double_t rf_time) 
{
  // Create PMT histogram
  TH1F* hPMT = GetPMT(pmt);
  pmt_offset[pmt] = RemoveADCOffset(hPMT);
  //if( !fCalibration )
  //FFTFilter(hPMT, pmt);
  //MedianFilter(hPMT);

  // Find peaks in histogram
  std::vector<Int_t> peak_time = FindPeaks(hPMT, pmt);
  peak_time = CheckHits(hPMT, pmt, peak_time);
  if( (Int_t)peak_time.size() < kMaxNHits ) pmt_hits[pmt] = peak_time.size();
  else                                      pmt_hits[pmt] = kMaxNHits; 

  if( peak_time[0] != -9999 ) {
    if( peak_time.size() > 1 )
      pmt_ratio[pmt] = -hPMT->GetBinContent(peak_time[1])/hPMT->GetBinContent(peak_time[0]);
    else
      pmt_ratio[pmt] = 0;
    for( Int_t hit = 0; hit < pmt_hits[pmt] && hit < PDSAnalysis::kMaxNHits; hit++ ) {
      pmt_peak[pmt][hit] = hPMT->GetBinContent(peak_time[hit]);
      pmt_FWHM[pmt][hit] = FWHM(hPMT, peak_time[hit]);
      pmt_time[pmt][hit] = FindEvTime(hPMT, peak_time[hit]);
      pmt_integral[pmt][hit] = Integral(hPMT, peak_time[hit]);
    }
  } else {
    pmt_ratio[pmt] = -9999;
    pmt_peak[pmt][0] = -9999;
    pmt_FWHM[pmt][0] = -9999;
    pmt_time[pmt][0] = -9999; 
    pmt_integral[pmt][0] = -9999;
  }
  
  // Flag if not noise
  pmt_flag[pmt] = IsPMTEvent(hPMT, pmt, peak_time);

  // Store waveform for calibration FFT
  if( fMeanWaveform.size() != kNPMTs+1 ) {
    fMeanWaveform.push_back( (TH1F*)hPMT->Clone(Form("hMeanWaveform_%d",pmt)) );
    fMeanWaveform[fMeanWaveform.size()-1]->Reset();
    if( pmt_flag[pmt] )
      fMeanWaveform[fMeanWaveform.size()-1]->Add(hPMT);
  } else if( pmt_flag[pmt] )
    fMeanWaveform[pmt+1]->Add(hPMT);
  
  hPMT->Delete();
}

void PDSAnalysis::DoPDSAnalysis(Double_t rf_pulse) {
  // Create PMT histogram        
  TH1F* hPMT = GetPMTSum();
  pds_offset = RemoveADCOffset(hPMT);
  //if( !fCalibration )
  //FFTFilter(hPMT, -1);
  //MedianFilter(hPMT);

  // Check beam                                          
  if( rf_pulse != 848 ) rf_time = rf_pulse;
  else                  rf_time = -9999;
  inBeamWindow = !(rf_time == -9999);
  isBeamTrigger = (tpc_subtrigno == 0);

  // Find peaks in histogram   
  std::vector<Int_t> peak_time = FindPeaks(hPMT);
  peak_time = CheckHits(hPMT, -1, peak_time);
  pds_hits = peak_time.size();
  if( peak_time[0] != -9999 ) {
    if( peak_time.size() > 1 )
      pds_ratio = -hPMT->GetBinContent(peak_time[1])/hPMT->GetBinContent(peak_time[0]);
    else
      pds_ratio = 0;
    
    for( Int_t hit = 0; hit < pds_hits; hit++ ) {
      pds_peak[hit] = hPMT->GetBinContent(peak_time[hit]);
      pds_FWHM[hit] = FWHM(hPMT, peak_time[hit]);
      pds_time[hit] = FindEvTime(hPMT, peak_time[hit]);
      pds_integral[hit] = Integral(hPMT, peak_time[hit]);
    } 
  } else {
    pds_ratio = -9999;
    pds_peak[0] = -9999;
    pds_FWHM[0] = -9999;
    pds_time[0] = -9999;
    pds_integral[0] = -9999;
  }
  
  // Flag if not noise 
  pds_flag = IsPDSEvent(hPMT, peak_time);

  // Check beam
  //TH1F* hRF = GetRFMean();
  //RemoveADCOffset(hRF);
  //rf_time = FindRFTime(hRF, (Int_t)pds_time[0]);
  //inBeamWindow  = !(rf_time == -9999);
  //isBeamTrigger = (subtrig == 0);

  // Time weighting
  if( inBeamWindow )
    timeWt = 1./((kBeamSearchWindow_pre + kBeamSearchWindow_post));
  else
    timeWt = 1./((kPeakSearchWindow_pre + kPeakSearchWindow_post) * kTick_to_ns);
  
  // PMT delta t ** not used currently and not entirely necessary**
  /* for( UInt_t ipmt = 0; ipmt < kNPMTs; ipmt++ )
    for( UInt_t jpmt = 0; jpmt < kNPMTs; jpmt++ )
    if( pmt_time[ipmt][0] == -9999 || pmt_time[jpmt][0] == -9999 )
    pmt_dtime[ipmt][jpmt] = -9999;
    else
    pmt_dtime[ipmt][jpmt] = pmt_time[ipmt][0] - pmt_time[jpmt][0]; */
  
  // Store waveform for calibration FFT                                           
  if( fMeanWaveform.size() != kNPMTs+1 ) {
    fMeanWaveform.push_back( (TH1F*)hPMT->Clone("hMeanWaveform_sum") );
    fMeanWaveform[fMeanWaveform.size()-1]->Reset();
    if( pds_flag )
      fMeanWaveform[fMeanWaveform.size()-1]->Add(hPMT);
  } else if( pds_flag )
    fMeanWaveform[kNPMTs]->Add(hPMT);
  
  hPMT->Delete();
}

Int_t PDSAnalysis::QuickCheckMult(std::vector<Double_t> &rf_pulse)
{
  // Checks to see if there is a hit for each rf pulse
  //
  Int_t nevent = 0;
  TH1F* hPMT = GetPMTSum();
  pds_offset = RemoveADCOffset(hPMT);
  
  for( Int_t rf_i = 0; rf_i < rf_pulse.size(); rf_i++ )
    for( Int_t sample = Max(rf_pulse[rf_i]-kBeamSearchWindow_pre, 1.); sample < Min(rf_pulse[rf_i]+kBeamSearchWindow_post, fNSamples + 1.); sample++ )
      if( hPMT->GetBinContent(sample) < -kSumThreshold ) {
	nevent++;
	break;
      }
  
  hPMT->Delete();
  return nevent;
}

std::vector<Int_t> PDSAnalysis::CheckHits(TH1F* h, Int_t pmt, std::vector<Int_t> &peak_time) 
{
  if( peak_time[0] < 0 ) return peak_time;
  if( peak_time.size() == 1 && h->GetBinContent(peak_time[0]) < 0 ) return peak_time;
  
  // Sort peaks in order of time
  Double_t prompt_time;
  if( pmt < 0 ) prompt_time = peak_time[0];
  else          prompt_time = pds_time[0];
  std::sort( peak_time.begin(), peak_time.end() );
  
  // Compare forward ratio
  std::vector<Bool_t> cut_hit(peak_time.size(),false);
  for( UInt_t i = 0; i < peak_time.size(); i++ ) {
    Double_t forward_ratio = 0;
    Double_t forward_dt = 100;
    Double_t backward_ratio = 0;
    Double_t backward_dt = 100;
    if( i + 1 < peak_time.size() ) {
      forward_ratio = -h->GetBinContent(peak_time[i+1])/h->GetBinContent(peak_time[i]);
      forward_dt = Abs( peak_time[i+1] - peak_time[i] );
    } else if( (Int_t)i - 1 >= 0 ) {
      backward_ratio = -h->GetBinContent(peak_time[i-1])/h->GetBinContent(peak_time[i]);
      backward_dt = Abs( peak_time[i-1] - peak_time[i] );
    }
    
    if( forward_ratio > kRatioThreshold && forward_dt < 10 ) 
      cut_hit[i] = true;
    else if( backward_ratio > kRatioThreshold && backward_dt < 10 )
      cut_hit[i] = true;
    else if( h->GetBinContent(peak_time[i]) >= 0 )
      cut_hit[i] = true;
    else 
      cut_hit[i] = false;
  }

  // Cut peaks that fail ratio test and place element closest to prompt at start
  std::vector<Int_t> new_vector;
  Int_t maxi = 0;
  for( UInt_t i = 0; i < peak_time.size(); i++ ) {
    if( !cut_hit[i] ) {
      new_vector.push_back(peak_time[i]);
      if( Abs(new_vector[new_vector.size()-1] - prompt_time) < Abs(new_vector[maxi] - prompt_time))
	maxi = new_vector.size()-1;
    }
    
  }
  
  if( new_vector.empty() )
    new_vector.push_back(-9999);
      
  // Place closest element to prompt at start
  if( maxi != 0 ) {
    new_vector.insert(new_vector.begin(), new_vector[maxi]);
    new_vector.erase(new_vector.begin()+maxi+1);
  }

  // Check if oversize
  if( (Int_t)new_vector.size() > kMaxNHits )
    std::cout << "WARNING! Hits greater than kMaxNHits!" << std::endl;

  return new_vector;
}

Bool_t PDSAnalysis::IsPMTEvent(TH1F* h, Int_t pmt, std::vector<Int_t> &peak_time) 
{
  if( pmt_time[pmt][0] == -9999 ) return false;
  if( pmt_FWHM[pmt][0] == -9999 ) return false;
  if( pmt_peak[pmt][0] == -9999 ) return false;
  if( pmt_integral[pmt][0] == -9999 ) return false;
  //if( TotalIntegral(h,peak_time) > -kIntegralThreshold_pmt ) return false;
  //if( pmt_FWHM[pmt][0] < kWidthThreshold ) return false;
  //if( pmt_ratio[pmt] > kRatioThreshold ) return false;
  return true;
}

Bool_t PDSAnalysis::IsPDSEvent(TH1F* h, std::vector<Int_t> &peak_time) 
{
  if( pds_time[0] == -9999 ) return false;
  if( pds_FWHM[0] == -9999 ) return false;
  if( pds_peak[0] == -9999 ) return false;
  if( pds_integral[0] == -9999 ) return false;
  //if( TotalIntegral(h,peak_time) > -kIntegralThreshold_pds ) return false;
  //if( pds_FWHM[0] < kWidthThreshold ) return false;
  //if( pds_ratio >  kRatioThreshold ) return false;
  return true;
}

TH1F* PDSAnalysis::GetPMT(Int_t pmt) 
{
  UInt_t board;
  if( pmt < 5 )
    board = 2;
  else if( pmt < 10 )
    board = 0;
  else
    board = 1;
  UInt_t channel = pmt % 5;

  TString name = Form("hPMT_%d", pmt);
  TH1F* h = new TH1F(name, name, fNSamples, 0, fNSamples);
  for( UInt_t sample = 0; sample < fNSamples; sample++ )
    h->Fill(sample, fDigitizerWaveform[board][channel][sample]);
  return h;
}

TH1F* PDSAnalysis::GetPMTSum(TString s) 
{
  TString name = "hPMTSum"+s;
  TH1F* h = new TH1F(name, name, fNSamples, 0, fNSamples);
  
  for( UInt_t pmt = 0; pmt < kNPMTs; pmt++ ) {
    UInt_t board;
    if( pmt < 5 )
      board = 2;
    else if( pmt < 10 )
      board = 0;
    else
      board = 1;
    UInt_t channel = pmt % 5;
    
    
    for( UInt_t sample = 0; sample < fNSamples; sample++ ) {
      if( !fCalibration )
	h->Fill(sample, -fDigitizerWaveform[board][channel][sample] * kADC_to_pe[pmt]);
      else
	h->Fill(sample, fDigitizerWaveform[board][channel][sample]);
    }
  }

  return h;
}

TH1F* PDSAnalysis::GetRFMean() 
{
  TString name = "hRFMean";
  TH1F* h = new TH1F(name, name, fNSamples, 0, fNSamples);
  
  for( UInt_t board = 0; board < kNBoards; board++ ) {
    UInt_t channel = 5;
    for( UInt_t sample = 0; sample < fNSamples; sample++ )
      h->Fill(sample, fDigitizerWaveform[board][channel][sample]);
  }
  h->Scale(1.0/3);
  
  return h;
}

Double_t PDSAnalysis::RemoveADCOffset(TH1F* h, Double_t left_offset) 
{
  // calculates an offset from a 100ns interval
  // returns the offset with a std. dev. < 1 ADC or the offset with the smallest std. dev. (the faster of the two)
  Double_t offset = 0.0;
  Double_t offset_min = 0.0; 

  UInt_t n = 25;
  UInt_t start = 1;
  UInt_t end = n + start;

  Double_t sum = n;
  Double_t sumsq = Power(n, 2);
  Double_t stddev = Sqrt(sumsq/n - Power(sum/n,2));
  Double_t stddev_min = Sqrt(sumsq/n - Power(sum/n,2));
  while( stddev > 1.0 && end < fNSamples ) {
    sum = 0;
    sumsq = 0;
    for( UInt_t sample = start; sample < end; sample++ ) {
      sum += h->GetBinContent(sample);
      sumsq += Power(h->GetBinContent(sample), 2);
    }
    offset = sum/n;
    stddev = Sqrt(sumsq/n - Power(sum/n,2));
    if( stddev < stddev_min ) {
      offset_min = offset;
      stddev_min = stddev;
    }
    start++;
    end = n + start;
  }
  // Remove offset
  for( UInt_t sample = 0; sample < fNSamples; sample++ )
    h->Fill(sample, -offset + left_offset);

  //  std::cout << "start" << start << "\tend" << end << std::endl;
  //  std::cout << "Minimum std. dev. for offset: " << stddev_min << std::endl;
  return offset_min;
}

TH1F* PDSAnalysis::MedianFilter(TH1F* h)
{
  // Refills histogram with median of every 3 values
  std::vector<Double_t> values(3,0.0);
  TH1F* hTemp = (TH1F*)h->Clone("hTemp");
  for( Int_t i = 1; i < h->GetNbinsX()+1; i++ ) {
    if( i == 1 || i == h->GetNbinsX() )
      continue;
    else
      for( UInt_t j = 0; j < 3; j++ )
        values[j] = hTemp->GetBinContent(i+j-1);
    
    std::sort( values.begin(), values.end() );

    h->SetBinContent(i, values[1]);
  }
  hTemp->Delete();
  return h;
}

TH1F* PDSAnalysis::FFTFilter(TH1F* h, Int_t pmt)
{
  Complex waveform[fNSamples];
  for( Int_t i = 0; i < (Int_t)fNSamples; i++ ) 
    waveform[i] = h->GetBinContent(i+1);
  
  CArray fft(waveform, fNSamples);

  FFT(fft);
  if( !fCalibration ) {
    // Deconvolution
    /* if( pmt < 0 )
      for( Int_t i = 0; i < (Int_t)fNSamples/2-1; i++ ) {
	Complex c(fCalibrationFFT[kNPMTs][0]->GetBinContent(i+1), fCalibrationFFT[kNPMTs][1]->GetBinContent(i+1));
	if( std::norm(c) != 0 )
	  fft[i] = fft[i] / c;
      }
    else if( false )
      for( Int_t i = 0; i < (Int_t)fNSamples/2-1; i++ ) {
	Complex c(fCalibrationFFT[pmt][0]->GetBinContent(i+1), fCalibrationFFT[pmt][1]->GetBinContent(i+1));
	if( std::norm(c) != 0 )
	  fft[i] = fft[i] / c;
	  } */
    // Band-pass filter
    for( Int_t i = 0; i < (Int_t)fNSamples; i++ )
      if( i == (Int_t)fNSamples / 2 )
	fft[i] = 0;
    else if( Abs(1.*fNSamples-i) == 512 || i == 512 ) // lots of noise at this freq
      fft[i] = fft[i] / (Complex)(12);
    else if( i < 260 && i > 230 )
      fft[i] = fft[i] / (Complex)(Abs(245-i)+1);
    else if( Abs(1.*fNSamples-i) < 260 && Abs(1.*fNSamples-i) > 230 )
      fft[i] = fft[i] / (Complex)(Abs(-1.*fNSamples+i+245)+1);
    else if( Abs(1.*fNSamples-i) == 131 || i == 131 ) // lots of noise at this freq
      fft[i] = fft[i] / (Complex)(12);
    //else if( i > 512 ) // high freq filter
    //fft[i] = fft[i] / (Complex)(i-512);
    
    IFFT(fft);
    
    for( Int_t i = 0; i < (Int_t)fNSamples; i++ )  {
      h->SetBinContent(i+1, std::real(fft[i]));
    }
    
  } else {
    // Band-pass filter
    for( Int_t i = 0; i < (Int_t)fNSamples; i++ )
      if( i >= (Int_t)fNSamples / 2 - 1 )
        fft[i] = 0;
      else if( i == 512 ) // lots of noise at this freq            
	fft[i] = 0;
      else if( i == 131 )
	fft[i] = 0;
    //else if( i > 512 ) // high freq filter
    //fft[i] = fft[i] / (Complex)(i-512);

    if( pmt < 0 )
      for( Int_t i = 0; i < (Int_t)fNSamples; i++ ) {
        fCalibrationFFT[kNPMTs][0]->SetBinContent(i+1, std::real(fft[i]));
	fCalibrationFFT[kNPMTs][1]->SetBinContent(i+1, std::imag(fft[i]));
      }
    else
      for( Int_t i = 0; i < (Int_t)fNSamples; i++ ) {
	fCalibrationFFT[pmt][0]->SetBinContent(i+1, std::real(fft[i]));
	fCalibrationFFT[pmt][1]->SetBinContent(i+1, std::imag(fft[i]));
      } 

    IFFT(fft);

    for( Int_t i = 0; i < (Int_t)fNSamples; i++ )  {
      h->SetBinContent(i+1, std::real(fft[i]));
    }
  }
  return h;
}

std::vector<Int_t> PDSAnalysis::FindPeaks(TH1F* h, Int_t pmt) 
{
  // Produces a list of peaks above the threshold in the beam window (if beam event) or search 
  //   window (if cosmic event)
  // First entry is global minimum (for pmt sum)
  
  // Set threshold
  Double_t threshold;
  if( fCalibration )
    threshold = 1.0;
  else if( pmt < 0 )
    if( fRateMode )
      threshold = -kPMTThreshold; // Don't set a sum threshold
    else
      threshold = -kSumThreshold;
  else
    threshold = kPMTThreshold / kADC_to_pe[pmt];
  
  // Find prompt (for PMT sum)
  UInt_t start;
  UInt_t finish;
  std::vector<Int_t> peak_time(1,-9999);
  if( pmt < 0 ) {
    if( fCalibration ) {
      start = 850;
      finish = 1000;
    } else if ( fRateMode ) {
      start = kTrigger - 5;//kHitSearchWindow_pre;
      finish = kTrigger + 5;//kHitSearchWindow_post;
    } else if ( rf_time != -9999 ){
      start = Max(rf_time - kBeamSearchWindow_pre, 1.);
      finish = Min(rf_time + kBeamSearchWindow_post, fNSamples+1.);
    } else {
      start = kTrigger - kPeakSearchWindow_pre;
      finish = kTrigger + kPeakSearchWindow_post;
    }
    for( UInt_t sample = start; sample < finish; sample++ ) {
      if( h->GetBinContent(sample) < threshold ) {
	if( peak_time[0] == -9999 )
	  peak_time[0] = sample;
	else if( h->GetBinContent(sample) < h->GetBinContent(peak_time[0]) )
	  peak_time[0] = sample;
      }
    }
  }
  
  // Find hits
  if( fCalibration ) {
    start = 850;
    finish = 1000;
  } else if( fRateMode ) {
    start = kTrigger - kHitSearchWindow_pre;                                      
    finish = kTrigger + kHitSearchWindow_post;
  } else if( pmt < 0 ) {
    if( peak_time[0] == -9999 ) return peak_time; // Skip events without coincidence/large signal (pmt sum)
    start = Max(peak_time[0] - kHitSearchWindow_pre, 1);
    finish = Min(peak_time[0] + kHitSearchWindow_post, (Int_t)fNSamples+1);
  } else if( pds_time[0] != -9999 ) {
    start = Max(pds_time[0] - kHitSearchWindow_pre, 1.);
    finish = Min(pds_time[0] + kHitSearchWindow_post, fNSamples+1.);
  } else return peak_time; // Skip events without coincidence/large signal (indiv. pmt)
  
  Double_t local_peak_time = peak_time[0];
  for( Int_t sample = start; sample < finish; sample++ )
    // Search for zero crossing OR threshold crossing
    if( h->GetBinContent(sample-1) * h->GetBinContent(sample) < 0 || 
	(Abs(h->GetBinContent(sample)) >= Abs(threshold) && 
	 Abs(h->GetBinContent(sample-1)) < Abs(threshold))) {
      // Crossed -> store previous peak
      if( local_peak_time != peak_time[0] && 
	  Abs(h->GetBinContent(local_peak_time)) > Abs(threshold) ) 
	peak_time.push_back(local_peak_time);
      // Crossed -> reset local peak
      local_peak_time = sample;
    } else if( local_peak_time != -9999 &&
	       Abs( h->GetBinContent(sample) ) > Abs( h->GetBinContent(local_peak_time) ) ) {
      // New maximum found
      local_peak_time = sample;
    }
  
  if( peak_time[0] == -9999 )
    peak_time.erase(peak_time.begin());
  
  return peak_time;
}

Double_t PDSAnalysis::FindEvTime(TH1F* h, Int_t peak_time)
{
  Double_t ev_time = -9999;
  
  // Interpolates the 10%-peak time (maximum half width at base = 200ns)
  Double_t peak_10p = h->GetBinContent(peak_time) * 0.10;
  for( Int_t sample = peak_time-1; sample > Max(peak_time - 50, 1); sample-- ) 
    if( Abs( h->GetBinContent(sample) ) < Abs(peak_10p) 
	|| h->GetBinContent(sample) * h->GetBinContent(sample+1) < 0) {
      Double_t dt = h->GetBinWidth(sample)/2;
      Double_t times[] = { h->GetBinLowEdge(sample-1)+dt, h->GetBinLowEdge(sample)+dt, 
			   h->GetBinLowEdge(sample+1)+dt };
      Double_t values[] = { h->GetBinContent(sample-1), h->GetBinContent(sample), 
			    h->GetBinContent(sample+1) };
      ev_time = QuadraticXInterpolate( values, times, peak_10p );
      break;
    }
  if( ev_time > fNSamples || ev_time < 0 )
    if( ev_time != -9999 ) {
      std::cout << "Warning: Event time is " << ev_time * kTick_to_ns << "ns!" << std::endl;
      return -9999;
    }

    return ev_time;
}

std::vector<Double_t> PDSAnalysis::FindRFTime()
{
  TH1F* h = GetRFMean();
  RemoveADCOffset(h);
  
  Int_t start = 0;
  Int_t end   = fNSamples;
  std::vector<Double_t> rf_time;

  // Interpolates the RF-threshold crossing time(s)
  for( Int_t sample = start+1; sample < end+1; sample++ ) {
    if( h->GetBinContent(sample) < -kRFThreshold 
	&& h->GetBinContent(sample-1) > -kRFThreshold ) {
      Double_t dt = h->GetBinWidth(sample)/2;
      Double_t times[] = { h->GetBinLowEdge(sample-1)+dt, h->GetBinLowEdge(sample)+dt,
			   h->GetBinLowEdge(sample+1)+dt };
      Double_t values[] = { h->GetBinContent(sample-1), h->GetBinContent(sample),
                            h->GetBinContent(sample+1) };
      rf_time.push_back( QuadraticXInterpolate( values, times, -kRFThreshold ) );
    }
  }

  h->Delete();
  return rf_time;
}

Double_t PDSAnalysis::FWHM(TH1F* h, Int_t peak_time)
{
  Double_t HM = h->GetBinContent(peak_time) * 0.5;
  // interpolate first HM
  Double_t time1 = -9999;
  for( Int_t sample = peak_time-1; sample > peak_time - 50; sample-- ) {
    if( Abs(h->GetBinContent(sample)) < Abs(HM) 
	|| h->GetBinContent(sample) * h->GetBinContent(sample+1) < 0) {
      Double_t dt = h->GetBinWidth(sample)/2;
      Double_t times[] = { h->GetBinLowEdge(sample-1)+dt, h->GetBinLowEdge(sample)+dt,
                           h->GetBinLowEdge(sample+1)+dt };
      Double_t values[] = { h->GetBinContent(sample-1), h->GetBinContent(sample),
                            h->GetBinContent(sample+1) };
      time1 = QuadraticXInterpolate( values, times, HM );
      break;
    }
  }

  // interpolate second HM
  Double_t time2 = -9999;
  for( Int_t sample = peak_time+1; sample < peak_time + 50; sample++ ) {
    if( Abs(h->GetBinContent(sample)) < Abs(HM) 
	|| h->GetBinContent(sample) * h->GetBinContent(sample-1) < 0) {
      Double_t dt = h->GetBinWidth(sample)/2;
      Double_t times[] = { h->GetBinLowEdge(sample-1)+dt, h->GetBinLowEdge(sample)+dt,
                           h->GetBinLowEdge(sample+1)+dt };
      Double_t values[] = { h->GetBinContent(sample-1), h->GetBinContent(sample),
                            h->GetBinContent(sample+1) };
      time2 = QuadraticXInterpolate( values, times, HM );
      break;
    }
  }

  if( time1 == -9999 || time2 == -9999 )
    return -9999;
  else
    return time2 - time1;
}

Double_t PDSAnalysis::SumHits(TH1F* h, std::vector<Int_t> &peak_time)
{
  if( peak_time[0] == -9999 ) return -9999;

  Double_t sum = 0;
  for( Int_t peak = 0; peak < (Int_t)peak_time.size(); peak++ ) {
    if( h->GetBinContent(peak_time[peak]) < 0 )
      sum += h->GetBinContent(peak_time[peak]);
  }
  return sum;
}

Double_t PDSAnalysis::NegativeIntegral(TH1F* h, std::vector<Int_t> &peak_time) 
{
  // Only integrates the negative peaks
  Double_t integral = 0;
  if( peak_time[0] == -9999 ) return 0;
  for( UInt_t peak = 0; peak < peak_time.size(); peak++ )
    if( h->GetBinContent(peak_time[peak]) < 0 ) {
      integral += Integral(h, peak_time[peak]);
    }
  return integral;
}

Double_t PDSAnalysis::TotalIntegral(TH1F* h, std::vector<Int_t> &peak_time)
{
  // Integrates all peaks
  Double_t integral = 0;
  if( peak_time[0] == -9999 ) return -9999;
  for( UInt_t peak = 0; peak < peak_time.size(); peak++ )
    integral += Integral(h, peak_time[peak]);
  return integral;
}

Double_t PDSAnalysis::Integral(TH1F* h, Int_t peak_time)
{
  // Integrates peak across 10%-peak width
  Double_t peak_10p = h->GetBinContent(peak_time) * 0.10;
  Double_t integral = 0;
  UInt_t sample = peak_time;
  integral += h->GetBinContent(peak_time);
  
  // Backwards from peak
  sample = peak_time - 1;
  while( Abs( h->GetBinContent(sample) ) > Abs(peak_10p) 
	 && h->GetBinContent(sample+1) * h->GetBinContent(sample) > 0) {
    integral += h->GetBinContent(sample);
    sample--;
  }
  // Interpolate last bin
  Double_t dt = h->GetBinWidth(sample)/2;
  Double_t times[] = { h->GetBinLowEdge(sample-1)+dt, h->GetBinLowEdge(sample)+dt,
		       h->GetBinLowEdge(sample+1)+dt };
  Double_t values[] = { h->GetBinContent(sample-1), h->GetBinContent(sample),
			h->GetBinContent(sample+1) };
  Double_t x0 = QuadraticXInterpolate( values, times, peak_10p );
  Double_t x1 = h->GetBinLowEdge(sample+1)+dt;
  Double_t dx = x1 - x0;
  Double_t y  = (h->GetBinContent(sample+1) + peak_10p)/2;
  integral += y * dx;
  
  // Forwards from peak
  sample = peak_time + 1;
  while( Abs( h->GetBinContent(sample) ) > Abs(peak_10p) 
	 && h->GetBinContent(sample-1) * h->GetBinContent(sample) > 0) {
    integral += h->GetBinContent(sample);
    sample++;
  }
  // Interpolate last bin   
  dt = h->GetBinWidth(sample)/2;
  times = { h->GetBinLowEdge(sample-1)+dt, h->GetBinLowEdge(sample)+dt,
	    h->GetBinLowEdge(sample+1)+dt };
  values = { h->GetBinContent(sample-1), h->GetBinContent(sample),
	     h->GetBinContent(sample+1) };
  x1 = QuadraticXInterpolate( values, times, peak_10p );
  x0 = h->GetBinLowEdge(sample-1)+dt;
  dx = x1 - x0;
  y  = (h->GetBinContent(sample-1) + peak_10p)/2;
  integral += y * dx;
  
  return integral;
}

Double_t PDSAnalysis::QuadraticYInterpolate(Double_t x[3], Double_t y[3], Double_t p)
{
  // 2nd order Lagrange polynomial interpolation
  // if x-pts are equal, returns highest order result possible
  
  // Degenerate, return mean
  if( x[0] == x[1] == x[2] ) return (y[0]+y[1]+y[2])/3;
  
  // 2nd order Lagrange 
  Bool_t failed = false;
  Double_t sum = 0;
  for( Int_t i = 0; i < 3; i++ ) {
    Double_t mult = y[i];
    for( Int_t j = 0; j < 3; j++ )
      if( i != j && x[i] != x[j] )
	mult *= ( p - x[j] ) / ( x[i] - x[j] );
      else if( i != j && x[i] == x[j] )
	failed = true;
    sum += mult;
  }

  if( failed )
    // 1st order Lagrange
    for( Int_t i = 0; i < 3; i++ )
      for( Int_t j = 0; j < 3; j++ )
	if( x[i] != x[j] )
	  return y[i]*(p-x[j])/(x[i]-x[j]) + y[j]*(p-x[i])/(x[j]-x[i]);

  return sum;
}

Double_t PDSAnalysis::QuadraticXInterpolate(Double_t y[3], Double_t x[3], Double_t p) 
{
  Double_t value = 0;

  Double_t A = 0;
  Double_t B = 0;
  Double_t C = 0;
  Double_t D = p;

  for( Int_t i = 0; i < 3; i++ ) {
    A +=  y[i] * (x[(i+1)%3] - x[(i+2)%3]);
    B += -y[i] * (Power(x[(i+1)%3],2) - Power(x[(i+2)%3],2));
    C +=  y[i] * x[(i+1)%3] * x[(i+2)%3] * (x[(i+1)%3] - x[(i+2)%3]);
    D *=  x[i] - x[(i+1)%3];
  }

  Double_t divisor = 2 * A;
  Double_t value1 = -B + Sqrt(Power(B,2) - 4 * A * ( C + D ));
  Double_t value2 = -B - Sqrt(Power(B,2) - 4 * A * ( C + D ));

  if( divisor == 0 ) // all x-values are equal -> return mean
    return (x[0]+x[1]+x[2])/3.;
  else if( 4 * A * ( C + D ) > Power(B,2) ) // some x-values are equal -> return Linear
    return QuadraticYInterpolate( y, x, p );
  else {
    value1 *= 1./divisor;
    value2 *= 1./divisor;
  }
 
  // return closest value to x[1]
  if( Abs(value1 - x[1]) < Abs(value2 - x[1]) )
    value = value1;
  else
    value = value2;

  return value;
}

void PDSAnalysis::ConvertUnits()
{
  Double_t mean_conversion = 0;
  for( UInt_t pmt = 0; pmt < kNPMTs; pmt++ ) {
    if( pmt != 0 ) mean_conversion = kADCns_to_pe[pmt] / kADC_to_pe[pmt];
    for( Int_t hit = 0; hit < pmt_hits[pmt] && hit < PDSAnalysis::kMaxNHits; hit++ ) {
      pmt_time[pmt][hit] *= kTick_to_ns;
      pmt_peak[pmt][hit] *= kADC_to_pe[pmt];
      pmt_FWHM[pmt][hit] *= kTick_to_ns;
      pmt_integral[pmt][hit] *= kADCns_to_pe[pmt];
    }
    for( UInt_t pmt2 = 0; pmt2 < kNPMTs; pmt2++ )
      pmt_dtime[pmt][pmt2] *= kTick_to_ns;
  }
  mean_conversion = mean_conversion / (kNPMTs - 1);
  for( Int_t hit = 0; hit < pds_hits; hit++ ) {
    pds_time[hit] *= kTick_to_ns;
    pds_peak[hit] *= -1.;
    pds_FWHM[hit] *= kTick_to_ns;
    pds_integral[hit] *= -mean_conversion;
  }
  rf_time *= kTick_to_ns;
}

void PDSAnalysis::FFT(CArray &x)
{
  const size_t N = x.size();
  if( N <= 1 ) return;

  CArray even = x[std::slice(0, N/2, 2)];
  CArray odd  = x[std::slice(1, N/2, 2)];

  FFT(even);
  FFT(odd);

  for( size_t k = 0; k < N/2; k++) {
    Complex t = std::polar(1.0, -2 * kPi * k / N) * odd[k];
    x[k]     = even[k] + t;
    x[k+N/2] = even[k] - t;
  }
}

void PDSAnalysis::IFFT(CArray &x)
{
  x = x.apply(std::conj);
  FFT(x);
  x = x.apply(std::conj);
  x /= x.size();
}

void PDSAnalysis::PrintEvent()
{
  std::cout << "Run#" << runno << "\n"
	    << "TPC trig#" << tpc_trigno << "\t"
	    << "Sub trig#" << tpc_subtrigno << "\t"
	    << "PDS trig#" << pds_trigno << "/" << fPMTTree->GetEntriesFast() << "\t"
	    << "PDS evno#" << pds_evno << "\n"
	    << "Multiplicity: " << pds_nevent << "\n"
	    << "GPS:" << Form("%.14g", gps_s + gps_ns*1e-9) << "\n"
	    << "RF:" << Form("%.6g",rf_time * kTick_to_ns) << "\t"
	    << "BEAM?:" << inBeamWindow << "\n"
	    << "PMT:  \tTIME: \tPEAK: \tFWHM: \tINT:  \tBL:   \tRATIO:\tHITS: \tFLAG?:" << "\n"
	    << "PDS" << "\t" << Form("%.6g",pds_time[0] * kTick_to_ns) << "\t"
	    << Form("%.4g",pds_peak[0] * -1.) << "\t"
	    << Form("%.4g",pds_FWHM[0] * kTick_to_ns) << "\t"
	    << Form("%.4g",pds_integral[0] * kTick_to_ns * -1.) << "\t"
	    << Form("%.4g",pds_offset) << "\t"
	    << Form("%.4g",pds_ratio) << "\t"
	    << Form("%5d",pds_hits) << "\t"
	    << pds_flag << "\n";
  for( UInt_t pmt = 0; pmt < kNPMTs; pmt++ )
    std::cout << pmt + 1 << "\t"
	      << Form("%.4g",pmt_time[pmt][0] * kTick_to_ns) << "\t"
	      << Form("%.4g",pmt_peak[pmt][0] * kADC_to_pe[pmt]) << "\t"
	      << Form("%.4g",pmt_FWHM[pmt][0] * kTick_to_ns) << "\t"
	      << Form("%.4g",pmt_integral[pmt][0] * kADCns_to_pe[pmt]) << "\t"
	      << Form("%.4g",pmt_offset[pmt]) << "\t"
	      << Form("%.4g",pmt_ratio[pmt]) << "\t"
	      << Form("%5d",pmt_hits[pmt]) << "\t"
	      << pmt_flag[pmt] << "\n";
  std::cout << std::endl;
}

void PDSAnalysis::DrawEvent()
{
  TLatex latex;
  gStyle->SetOptStat(0);
  fCanvas->cd();
  //fCanvas->SetRightMargin(0.25);
  //fCanvas->SetGridx();
  //fCanvas->SetGridy();

  TObjArray* pmt_hists = new TObjArray();
  TObjArray* pmt_lines = new TObjArray();
  //TLegend* leg = new TLegend(0.75,0.5,0.95,0.95);

  Double_t xmin = 0;
  Double_t xmax = fNSamples;
  
  Double_t ymin = -150;
  Double_t ymax = +300;

  // Draw PDS sum
  TH1F* hSum = GetPMTSum();
  pmt_hists->Add(hSum);
  RemoveADCOffset(hSum);
  hSum->Scale(3.);
  //FFTFilter(hSum, -1);
  //MedianFilter(hSum);
  RemoveADCOffset(hSum,-50);
  xmin = 0;
  xmax = fNSamples;
  hSum->GetXaxis()->SetRangeUser(xmin,xmax);
  hSum->GetYaxis()->SetRangeUser(ymin,ymax);
  hSum->SetTitle(Form("PDS%d-TPC%d-%d.%d",pds_trigno,tpc_trigno,tpc_subtrigno,pds_evno));
  hSum->GetXaxis()->SetTitle("Sample (4ns ea.)");
  hSum->GetYaxis()->SetTitle("ADC count");
  if( !pds_flag )
    hSum->SetLineColor(kGray + 2);
  else
    hSum->SetLineColor(kBlue + 2);
  hSum->Draw("l");
  latex.DrawLatex(.02*fNSamples,-50+5,"#scale[0.4]{#font[4]{PMT sum (scaled)}}");

  // Draw PDS lines
  TLine* l_trig = new TLine(kTrigger, ymin, kTrigger, ymax);
  pmt_lines->Add(l_trig);
  l_trig->SetLineColor(kBlack);
  l_trig->SetLineStyle(2);
  l_trig->Draw("same");

  if( pds_flag ) {
    RemoveADCOffset(hSum);
    hSum->Scale(1./3.);
    std::vector<Int_t> peak_time = FindPeaks(hSum);
    peak_time = CheckHits(hSum, -1, peak_time);
    hSum->Scale(3.);
    RemoveADCOffset(hSum,-50);
    xmin = pds_time[0] - kHitSearchWindow_pre;
    xmax = pds_time[0] + kHitSearchWindow_post;
    hSum->GetYaxis()->SetRangeUser(ymin,ymax);

    Int_t n = peak_time.size();
    Double_t t[n];
    Double_t y[n];
    for( int i = 0; i < n; i++ ) {
      Double_t dt = hSum->GetBinWidth(peak_time[i])/2;
      t[i] = hSum->GetBinLowEdge(peak_time[i]) + dt;
      y[i] = hSum->GetBinContent(peak_time[i]);
    }
    TGraph* g_peak = new TGraph(n,t,y);
    pmt_hists->Add(g_peak);
    g_peak->SetMarkerStyle(24);
    g_peak->SetMarkerColor(kRed);
    g_peak->SetMarkerSize(0.3);
    g_peak->Draw("same p");

    //TLine* l_threshold1 = new TLine(xmin, kSumThreshold*3-50, xmax, kSumThreshold*3-50);
    TLine* l_threshold2 = new TLine(xmin, -kSumThreshold*3-50, xmax, -kSumThreshold*3-50);
    //pmt_lines->Add(l_threshold1);
    pmt_lines->Add(l_threshold2);
    //l_threshold1->SetLineColor(kBlue + 2);
    l_threshold2->SetLineColor(kBlue + 2);
    //l_threshold1->SetLineStyle(2);
    l_threshold2->SetLineStyle(2);
    //l_threshold1->Draw("same");
    l_threshold2->Draw("same");
    
    TLine* l_time = new TLine(pds_time[0], ymin, pds_time[0], ymax);
    pmt_lines->Add(l_time);
    l_time->SetLineColor(kSpring + 2);
    l_time->SetLineStyle(2);
    l_time->Draw("same");
  }

  // Draw RF mean
  TH1F* hRF = GetRFMean();
  pmt_hists->Add(hRF);
  hRF->Scale(1./15);
  RemoveADCOffset(hRF,-25);
  xmin = 0;
  xmax = fNSamples;
  hRF->GetXaxis()->SetRangeUser(xmin,xmax);
  hRF->GetYaxis()->SetRangeUser(ymin,ymax);
  if( !inBeamWindow )
    hRF->SetLineColor(kGray + 2);
  else
    hRF->SetLineColor(kRed + 2);
  hRF->Draw("l same");
  latex.DrawLatex(.02*fNSamples,-25+5,"#scale[0.4]{#font[4]{RF pulse (scaled)}}");
  
  // Draw RF lines
  if( inBeamWindow ) {
    xmin = rf_time - kBeamSearchWindow_pre;
    xmax = rf_time + kBeamSearchWindow_post;

    TLine* l_thresh = new TLine(xmin, -kRFThreshold/30-25, xmax, -kRFThreshold/30-25);
    pmt_lines->Add(l_thresh);
    l_thresh->SetLineColor(kRed + 2);
    l_thresh->SetLineStyle(2);
    l_thresh->Draw("same");

    TLine* l_time = new TLine(rf_time, ymin, rf_time, ymax);
    pmt_lines->Add(l_time);
    l_time->SetLineColor(kGreen + 2);
    l_time->SetLineStyle(2);
    l_time->Draw("same");
  }

  // Draw PMTs
  for( UInt_t pmt = 0; pmt < kNPMTs; pmt++ ) {
    TH1F* h = GetPMT(pmt);
    pmt_hists->Add(h);
    //RemoveADCOffset(h);
    //FFTFilter(h,pmt);
    //MedianFilter(h);
    RemoveADCOffset(h,pmt*20);
    xmin = 0;
    xmax = fNSamples;
    h->GetXaxis()->SetRangeUser(xmin,xmax);
    h->GetYaxis()->SetRangeUser(ymin,ymax);
    if( !pmt_flag[pmt] )
      h->SetLineColor(kGray);
    else
      h->SetLineColor(kBlue);
    h->Draw("l same");
    latex.DrawLatex(.02*fNSamples,pmt*20+5,Form("#scale[0.4]{#font[4]{PMT#%d}}",pmt+1));

    // Draw PMT lines
    if( pmt_flag[pmt] ) {
      RemoveADCOffset(h);
      std::vector<Int_t> peak_time = FindPeaks(h, pmt);
      peak_time = CheckHits(h, pmt, peak_time);
      RemoveADCOffset(h,pmt*20);
      xmin = pds_time[0] - kHitSearchWindow_pre;
      xmax = pds_time[0] + kHitSearchWindow_post;
      h->GetYaxis()->SetRangeUser(ymin,ymax);

      Int_t n = peak_time.size();
      Double_t t[n];
      Double_t y[n];
      for( int i = 0; i < n; i++ ) {
	Double_t dt = h->GetBinWidth(peak_time[i])/2;
	t[i] = h->GetBinLowEdge(peak_time[i]) + dt;
	y[i] = h->GetBinContent(peak_time[i]);
      }
      TGraph* g_peak = new TGraph(n,t,y);
      pmt_hists->Add(g_peak);
      g_peak->SetMarkerStyle(24);
      g_peak->SetMarkerColor(kRed);
      g_peak->SetMarkerSize(0.2);
      g_peak->Draw("same p");

      //TLine* l_threshold1 =new TLine(xmin, -kPMTThreshold/kADC_to_pe[pmt]+pmt*20, xmax, +kPMTThreshold/kADC_to_pe[pmt]+pmt*20);
      TLine* l_threshold2 = new TLine(xmin, kPMTThreshold/kADC_to_pe[pmt]+pmt*20, xmax, kPMTThreshold/kADC_to_pe[pmt]+pmt*20);
      //pmt_lines->Add(l_threshold1);
      pmt_lines->Add(l_threshold2);
      //l_threshold1->SetLineColor(kBlue);
      l_threshold2->SetLineColor(kBlue);
      //l_threshold1->SetLineStyle(3);
      l_threshold2->SetLineStyle(3);
      //l_threshold1->Draw("same");
      l_threshold2->Draw("same");
      
      //TLine* l_time = new TLine(pmt_time[pmt][0], ymin, pmt_time[pmt][0], ymax);
      //pmt_lines->Add(l_time);
      //l_time->SetLineColor(kSpring);
      //l_time->SetLineStyle(3);
      //l_time->Draw("same");
    }    
  }

  // FFT
  /*TCanvas* c = new TCanvas();
  Complex waveform[fNSamples];
  RemoveADCOffset(hSum);
  //FFTFilter(hSum, -1);
  TH1F* h_re = new TH1F("FFT_re","FFT;i (122 kHz);Amp.",fNSamples,-1.*fNSamples/2,fNSamples/2);
  TH1F* h_im = new TH1F("FFT_im","FFT;i (122 kHz);Amp.",fNSamples,-1.*fNSamples/2,fNSamples/2);
  TH1F* h_abs = new TH1F("FFT","FFT;i (122 kHz);Amp.",fNSamples,-1.*fNSamples/2,fNSamples/2);
  h_im->SetLineColor(kRed+2);
  h_abs->SetLineColor(kBlack);
  pmt_hists->Add(h_re);
  pmt_hists->Add(h_im);
  pmt_hists->Add(h_abs);
  for( Int_t i = 0; i < (Int_t)fNSamples; i++ )
    waveform[i] = hSum->GetBinContent(i+1);
  CArray fft(waveform, fNSamples);
  FFT(fft);
  for( Int_t i = 0; i < (Int_t)fNSamples/2; i++ ) {
    h_re->SetBinContent(i+1,std::real(fft[i+fNSamples/2]));
    h_im->SetBinContent(i+1,std::imag(fft[i+fNSamples/2]));
    h_abs->SetBinContent(i+1,std::abs(fft[i+fNSamples/2]));
  }
  for( Int_t i = (Int_t)fNSamples/2;  i < (Int_t)fNSamples; i++ ) {
    h_re->SetBinContent(i+1,std::real(fft[i-fNSamples/2]));
    h_im->SetBinContent(i+1,std::imag(fft[i-fNSamples/2]));
    h_abs->SetBinContent(i+1,std::abs(fft[i-fNSamples/2]));
  }

  c->cd();
  h_abs->Draw("l");
  h_re->Draw("l same");
  h_im->Draw("l same");
  c->Update();
  RemoveADCOffset(hSum,-50);  */

  fCanvas->Update();

  if( !fCanvas->IsBatch() ) {
    std::cout << "Press any key to continue..." << std::endl;
    char* s = new char[1];
    gets(s);
  }

  pmt_hists->Delete();
  pmt_lines->Delete();
  //c->Close();
}
