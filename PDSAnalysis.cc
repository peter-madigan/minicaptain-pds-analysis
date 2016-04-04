#include <sstream>
#include <unistd.h>
#include <iostream>
#include <vector>

#include <Rtypes.h>
#include <TTree.h>
#include <TString.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TFile.h>
#include <THStack.h>
#include <TMath.h>
#include <TLine.h>

#include "PDSAnalysis.h"

using namespace TMath;

const Int_t    PDSAnalysis::kTrigger        = 848;
const Double_t PDSAnalysis::kSampleRate     = 250000000.;

const Double_t PDSAnalysis::kSumThreshold      = 2.00; // pe
const Double_t PDSAnalysis::kRFThreshold       = 50.0; // ADC
const Double_t PDSAnalysis::kPMTThreshold      = 0.50; // pe
const Double_t PDSAnalysis::kIntegralThreshold = 10.0; // ADC ticks

const Int_t    PDSAnalysis::kPeakSearchWindow_pre  = 150; // ticks vvv
const Int_t    PDSAnalysis::kPeakSearchWindow_post = 400; // 1600ns Ar triplet lifetime
const Int_t    PDSAnalysis::kBeamSearchWindow_post = 400;
const Int_t    PDSAnalysis::kBeamSearchWindow_pre  = 0;

const Double_t PDSAnalysis::kBeamPulseWidth = 625e-6;
const Double_t PDSAnalysis::kTPCGateWidth   = 4e-3;

const Double_t PDSAnalysis::kTick_to_ns     =  4.0/1.0;
void PDSAnalysis::InitializeADC_to_pe() {
  kADC_to_pe.push_back(-1.); // Dead pmt
  kADC_to_pe.push_back(-1./6.421);
  kADC_to_pe.push_back(-1./5.777);
  kADC_to_pe.push_back(-1./6.343);
  kADC_to_pe.push_back(-1./7.541);

  kADC_to_pe.push_back(-1./4.592);
  kADC_to_pe.push_back(-1./5.513);
  kADC_to_pe.push_back(-1./4.187);
  kADC_to_pe.push_back(-1./7.363);
  kADC_to_pe.push_back(-1./7.482);

  kADC_to_pe.push_back(-1./5.908);
  kADC_to_pe.push_back(-1./5.625);
  kADC_to_pe.push_back(-1./7.522);
  kADC_to_pe.push_back(-1./6.101);
  kADC_to_pe.push_back(-1./6.964);

  if( fCalibration )
    for( UInt_t pmt = 0; pmt < kNPMTs; pmt++ )
      kADC_to_pe[pmt] = 1.0;
}

PDSAnalysis::PDSAnalysis(TString fiName, UInt_t runNum, TString foName, Bool_t CalibrationMode, Bool_t ViewerMode) 
{
  std::cout << "New analysis: " << runNum << std::endl;
  fCalibration = CalibrationMode;
  fViewerMode  = ViewerMode;
  
  if( fViewerMode ) {
    std::cout << "Running in viewer mode..." << std::endl;
    fCanvas = new TCanvas("fCanvas","Event viewer");
  }

  if( fCalibration )
    std::cout << "Running in calibration mode..." << std::endl;
  
  runno = runNum;
  std::cout << "Analyzing run number " << runno << "..." << std::endl;

  fPMTTree = ImportTree(fiName);
  fAnalysisTree = SetupNewTree(foName);
  InitializeADC_to_pe();
  
  Loop();
  
  fAnalysisTree->Write();
  
  std::cout << "Analysis complete!" << std::endl;
}

PDSAnalysis::~PDSAnalysis()
{
  std::cout << "Cleaning up..." << std::endl;
  if( fViewerMode )
    fCanvas->Close();
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
    
  tree->Branch("runno",    &runno,    "runno/i");
  tree->Branch("tpc_evno", &tpc_evno, "tpc_evno/I");
  tree->Branch("pds_nevent", &pds_nevent, "pds_nevent/i");
  tree->Branch("gps_yr",   &gps_yr,   "gps_yr/s");
  tree->Branch("gps_d",    &gps_d,    "gps_d/s");
  tree->Branch("gps_s",    &gps_s,    "gps_s/i");
  tree->Branch("gps_ns",   &gps_ns,   "gps_ns/i");

  tree->Branch("pds_evno", pds_evno, "pds_evno[pds_nevent]/I");
  tree->Branch("pds_time", pds_time, "pds_time[pds_nevent]/D");
  tree->Branch("pds_peak", pds_peak, "pds_peak[pds_nevent]/D");
  tree->Branch("pds_integral", pds_integral, "pds_integral[pds_nevent]/D");
  tree->Branch("pds_offset",   pds_offset,   "pds_offset[pds_nevent]/D");
  tree->Branch("pds_flag", pds_flag, "pds_flag[pds_nevent]/O");

  tree->Branch("pmt_time", pmt_time, "pmt_time[pds_nevent][15]/D");
  tree->Branch("pmt_peak", pmt_peak, "pmt_peak[pds_nevent][15]/D");
  tree->Branch("pmt_integral", pmt_integral, "pmt_integral[pds_nevent][15]/D");
  tree->Branch("pmt_dtime",    pmt_dtime,    "pmt_dtime[pds_nevent][15][15]/D");
  tree->Branch("pmt_offset",   pmt_offset,   "pmt_offset[pds_nevent][15]/D");
  tree->Branch("pmt_flag", pmt_flag, "pmt_flag[pds_nevent][15]/O");

  tree->Branch("rf_time", rf_time, "rf_time[pds_nevent]/D");
  tree->Branch("isBeamTrigger", isBeamTrigger, "isBeamTrigger[pds_nevent]/O");
  tree->Branch("inBeamWindow",  inBeamWindow,  "inBeamWindow[pds_nevent]/O");
  
  tree->Branch("timeWt",    timeWt,    "timeWt[pds_nevent]/D");
  
  return tree;
}

void PDSAnalysis::Loop()
{
  std::cout << "Starting loop..." << std::endl;

  Bool_t TPC0 = false;
  Bool_t TPC1 = false;

  // Loop over all events in tree
  tpc_evno = 0;
  UInt_t i = 0; // TPC start variable
  UInt_t j = 1; // TPC end variable
  if( !fCalibration ) {
    while( fPMTTree->GetEntry(i) ) {
      // Check if current event has a gps signature
      TPC0 = (fGPS_s + fGPS_ns > 0);
      if( TPC0 ) {
	// Check if next event has a gps signature
	j = i+1;
	fPMTTree->GetEntry(j);
	TPC1 = (fGPS_s + fGPS_ns > 0);
	if( TPC1 ) {
	  // New TPC trigger
	  tpc_evno++;
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
	  if( j>i ) DoEventAnalysis(i,j);
	  else { i++; continue; }
	  
	  // Save
	  fAnalysisTree->Fill();
	  i++;
	} else i++;
      } else i++;  
    }
  }
  // No tpc trigger found -> do cosmic analysis
  if( tpc_evno == 0 ) {
    if( !fCalibration )
      std::cout << "No tpc triggers found. Re-analyzing without tpc event grouping..." << std::endl;
    else
      std::cout << "Looping in calibration mode..." << std::endl;
    while( fPMTTree->GetEntry(i) ) {
      // Do single event analysis
      DoEventAnalysis(i, i);

      // Save
      fAnalysisTree->Fill();
      i++;
    }
  }
}

void PDSAnalysis::DoEventAnalysis(Int_t start, Int_t end)
{
  fPMTTree->GetEntry(start);
  gps_yr = fGPS_y;
  gps_d  = fGPS_d;
  gps_s  = fGPS_s;
  gps_ns = fGPS_ns;
  
  pds_nevent = end - start + 1;
  UInt_t subevent = 0;
  while( fPMTTree->GetEntry(start + subevent) && subevent < pds_nevent ) {
    // Do individual pmt analysis
    for( UInt_t pmt = 0; pmt < kNPMTs; pmt++ ) {
      DoPMTAnalysis(subevent, pmt);
    }

    // Do overall analysis
    pds_evno[subevent] = start + subevent;
    DoPDSAnalysis(subevent);

    // Draw event
    if( pds_evno[subevent] % (fPMTTree->GetEntries()/10)==0 )
      PrintEvent(subevent);
    if( fViewerMode ) {
      PrintEvent(subevent);
      DrawEvent(subevent);
    }

    // Convert units
    ConvertUnits(subevent);
    
    // Ready for next event
    subevent++;
  }
}

void PDSAnalysis::DoPMTAnalysis(Int_t subevent, Int_t pmt) 
{
  // Create PMT histogram
  TH1F* hPMT = GetPMT(pmt);
  pmt_offset[subevent][pmt] = RemoveADCOffset(hPMT);
  
  // Find peaks in histogram
  std::vector<Int_t> peak_time = FindPeaks(hPMT, pmt);
  if( peak_time[0] != -9999 ) {
    pmt_peak[subevent][pmt] = hPMT->GetBinContent(peak_time[0]);
    pmt_time[subevent][pmt] = FindEvTime(hPMT, peak_time[0]);
    if( fCalibration )
      pmt_integral[subevent][pmt] = TotalIntegral(hPMT, peak_time);
    else
      pmt_integral[subevent][pmt] = NegativeIntegral(hPMT, peak_time);
  } else {
    pmt_peak[subevent][pmt] = -9999;
    pmt_time[subevent][pmt] = -9999; 
    pmt_integral[subevent][pmt] = -9999;
  }

  // Flag if not noise
  pmt_flag[subevent][pmt] = IsPMTEvent(hPMT, subevent, pmt, peak_time);

  hPMT->Delete();
}

void PDSAnalysis::DoPDSAnalysis(Int_t subevent) {
  // Create PMT histogram        
  TH1F* hPMT = GetPMTSum();
  pds_offset[subevent] = RemoveADCOffset(hPMT);

  // Find peaks in histogram   
  std::vector<Int_t> peak_time = FindPeaks(hPMT, -1);
  if( peak_time[0] != -9999 ) {
    pds_peak[subevent] = hPMT->GetBinContent(peak_time[0]);
    pds_time[subevent] = FindEvTime(hPMT, peak_time[0]);
    if( fCalibration )
      pds_integral[subevent] = TotalIntegral(hPMT, peak_time);
    else
      pds_integral[subevent] = NegativeIntegral(hPMT, peak_time);
  } else {
    pds_peak[subevent] = -9999;
    pds_time[subevent] = -9999;
    pds_integral[subevent] = -9999;
  }

  // Flag if not noise 
  pds_flag[subevent] = IsPDSEvent(hPMT, subevent, peak_time);

  // Check beam
  TH1F* hRF = GetRFMean();
  RemoveADCOffset(hRF);
  rf_time[subevent] = FindRFTime(hRF, Floor(pds_time[subevent]));
  inBeamWindow[subevent]  = !(rf_time[subevent] == -9999) || isBeamTrigger[subevent];
  isBeamTrigger[subevent] = (subevent == 0);

  // Time weighting
  if( inBeamWindow[subevent] )
    timeWt[subevent] = 1./kBeamPulseWidth;
  else if( tpc_evno == 0 )
    timeWt[subevent] = 1./((kPeakSearchWindow_pre + kPeakSearchWindow_post) * kTick_to_ns);
  else
    timeWt[subevent] = 1./(kTPCGateWidth - kBeamPulseWidth);

  // PMT delta t
  for( UInt_t ipmt = 0; ipmt < kNPMTs; ipmt++ )
    for( UInt_t jpmt = 0; jpmt < kNPMTs; jpmt++ )
      if( pmt_time[subevent][ipmt] == -9999 || pmt_time[subevent][jpmt] == -9999 )
	pmt_dtime[subevent][ipmt][jpmt] = -9999;
      else
	pmt_dtime[subevent][ipmt][jpmt] = pmt_time[subevent][ipmt] - pmt_time[subevent][jpmt];

  hPMT->Delete();
  hRF->Delete();
}

Bool_t PDSAnalysis::IsPMTEvent(TH1F* h, Int_t subevent, Int_t pmt, std::vector<Int_t> peak_time) 
{
  if( pmt_time[subevent][pmt] == -9999 ) return false;
  if( pmt_peak[subevent][pmt] == -9999 ) return false;
  if( pmt_integral[subevent][pmt] == -9999 ) return false;
  if( TotalIntegral(h,peak_time) > -kIntegralThreshold ) return false;
  return true;
}

Bool_t PDSAnalysis::IsPDSEvent(TH1F* h, Int_t subevent, std::vector<Int_t> peak_time) 
{
  if( pds_time[subevent] == -9999 ) return false;
  if( pds_peak[subevent] == -9999 ) return false;
  if( pds_integral[subevent] == -9999 ) return false;
  if( TotalIntegral(h,peak_time) > -kIntegralThreshold ) return false;
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

  TString name = Form("hPMT-%d", pmt);
  TH1F* h = new TH1F(name, name, fNSamples, 0, fNSamples);
  for( UInt_t sample = 0; sample < fNSamples; sample++ )
    h->Fill(sample, fDigitizerWaveform[board][channel][sample]);

  return h;
}

TH1F* PDSAnalysis::GetPMTSum() 
{
  TString name = "hPMTSum";
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
    
    for( UInt_t sample = 0; sample < fNSamples; sample++ )
      h->Fill(sample, -fDigitizerWaveform[board][channel][sample] * kADC_to_pe[pmt]);
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
  // returns the offset with a std. dev. < 0.75 ADC or the offset with the smallest std. dev. (the faster of the two)
  Double_t offset = 0.0;
  Double_t offset_min = 0.0; 

  UInt_t n = 25;
  UInt_t start = 1;
  UInt_t end = n + start;

  Double_t sum = n;
  Double_t sumsq = Power(n, 2);
  Double_t stddev = Sqrt(sumsq/n - Power(sum/n,2));
  Double_t stddev_min = Sqrt(sumsq/n - Power(sum/n,2));
  while( stddev > 0.75 && end < fNSamples ) {
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

std::vector<Int_t> PDSAnalysis::FindPeaks(TH1F* h, Int_t pmt) 
{
  // Produces a list of peaks above the threshold
  // First entry is global minimum (in search window)
  
  // Set threshold
  Double_t threshold;
  if( fCalibration )
    threshold = -3;
  else if( pmt < 0 )
    threshold = -kSumThreshold;
  else
    threshold = -kPMTThreshold / kADC_to_pe[pmt];

  // Find global minimum
  std::vector<Int_t> peak_time(1,-9999);
  for( UInt_t sample = kTrigger - kPeakSearchWindow_pre; sample < kTrigger + kPeakSearchWindow_post; sample++ ) {
    if( h->GetBinContent(sample) < threshold ) {
      if( peak_time[0] == -9999 )
	peak_time[0] = sample;
      else if( h->GetBinContent(sample) < h->GetBinContent(peak_time[0]) )
	peak_time[0] = sample;
    }
  }
  
  // Find triplet peaks
  if( peak_time[0] != -9999 ) {
    Double_t local_peak_time = peak_time[0];
    Bool_t ascending = false;
    Bool_t descending = false;
    for( Int_t sample = peak_time[0]; sample < peak_time[0] + kPeakSearchWindow_post; sample++ )
      if( Abs( h->GetBinContent(sample) ) > Abs(threshold) && descending ) {
	// Crossed threshold -> begin tracking local peak
	ascending = true;
	descending = false;
	local_peak_time = sample;
      } else if( Abs( h->GetBinContent(sample) ) < Abs(threshold) && ascending ) { 
	// Crossed threshold -> stop tracking local peak and store
	ascending = false;
	descending = true;
	peak_time.push_back(local_peak_time);
      } else if( ascending && Abs( h->GetBinContent(sample) ) > Abs( h->GetBinContent(local_peak_time) ) ) {
	// New maximum found above threshold
	local_peak_time = sample;
      }
  }
  return peak_time;
}

Double_t PDSAnalysis::FindEvTime(TH1F* h, Int_t peak_time)
{
  Double_t ev_time = -9999;
  
  // Interpolates the 10%-peak time (maximum half width at base = 200ns)
  Double_t peak_10p = h->GetBinContent(peak_time) * 0.10;
  for( Int_t sample = peak_time; sample > peak_time - 50; sample-- ) 
    if( Abs( h->GetBinContent(sample) ) < Abs(peak_10p) ) {
      Double_t dt = h->GetBinWidth(sample)/2;
      Double_t times[] = { h->GetBinLowEdge(sample)+dt, h->GetBinLowEdge(sample+1)+dt, 
			   h->GetBinLowEdge(sample+2)+dt };
      Double_t values[] = { h->GetBinContent(sample), h->GetBinContent(sample+1), 
			    h->GetBinContent(sample+2) };
      ev_time = QuadraticInterpolate( values, times, peak_10p );
      break;
    }
  if( ev_time > fNSamples || ev_time < 1 ) {
    if( ev_time != -9999 )
      std::cout << "ERROR: Event time is " << ev_time << "!" << std::endl;
    return -9999;
  } else
    return ev_time;
}

Double_t PDSAnalysis::FindRFTime(TH1F* h, Int_t ev_time)
{
  Int_t start;
  if( ev_time < 0 || ev_time > fNSamples )
    start = kTrigger;
  else
    start = ev_time;

  // Interpolates the RF-threshold crossing time
  for( Int_t sample = start - kBeamSearchWindow_pre; sample < start + kBeamSearchWindow_post; sample++ ) {
    if( Abs( h->GetBinContent(sample) ) > Abs(kRFThreshold) ) {
      Double_t dt = h->GetBinWidth(sample)/2;
      Double_t times[] = { h->GetBinLowEdge(sample)+dt, h->GetBinLowEdge(sample+1)+dt,
			   h->GetBinLowEdge(sample+2)+dt };
      Double_t values[] = { h->GetBinContent(sample), h->GetBinContent(sample+1),
                            h->GetBinContent(sample+2) };
      return QuadraticInterpolate( values, times, kRFThreshold );
    }
  }
  return -9999;
}

Double_t PDSAnalysis::NegativeIntegral(TH1F* h, std::vector<Int_t> peak_time) 
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

Double_t PDSAnalysis::TotalIntegral(TH1F* h, std::vector<Int_t> peak_time)
{
  // Integrates all peaks
  Double_t integral = 0;
  if( peak_time[0] == -9999 ) return 0;
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
  // Backwards from peak
  while( Abs( h->GetBinContent(sample) ) > Abs(peak_10p) ) {
    integral += h->GetBinContent(sample);
    sample--;
  }
  // Interpolate last bin
  Double_t dt = h->GetBinWidth(sample)/2;
  Double_t times[] = { h->GetBinLowEdge(sample)+dt, h->GetBinLowEdge(sample+1)+dt,
		       h->GetBinLowEdge(sample+2)+dt };
  Double_t values[] = { h->GetBinContent(sample), h->GetBinContent(sample+1),
			h->GetBinContent(sample+2) };
  Double_t x0 = QuadraticInterpolate( values, times, peak_10p );
  Double_t x1 = h->GetBinLowEdge(sample+1)+dt;
  Double_t dx = x1 - x0;
  Double_t y  = (h->GetBinContent(sample+1) + peak_10p)/2;
  integral += y * dx;
  
  // Forwards from peak
  sample = peak_time + 1;
  while( Abs( h->GetBinContent(sample) ) > Abs(peak_10p) ) {
    integral += h->GetBinContent(sample);
    sample++;
  }
  // Interpolate last bin   
  dt = h->GetBinWidth(sample)/2;
  times = { h->GetBinLowEdge(sample)+dt, h->GetBinLowEdge(sample-1)+dt,
	    h->GetBinLowEdge(sample-2)+dt };
  values = { h->GetBinContent(sample), h->GetBinContent(sample-1),
	     h->GetBinContent(sample-2) };
  x1 = QuadraticInterpolate( values, times, peak_10p );
  x0 = h->GetBinLowEdge(sample-1)+dt;
  dx = x1 - x0;
  y  = (h->GetBinContent(sample-1) + peak_10p)/2;
  integral += y * dx;

  return integral;
}

Double_t PDSAnalysis::QuadraticInterpolate(Double_t x[3], Double_t y[3], Double_t p)
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

void PDSAnalysis::ConvertUnits(Int_t subevent)
{
  pds_time[subevent] *= kTick_to_ns;
  pds_peak[subevent] *= -1.;
  pds_integral[subevent] *= kTick_to_ns * -1.;
  rf_time[subevent] *= kTick_to_ns;
  for( UInt_t pmt = 0; pmt < kNPMTs; pmt++ ) {
    pmt_time[subevent][pmt] *= kTick_to_ns;
    pmt_peak[subevent][pmt] *= kADC_to_pe[pmt];
    pmt_integral[subevent][pmt] *= kTick_to_ns * kADC_to_pe[pmt];
    for( UInt_t pmt2 = 0; pmt2 < kNPMTs; pmt2++ )
      pmt_dtime[subevent][pmt][pmt2] *= kTick_to_ns;
  }
}

void PDSAnalysis::PrintEvent(Int_t subevent)
{
  std::cout << "Run#" << runno << "\n"
	    << "TPC Ev#" << tpc_evno << "\t"
	    << "Sub Ev#" << subevent << "\t"
	    << "PDS Ev#" << pds_evno[subevent] << "\n"
	    << "GPS:" << Form("%.14g", gps_s + gps_ns*1e-9) << "\n"
	    << "RF:" << Form("%.6g",rf_time[subevent]) << "\t"
	    << "BEAM?:" << inBeamWindow[subevent] << "\n"
	    << "PMT:  \tTIME: \tPEAK: \tINT:  \tBL:   \tFLAG?:" << "\n"
	    << "PDS" << "\t" << Form("%.6g",pds_time[subevent]) << "\t"
	    << Form("%.5g",pds_peak[subevent]) << "\t"
	    << Form("%.5g",pds_integral[subevent]) << "\t"
	    << Form("%.5g",pds_offset[subevent]) << "\t"
	    << pds_flag[subevent] << "\n";
  for( UInt_t pmt = 0; pmt < kNPMTs; pmt++ )
    std::cout << pmt + 1 << "\t"
	      << Form("%.6g",pmt_time[subevent][pmt]) << "\t"
	      << Form("%.5g",pmt_peak[subevent][pmt]) << "\t"
	      << Form("%.4g",pmt_integral[subevent][pmt]) << "\t"
	      << Form("%.5g",pmt_offset[subevent][pmt]) << "\t"
	      << pmt_flag[subevent][pmt] << "\n";
  std::cout << std::endl;
}

void PDSAnalysis::DrawEvent(Int_t subevent)
{
  fCanvas->cd();
  //fCanvas->SetGridx();
  //fCanvas->SetGridy();

  TObjArray* pmt_hists = new TObjArray();
  TObjArray* pmt_lines = new TObjArray();

  Double_t xmin = pds_time[subevent] - kPeakSearchWindow_pre;
  Double_t xmax = pds_time[subevent] + kPeakSearchWindow_post;
  Double_t ymin = -150;
  Double_t ymax = +150;

  // Draw PDS sum
  TH1F* hSum = GetPMTSum();
  pmt_hists->Add(hSum);
  RemoveADCOffset(hSum,-50);
  hSum->GetXaxis()->SetRangeUser(xmin,xmax);
  hSum->GetYaxis()->SetRangeUser(ymin,ymax);
  hSum->SetTitle(Form("PDS%d-TPC%d-%d",pds_evno[subevent],tpc_evno,subevent));
  if( !pds_flag[subevent] )
    hSum->SetLineColor(kGray + 2);
  else
    hSum->SetLineColor(kBlue + 2);
  hSum->Draw("l");
  
  // Draw PDS lines
  TLine* l_trig = new TLine(kTrigger, ymin, kTrigger, ymax);
  pmt_lines->Add(l_trig);
  l_trig->SetLineColor(kBlack);
  l_trig->SetLineStyle(2);
  l_trig->Draw("same");

  if( pds_flag[subevent] ) {
    TLine* l_time = new TLine(pds_time[subevent], ymin, pds_time[subevent], ymax);
    pmt_lines->Add(l_time);
    l_time->SetLineColor(kSpring + 2);
    l_time->SetLineStyle(2);
    l_time->Draw("same");

    TLine* l_peak = new TLine(xmin, pds_peak[subevent]-50, xmax, pds_peak[subevent]-50);
    pmt_lines->Add(l_peak);
    l_peak->SetLineColor(kViolet + 2);
    l_peak->SetLineStyle(2);
    l_peak->Draw("same");
  }

  // Draw RF mean
  TH1F* hRF = GetRFMean();
  pmt_hists->Add(hRF);
  RemoveADCOffset(hRF,-25);
  hRF->GetXaxis()->SetRangeUser(xmin,xmax);
  hRF->GetYaxis()->SetRangeUser(ymin,ymax);
  if( !inBeamWindow[subevent] )
    hRF->SetLineColor(kGray + 2);
  else
    hRF->SetLineColor(kRed + 2);
  hRF->Draw("l same");
  
  // Draw RF lines
  if( inBeamWindow[subevent] ) {
    TLine* l_time = new TLine(rf_time[subevent], ymin, rf_time[subevent], ymax);
    pmt_lines->Add(l_time);
    l_time->SetLineColor(kGreen + 2);
    l_time->SetLineStyle(2);
    l_time->Draw("same");
  }

  // Draw PMTs
  for( UInt_t pmt = 0; pmt < kNPMTs; pmt++ ) {
    TH1F* h = GetPMT(pmt);
    pmt_hists->Add(h);
    RemoveADCOffset(h,pmt*10);
    h->GetXaxis()->SetRangeUser(xmin,xmax);
    h->GetYaxis()->SetRangeUser(ymin,ymax);
    if( !pmt_flag[subevent][pmt] )
      h->SetLineColor(kGray);
    else
      h->SetLineColor(kBlue);
    h->Draw("l same");

    // Draw PMT lines
    if( pmt_flag[subevent][pmt] ) {
      TLine* l_time = new TLine(pmt_time[subevent][pmt], ymin, pmt_time[subevent][pmt], ymax);
      pmt_lines->Add(l_time);
      l_time->SetLineColor(kSpring);
      l_time->SetLineStyle(3);
      l_time->Draw("same");

      TLine* l_peak = new TLine(xmin, pmt_peak[subevent][pmt]+10*pmt, xmax, pmt_peak[subevent][pmt]+10*pmt);
      pmt_lines->Add(l_peak);
      l_peak->SetLineColor(kViolet);
      l_peak->SetLineStyle(3);
      l_peak->Draw("same");
    }    
  }

  fCanvas->Update();

  if( !fCanvas->IsBatch() ) {
    std::cout << "Press any key to continue..." << std::endl;
    char* s = new char[1];
    gets(s);
  }

  pmt_hists->Delete();
  pmt_lines->Delete();
}
