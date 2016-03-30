// Macro to examine PDS system by event

#include <sstream>
#include <unistd.h>

#include <Rtypes.h>
#include <TTree.h>
#include <TString.h>
#include <TH1.h>
#include <TCanvas.h>


const size_t maxTime  = 2048, nBoards = 3, nPMTs = 8;
const Int_t trigger         = 848;
const Double_t sampleRate   = 250000000.;
const Double_t rfThreshold  = 2150, pmtThreshold = 3900.5-3.9;
const Int_t beamWindow      = -500, triggerWindow = 10;
const Double_t beamPulseWidth = 625e-6, tpcGateWidth = 4e-3;

Double_t tpcTriggerTime;

TTree* pdsTree;
UShort_t waveforms[nBoards][nPMTs][maxTime];
UInt_t   number;

struct pmt_time {
  UShort_t gps_y, gps_d; 
  UInt_t   gps_s, gps_ns;
  Int_t comp_s; Long64_t comp_ns;
  UShort_t gps_flag;
  UInt_t digit_time[3];
} time;

// Event variables
Int_t evno_tpc;
Int_t nevent;
Int_t evno_pds[1000];
Int_t delay[1000];
UShort_t gps_yr, gps_d;
Int_t    gps_s,  gps_ns;
Double_t peak[1000], integral[1000];
Double_t integral_pmt[1000][3][8];
Bool_t inBeamWindow[1000], isBeamTrigger[1000];
Double_t pds_time[1000];
Double_t timeWt[1000];

TCanvas* c;

void PDSAnalyze(TString fiName="outFile_1.root", TString foName="pdsEvTree_.root") 
{
  gStyle->SetOptStat(0);
  c = new TCanvas("c","Viewer");
  c->SetGridx(); c->SetGridy();  
  
  pdsTree = ImportTree(fiName,"pmt_tree");
  TFile* fo = new TFile(foName,"RECREATE");
  TTree* newTree = SetupNewTree();
  
  Loop(pdsTree, newTree);
  
  PrintInfo();
  
  newTree->Write();
  
  //fo->Close();
  c->Close();

  newTree->Delete();
  pdsTree->Delete();
  return;
}

////////////////////////////////////////////////////////////////////////////////
TTree* ImportTree(TString& fiName, TString& treeName)
{
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
  
  tree->SetBranchAddress("digitizer_time", &time.digit_time);
  
  //tree->SetDirectory(0);
  //fi->Close();
 
  return tree;
}

TTree* SetupNewTree() 
{
  TTree* tree = new TTree("pdsEvTree","pdsEvTree");
    
  tree->Branch("evno_tpc", &evno_tpc, "evno_tpc/I");
  tree->Branch("nevent",   &nevent,   "nevent/I");
  tree->Branch("gps_yr",   &gps_yr,   "gps_yr/s");
  tree->Branch("gps_d",    &gps_d,    "gps_d/s");
  tree->Branch("gps_s",    &gps_s,    "gps_s/i");
  tree->Branch("gps_ns",   &gps_ns,   "gps_ns/i");
  
  tree->Branch("evno_pds",  evno_pds,  "evno_pds[nevent]/I");
  tree->Branch("delay",     delay,     "delay[nevent]/I");
  tree->Branch("peak",      peak,      "peak[nevent]/D");
  tree->Branch("integral",  integral,  "integral[nevent]/D");
  tree->Branch("integral_pmt",  integral_pmt,  "integral_pmt[nevent][3][8]/D"); 
  tree->Branch("isBeamTrigger", isBeamTrigger, "isBeamTrigger[nevent]/O");
  tree->Branch("inBeamWindow",  inBeamWindow,  "inBeamWindow[nevent]/O");
  tree->Branch("pds_time",  pds_time,  "pds_time[nevent]/D");
  tree->Branch("timeWt",    timeWt,    "timeWt[nevent]/D");

  return tree;
}

void Loop(TTree* pdsTree, TTree* outTree) {
  Bool_t inBeamWindow_prev = false;
  tpcTriggerTime = -1;
  
  Int_t eventNumber = 0;
  for( int i = 0; i < pdsTree->GetEntries(); i++) {
    if( i%(pdsTree->GetEntries()/10) == 0 ) 
      std::cout << "PDS ev# " << eventNumber 
		<< "/" << pdsTree->GetEntriesFast() << std::endl;
    
    pdsTree->GetEntry(i);
    Bool_t inBeamWindow_curr = (trigger - CheckBeamWindow() > 0 && CheckBeamWindow() > 0);
    if( !inBeamWindow_prev && inBeamWindow_curr ) {
      eventNumber++;
      tpcTriggerTime = time.comp_ns * 1e-9 + time.comp_s;
      DoEventAnalysis(i);
      DrawEvent(i);
    } else {
      inBeamWindow_prev = inBeamWindow_curr;
      continue;
    }
    
    outTree->Fill();
  }
}

void PrintInfo() {
  return;
}

Bool_t IsEvent() 
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

TH1F* PMTSum()
{
  TH1F* hSum = new TH1F("hSum","hSum",maxTime,0,maxTime);
  
  for(int board = 0; board < nBoards; board++) {
    for(int pmt = 0; pmt < nPMTs; pmt++) {
      for(int time = 0; time < maxTime; time++) {
	if( pmt != 5 )
	  hSum->Fill(time,waveforms[board][pmt][time]);
      }
    }
  }
  return hSum;
}

void DoEventAnalysis(Int_t i) 
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
    RemoveADCOffset( hPmtSum );
    evno_pds[j] = i + j;
    peak[j]     = FindPeak( hPmtSum );
    integral[j] = hPmtSum -> Integral( trigger, maxTime );

    inBeamWindow[j] = inBeamWindow_curr;
    if( j == 0 ) isBeamTrigger[j] = true;
    else         isBeamTrigger[j] = false;
    
    if( inBeamWindow[j] ) delay[j] = trigger - CheckBeamWindow();
    else                  delay[j] = -9999;
    
    if( inBeamWindow[j] ) timeWt[j] = 1./beamPulseWidth;
    else                  timeWt[j] = 1./(tpcGateWidth - beamPulseWidth);

    if( tpcTriggerTime <= 0 )                
      pds_time[j] = -9999;
    else
      pds_time[j] = time.comp_s + time.comp_ns * 1e-9 - tpcTriggerTime;
    nevent++;
    
    for( int board = 0; board < nBoards; board++ ) {
      for( int pmt = 0; pmt < nPMTs; pmt++) {
	if( pmt != 5 )
	  integral_pmt[j][board][pmt] = 0; //Integral( waveforms[board][pmt] );
      }
    }
    
    j++;
    pdsTree->GetEvent(i + j);
    inBeamWindow_prev = inBeamWindow_curr;
    inBeamWindow_curr = (trigger - CheckBeamWindow() > 0 && CheckBeamWindow() > 0);
    
    //cout << "tpc_ev#:" << i << "\t" << "pds_ev#:" << j << endl;
    //cout << "beam window: " << inBeamWindow_prev << inBeamWindow_curr << endl;

    hPmtSum->Delete();
  } while ( !( !inBeamWindow_prev && inBeamWindow_curr ) && pdsTree->GetEvent(i + j));
}

void RemoveADCOffset(TH1F* h) 
{
  Double_t offset = 0;
  for( int time = 0; time < 250; time++ ) {
    offset += h->GetBinContent(time)/250.;
  }
  
  for( int i = 0; i < h->GetNbinsX(); i++ ) {
    h->SetBinContent(i, h->GetBinContent(i) - offset);
  }
}

Double_t Integral(Double_t& waveform[maxTime])
{
  Double_t integral = 0;
  for( int time = trigger; time < maxTime; time++ ) {
    integral += waveform[time];
  }
  return integral;
}

Double_t FindPeak(TH1F* h) 
{
    // note that the peak is actually the minimum
  Double_t peak = h->GetBinContent(trigger);
  for(Int_t time = -250; time < 250; time++) {
    if( h->GetBinContent(  time + trigger ) < peak ) peak = h->GetBinContent(time + trigger);
      if( h->GetBinContent( -time + trigger ) < peak ) peak = h->GetBinContent(time + trigger);
  }
  return peak;
}

Int_t CheckBeamWindow(Int_t board=0) 
{
  Bool_t ascending = false;
  Bool_t descending = false;
  Int_t rfTime = -9999;
  
  for( int time = trigger; time > trigger + beamWindow; time--) {
    if( waveforms[board][5][time] <= rfThreshold && waveforms[board][5][time+1] > rfThreshold ) {
      descending = true;
    } 
    if( descending && waveforms[board][5][time] > rfThreshold ) {
      ascending = true;
      rfTime = time;
      break;
    }
  }
  //std::cout << rfTime << std::endl;
  return rfTime;
}
////////////////////////////////////////////////////////////////////////////////

void DrawWaveform(Int_t event, Int_t board, Int_t pmt, TString option="") 
{
  pdsTree->GetEntry(event);
  
  TString waveformName = Form("hEv%d_b%d_pmt%d",event,board,pmt);
  TH1F* waveform = new TH1F(waveformName,waveformName+";time (s);ADC",maxTime,0,maxTime);
  
  for( int i = 0; i < maxTime; i++ ) {
    waveform->Fill(i, waveforms[board][pmt][i]);
  }
  
  c->cd();
    waveform->Draw("l "+option);
    c->Update();
  
    return;
}

void DrawEvent(Int_t event, TString option="") 
{
  pdsTree->GetEntry(event);
  
  TString pmtWaveformName = Form("hEv%d_pmtAvg",event);
  TString rfWaveformName  = Form("hEv%d_rfAvg", event);
  
  TH1F* pmtWaveform = new TH1F(pmtWaveformName,Form("ev%d;Sample;ADC",event),maxTime,0,maxTime);
  TH1F* rfWaveform = new TH1F(rfWaveformName,Form("ev%d;Sample;ADC",event),maxTime,0,maxTime);
  
  for( int board = 0; board < nBoards; board++) {
    for( int pmt = 0; pmt < nPMTs; pmt++) {
      for( int i = 0; i < maxTime; i++ ) {
	if( pmt == 5 )
	  rfWaveform->Fill(i, waveforms[board][pmt][i]/nBoards + 300);
	else
	  pmtWaveform->Fill(i, waveforms[board][pmt][i]/nBoards/(nPMTs-1));
      }
    }
  }
  
  rfWaveform->SetLineColor(kRed+2);
  rfWaveform->GetYaxis()->SetRangeUser(0,4000);
  rfWaveform->Draw("l "+option);
  pmtWaveform->Draw("l same"+option);
  
  c->Update();
  
  return;
}


