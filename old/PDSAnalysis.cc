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

#include "PDSAnalysis.h"

//using namespace TMath;

const Int_t    PDSAnalysis::trigger        = 848;
const Double_t PDSAnalysis::sampleRate     = 250000000.;
const Double_t PDSAnalysis::rfThreshold    = 2050;
const Double_t PDSAnalysis::pmtSumThreshold= -8.;
const Double_t PDSAnalysis::pmtSingleThreshold=-1.5;
const Int_t    PDSAnalysis::beamWindow     = 800;
const Int_t    PDSAnalysis::triggerWindow  = 10;
const Double_t PDSAnalysis::beamPulseWidth = 625e-6;
const Double_t PDSAnalysis::tpcGateWidth   = 4e-3;
const Double_t PDSAnalysis::ADC_to_pe      =  -1.;
const Double_t PDSAnalysis::tick_to_ns     =  4.0/1.0;

PDSAnalysis::PDSAnalysis(TString fiName, UInt_t runNum, TString foName) 
{
  std::cout << "New analysis\t" << runNum << std::endl;
  //gStyle->SetOptStat(0);
  c = new TCanvas("c","Viewer");
  c->SetGridx(); c->SetGridy();

  runno = runNum;
  std::cout << "Analyzing run number " << runno << "..." << std::endl;

  pdsTree = ImportTree(fiName,TString("pmt_tree"));
  TFile* fo = new TFile(foName,"RECREATE");
  TTree* newTree = SetupNewTree();

  Loop(pdsTree, newTree);

  PrintInfo();

  newTree->Write();
  newTree->Delete();

  //fo->Close(); 
}

PDSAnalysis::~PDSAnalysis()
{
  std::cout << "Cleaning up..." << std::endl;
  c->Close();
}

TTree* PDSAnalysis::ImportTree(TString fiName, TString treeName)
{
  std::cout << "Importing tree from " << fiName << "..." << std::endl;
  TFile* fi = new TFile(fiName);
  if(!fi) return NULL;
  
  TTree* tree = (TTree*)(fi->Get(treeName));
  if(!tree) return NULL;
  
  tree->SetBranchAddress("event_number", &number);
  
  tree->SetBranchAddress("digitizer_waveforms", &waveforms);
  
  tree->SetBranchAddress("gps_ctrlFlag", &time.gps_flag);
  tree->SetBranchAddress("gps_Year",     &time.gps_y);
  tree->SetBranchAddress("gps_daysIntoYear", &time.gps_d);
  tree->SetBranchAddress("gps_secIntoDay",   &time.gps_s);
  tree->SetBranchAddress("gps_nsIntoSec",    &time.gps_ns);
  
  tree->SetBranchAddress("computer_secIntoEpoch", &time.comp_s);
  tree->SetBranchAddress("computer_nsIntoSec",    &time.comp_ns);
  
  tree->SetBranchAddress("nSamples", &nSamples);
  
  tree->SetBranchAddress("digitizer_time", &time.digit_time);
  
  //tree->SetDirectory(0);                                                 
  //fi->Close();                                                                      

  return tree;
}

TTree* PDSAnalysis::SetupNewTree()
{
  std::cout << "Creating new tree..." << std::endl;
  TTree* tree = new TTree("pdsEvTree","pdsEvTree");
    
  tree->Branch("runno",    &runno,    "runno/i");
  tree->Branch("evno_tpc", &evno_tpc, "evno_tpc/I");
  tree->Branch("nevent",   &nevent,   "nevent/i");
  tree->Branch("gps_yr",   &gps_yr,   "gps_yr/s");
  tree->Branch("gps_d",    &gps_d,    "gps_d/s");
  tree->Branch("gps_s",    &gps_s,    "gps_s/i");
  tree->Branch("gps_ns",   &gps_ns,   "gps_ns/i");

  tree->Branch("evno_pds",  evno_pds,  "evno_pds[nevent]/I");
  tree->Branch("time_interp", time_interp, "time_interp[nevent]/D");
  tree->Branch("peak",      peak,      "peak[nevent]/D");
  tree->Branch("pmt_peak",  pmt_peak,  "pmt_peak[nevent][15]/D");
  tree->Branch("peak_time", peak_time, "peak_time[nevent]/D");
  tree->Branch("pmt_peak_time", pmt_peak_time, "pmt_peak_time[nevent][15]/D");
  tree->Branch("pmt_time_interp", pmt_time_interp, "pmt_time_interp[nevent][15]/D");
  tree->Branch("pmt_dtime", pmt_dtime, "pmt_dtime[nevent][15][15]/D");
  tree->Branch("integral",  integral,  "integral[nevent]/D");
  tree->Branch("pmt_integral", pmt_integral, "pmt_integral[nevent][15]/D");
  tree->Branch("rf_time",   rf_time,   "rf_time[nevent]/D");

  tree->Branch("isBeamTrigger", isBeamTrigger, "isBeamTrigger[nevent]/O");
  tree->Branch("inBeamWindow",  inBeamWindow,  "inBeamWindow[nevent]/O");

  tree->Branch("pds_time",  pds_time,  "pds_time[nevent]/D");
  tree->Branch("timeWt",    timeWt,    "timeWt[nevent]/D");
  
  return tree;
}

void PDSAnalysis::Loop(TTree* pdsTree, TTree* outTree)
{
  Bool_t inBeamWindow_prev = false;
  tpcTriggerTime = -1;
  
  std::cout << "Starting loop..." << std::endl;

  Int_t eventNumber = 0;
  for( int i = 0; i < pdsTree->GetEntries(); i++) {
    if( i%(pdsTree->GetEntries()/10) == 0 )
      std::cout << "PDS ev# " << i
		<< "/" << pdsTree->GetEntriesFast() - 1 
		<< "\t TPC ev# " << eventNumber << std::endl;
    
    if( pdsTree->GetEntry(i) ) {
      // DrawEvent(i);
      Bool_t inBeamWindow_curr = (trigger - CheckBeamWindow(trigger) > 0 && CheckBeamWindow(trigger) > 0);
      //if( !inBeamWindow_prev && inBeamWindow_curr ) {
      if( (time.gps_s > 0) && inBeamWindow_curr && !inBeamWindow_prev ) {
	//std::cout<< "TPC TRIG" << std::endl;
	eventNumber++;
	evno_tpc = eventNumber;
	tpcTriggerTime = time.comp_ns * 1e-9 + time.comp_s;
	DoEventAnalysis(i);
	inBeamWindow_prev = inBeamWindow_curr;
      } else {
	//std::cout << "PDS TRIG" << std::endl;
	inBeamWindow_prev = inBeamWindow_curr;
	continue;
      }
      
      outTree->Fill();
    }
  }
  
  // no tpc trigger found -> do cosmic analysis
  if( eventNumber == 0) {
    std::cout << "No tpc triggers found. Re-analyzing without tpc event grouping..." << std::endl;
    Int_t bundleno = 0;
    Int_t i        = 0;
    do {
      Int_t j = 0;
      bundleno++;
      nevent   = 0;
      evno_tpc = 0;
      tpcTriggerTime = -9999;
      gps_yr = time.gps_y;
      gps_d  = time.gps_d;
      gps_s  = time.gps_s;
      gps_ns = time.gps_ns;
      
      while( j < 1000 && pdsTree->GetEntry(i+j) ) {
	if( (i+j)%(pdsTree->GetEntries()/10) == 0 ) {
	  std::cout << "PDS ev# " << i+j
		    << "/" << pdsTree->GetEntriesFast()
		    << "\t bundle# " << bundleno << std::endl;
	}	
	
	TH1F* hPmtSum      = PMTSum();
	std::vector<TH1F*> hPmt = PMTHists();
	RemoveADCOffset( hPmtSum );
	evno_pds[j] = i + j;
	
	peak_time[j]   = FindPeak( hPmtSum );
	time_interp[j] = TriggerOffset( hPmtSum, peak_time[j], pmtSumThreshold ) * tick_to_ns;
	peak[j]        = hPmtSum -> GetBinContent( peak_time[j] ) * ADC_to_pe;
	integral[j]    = Integral( hPmtSum, peak_time[j], pmtSumThreshold ) * tick_to_ns * ADC_to_pe;
	
	peak_time[j] = peak_time[j] * tick_to_ns;
	
	for( UInt_t pmt = 0; pmt < (nPMTs-1)*nBoards; pmt++ ) {
	  RemoveADCOffset( hPmt[pmt] );
	  pmt_peak_time[j][pmt] = FindPeak( hPmt[pmt] );
	  pmt_time_interp[j][pmt] = TriggerOffset( hPmt[pmt], pmt_peak_time[j][pmt], pmtSingleThreshold ) * tick_to_ns;
	  pmt_peak[j][pmt]      = hPmt[pmt] -> GetBinContent( pmt_peak_time[j][pmt] ) * ADC_to_pe;
	  pmt_integral[j][pmt]  = Integral( hPmt[pmt], pmt_peak_time[j][pmt], pmtSingleThreshold ) * tick_to_ns * ADC_to_pe;
	  
	  pmt_peak_time[j][pmt] = pmt_peak_time[j][pmt] * tick_to_ns;
	  
	  hPmt[pmt]->Delete();
	}
	for( UInt_t pmt_i = 0; pmt_i < (nPMTs-1)*nBoards; pmt_i++ ) {
	  for( UInt_t pmt_j = 0; pmt_j < (nPMTs-1)*nBoards; pmt_j++ ) {
	    pmt_dtime[j][pmt_i][pmt_j] = pmt_time_interp[j][pmt_i] - pmt_time_interp[j][pmt_j];
	  }
	}
	
	inBeamWindow[j]  = false;
	isBeamTrigger[j] = false;
	
	rf_time[j]  = -9999;
	timeWt[j]   = 1.;
	pds_time[j] = time.comp_ns * 1e-9 + time.comp_s;
	
	nevent++;
	
	j++;
	hPmtSum->Delete();
      }
      i = i+j;
      outTree->Fill();
    } while( pdsTree->GetEntry(i) );
  }
}

void PDSAnalysis::PrintInfo()
{
  std::cout << "Done with analysis!" << std::endl;
  return;
}

Bool_t PDSAnalysis::IsEvent()
{
  return true;
  /*                                                                                    
    Double_t pmtSum[maxTime] = PMTSum();
    for( int time = 0; time < triggerWindow; time++) {                                         
    if( pmtSum[trigger+time] < pmtThreshold )                                                
    return true;                                                                             
    if( pmtSum[trigger-time] < pmtThreshold )                                              
    return true;                                                                            
    }                                                                                      
    return false;*/
}

void PDSAnalysis::DoEventAnalysis(Int_t i)
{
  gps_yr = time.gps_y;
  gps_d  = time.gps_d;
  gps_s  = time.gps_s;
  gps_ns = time.gps_ns;
  
  nevent = 0;
  Int_t j = 0;
  Bool_t inBeamWindow_prev = true;
  Bool_t inBeamWindow_curr = true;
  do {
    TH1F* hPmtSum = PMTSum();
    std::vector<TH1F*> hPmt = PMTHists();
    RemoveADCOffset( hPmtSum );
    evno_pds[j] = i + j;
    
    peak_time[j] = FindPeak( hPmtSum );
    
    inBeamWindow[j] = inBeamWindow_curr;
    if( time.gps_s > 0 ) isBeamTrigger[j] = true;
    else                 isBeamTrigger[j] = false;
    
    if( inBeamWindow[j] )
      rf_time[j] = CheckBeamWindow( peak_time[j] );
    else
      rf_time[j] = -9999.;
    
    if( inBeamWindow[j] ) timeWt[j] = 1./beamPulseWidth;
    else                  timeWt[j] = 1./(tpcGateWidth - beamPulseWidth);
    
    time_interp[j] = TriggerOffset( hPmtSum, peak_time[j], pmtSumThreshold ) * tick_to_ns; 
    peak[j]      = hPmtSum -> GetBinContent( peak_time[j] ) * ADC_to_pe;
    integral[j]  = Integral( hPmtSum, peak_time[j], pmtSumThreshold ) * ADC_to_pe * tick_to_ns;
    
    peak_time[j] = peak_time[j] * tick_to_ns;

    //std::cout << "Analyzing individual pmts..." << std::endl;
    
    for( UInt_t pmt = 0; pmt < (nPMTs-1)*nBoards; pmt++ ) {
      RemoveADCOffset( hPmt[pmt] );
      pmt_peak_time[j][pmt] = FindPeak( hPmt[pmt] );
      pmt_time_interp[j][pmt] = TriggerOffset( hPmt[pmt], pmt_peak_time[j][pmt], pmtSingleThreshold ) * tick_to_ns;
      pmt_peak[j][pmt]      = hPmt[pmt] -> GetBinContent( pmt_peak_time[j][pmt] ) * ADC_to_pe;
      pmt_integral[j][pmt]  = Integral( hPmt[pmt], pmt_peak_time[j][pmt], pmtSingleThreshold ) * ADC_to_pe * tick_to_ns;
      
      pmt_peak_time[j][pmt] = pmt_peak_time[j][pmt] * tick_to_ns;
      hPmt[pmt]->Delete();
    }
    for( UInt_t pmt_i = 0; pmt_i < (nPMTs-1)*nBoards; pmt_i++ ) {
      for( UInt_t pmt_j = 0; pmt_j < (nPMTs-1)*nBoards; pmt_j++ ) {
	pmt_dtime[j][pmt_i][pmt_j] = pmt_time_interp[j][pmt_i] - pmt_time_interp[j][pmt_j];
      }
    }

    //std::cout << "Done!" << std::endl;
    
    if( tpcTriggerTime <= 0 )
      pds_time[j] = -9999;
    else
      pds_time[j] = time.comp_s + time.comp_ns * 1e-9 - tpcTriggerTime;
    nevent++;
    
    //cout << "tpc_ev#: " << i << "\t" << "pds_ev#: " << j << endl;
    //cout << inBeamWindow_curr << inBeamWindow_prev << endl;
    //cout << "Time (s): " << time.comp_s << "." << Form("%09d",time.comp_ns) << endl;
    
    j++;
    pdsTree->GetEvent(i + j);
    inBeamWindow_prev = inBeamWindow_curr;
    inBeamWindow_curr = (trigger - CheckBeamWindow(trigger) > 0 && CheckBeamWindow(trigger) > 0);
    
    hPmtSum->Delete();

    //Bool_t cont = ( !( !inBeamWindow_prev && inBeamWindow_curr ) && pdsTree->GetEvent(i + j) );
    //std::cout << "CONTINUE?: " << cont << std::endl;
  } while ( !( !inBeamWindow_prev && inBeamWindow_curr ) && pdsTree->GetEvent(i + j));
  //} while ( inBeamWindow_curr || ( time.gps_s <= 0.0 ) && pdsTree->GetEvent( i + j ) );
}

TH1F* PDSAnalysis::PMTSum()
{
  TH1F* hSum = new TH1F("hSum","hSum",maxTime,0,maxTime);
  
  for(UInt_t board = 0; board < nBoards; board++) {
    for(UInt_t pmt = 0; pmt < nPMTs; pmt++) {
      for(UInt_t time = 0; time < maxTime; time++) {
        if( pmt != 5 )
          hSum->Fill(time,waveforms[board][pmt][time]);
      }
    }
  }
  return hSum;
}

std::vector<TH1F*> PDSAnalysis::PMTHists()
{
  std::vector<TH1F*> hists;
  for( UInt_t board = 0; board < nBoards; board++ ) {
    for( UInt_t pmt = 0; pmt < nPMTs-1; pmt++ ) {
      TH1F* h = new TH1F(Form("hPMT_b%d_p%d",board,pmt),Form("hPMT_b%d_p%d",board,pmt),maxTime,0,maxTime);
      for( UInt_t time = 0; time < maxTime; time ++ ) {
	h->Fill(time,waveforms[board][pmt][time]);
      }
      hists.push_back(h);
    }
  }
  return hists;
}

void PDSAnalysis::RemoveADCOffset(TH1F* h)
{
  Double_t offset = 0;
  for( int time = 2; time < 102; time++ ) {
    offset += h->GetBinContent(time)/100.;
  }
  
  for( int i = 0; i < h->GetNbinsX()+1; i++ ) {
    h->SetBinContent(i, h->GetBinContent(i) - offset);
  }
}

Double_t PDSAnalysis::Integral(TH1F* h, Int_t peak, Double_t pmtThreshold)
{
  Double_t integral0 = 0;
  Double_t integral1 = 0;
  UInt_t time_low  = peak;
  UInt_t time_high = peak;
  for( UInt_t time = peak; time > 0; time-- ) {
    integral0 += h->GetBinContent(time);
    if( h->GetBinContent(time) > pmtThreshold ) {
      time_low = time;
      break;
    }
  }
  for( UInt_t time = peak+1; time < nSamples; time++) {
    integral0 += h->GetBinContent(time);
    if( h->GetBinContent(time) > pmtThreshold ) {
      time_high = time;
      break;
    }
  }
  
  return integral0;
}

Int_t PDSAnalysis::FindPeak(TH1F* h)
{
  // note that the peak is actually the minimum                                                   
  Int_t peak = trigger;
  for(Int_t time = -200 + trigger; time < 200 + trigger; time++) {
    if( h->GetBinContent(  time ) < h->GetBinContent( peak ) ) 
      peak =  time;
  }
  return peak;
}

Double_t PDSAnalysis::TriggerOffset(TH1F* h, Int_t peak, Double_t pmtThreshold)
{
  //std::cout << "Interpolating trigger time..." << std::endl;
  // backward search for threshold crossing point
  Double_t interpolatedTime = -9999.;

  for( Int_t time = peak; time > peak - 50; time-- ) {
    if( h->GetBinContent( time ) > pmtThreshold ) { 
      UInt_t time_arr[] = { time, time+1, time+2 };
      return RecursiveQuadInter( h, time_arr, pmtThreshold );
    }
  }
  //std::cout << "Failed!" << std::endl;
  //cout << interpolatedTime << "\t" << peak_time << "\t" << trigger << endl;
  return interpolatedTime;
}

Double_t PDSAnalysis::RecursiveQuadInter(TH1F* h, UInt_t time[3], Double_t p) 
{
  if( time[0] < 1 || time[3] > nSamples )
    return -9999.;

  if( h->GetBinContent(time[0]) == h->GetBinContent(time[1]) ||
      h->GetBinContent(time[1]) == h->GetBinContent(time[2]) ||
      h->GetBinContent(time[2]) == h->GetBinContent(time[0]) ) {
    UInt_t new_time[] = { time[0]+1, time[1]+1, time[2]+1 };
    return RecursiveQuadInter(h, new_time, p);
  }

  Double_t sum = 0;
  for( Int_t i = 0; i < 3; i++ ) {
    Double_t mult = time[i];
    for( Int_t j = 0; j < 3; j++ ) {
      if( j != i )
	mult = mult * ( p - h->GetBinContent(time[j]) )/( h->GetBinContent(time[i]) - h->GetBinContent(time[j]) );
    }
    sum += mult;
  }
  if( sum > nSamples )
    std::cout << "ERROR!" << "\n"
	      << "Current event has a start time of " << sum * tick_to_ns / 1000. << "us!" 
	      << std::endl;
  return sum;
}

Double_t PDSAnalysis::CheckBeamWindow(Int_t peak)
{
  Bool_t ascending = false;
  Bool_t descending = false;
  Double_t rfTime = -9999;
  std::vector<Double_t> mean;
  mean.push_back(0.); mean.push_back(0.); mean.push_back(0.);
  // Forward search
  Int_t preWindow;
  if( peak == trigger )
    preWindow = trigger;
  else
    preWindow = 0;
  
  for( int time = peak - preWindow; time < peak + beamWindow; time++) {
    mean[0] = 0.;
    for( UInt_t board = 0; board < nBoards; board++ ) {
      mean[0] += waveforms[board][5][time]/nBoards;
    }
    
    if( mean[0] <= rfThreshold && mean[1] > rfThreshold ) {
      if( mean[0] != mean[1] )
	return time-1 + (rfThreshold - mean[1])/(mean[0] - mean[1]); // linear interpolation
      else
	return time-2 + 2*(rfThreshold - mean[2])/(mean[0] - mean[2]);
      break;
    }
    
    mean[2] = mean[1];
    mean[1] = mean[0];
  }

  //std::cout << rfTime << std::endl;                                          
  return rfTime;
}

void PDSAnalysis::DrawEvent(Int_t i)
{
  std::cout << "TPC EVENT#:" << evno_tpc << "\t"
	    << "PDS EVENT#:" << i << "\t" 
	    << "TIME:" << time.gps_s << "." << time.gps_ns << std::endl;

  THStack* s = new THStack(Form("hs_TPC%d-PDS%d",evno_tpc,i), Form("hs_TPC%d-PDS%d",evno_tpc,i));
  for( UInt_t board = 0; board < nBoards; board++ ) {
    for( UInt_t pmt = 0; pmt < nPMTs; pmt++ ) {
      TString name = Form("hTemp_%d-%d",board,pmt);
      TH1F* tmpHist = new TH1F(name,name,nSamples,0,nSamples);
      for( UInt_t sample = 0; sample < nSamples; sample++ ) {
	tmpHist -> Fill(sample,waveforms[board][pmt][sample]);
      }
      RemoveADCOffset(tmpHist);
      tmpHist->GetYaxis()->SetRangeUser(-100,50);
      if( pmt == 5 ) tmpHist->SetLineColor(kRed + 2);
      s->Add(tmpHist);
    }
  }
  c->cd();
  s->Draw("nostack");

  TH1F* hPmtSum = PMTSum();
  RemoveADCOffset( hPmtSum );
  hPmtSum->SetLineColor(kGreen+2);
  hPmtSum->Draw("same");

  std::cout << "PEAK:" << hPmtSum->GetBinContent(FindPeak(hPmtSum)) << "\n"
	    << "TRIG:" << TriggerOffset(hPmtSum, FindPeak(hPmtSum), pmtSumThreshold)
	    << std::endl;

  c->Update();

  TList* hists = s->GetHists();
  hists->SetOwner(true);
  hists->Delete();
  s->Delete();
  hPmtSum->Delete();
}
