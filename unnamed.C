// Dialog macro generated from application: /global/project/projectdirs/captain/releases/LCGCMT/2.0.3/EXTERNALS/ROOT/5.34.34/x86_64-linux-gcc44-opt/bin/root.exe
// By ROOT version 5.34/34 on 2016-04-04 14:11:26

#if !defined( __CINT__) || defined (__MAKECINT__)

#ifndef ROOT_TGFrame
#include "TGFrame.h"
#endif
#ifndef ROOT_TGListBox
#include "TGListBox.h"
#endif
#ifndef ROOT_TGScrollBar
#include "TGScrollBar.h"
#endif
#ifndef ROOT_TGComboBox
#include "TGComboBox.h"
#endif
#ifndef ROOT_TGMenu
#include "TGMenu.h"
#endif
#ifndef ROOT_TGFileDialog
#include "TGFileDialog.h"
#endif
#ifndef ROOT_TGButtonGroup
#include "TGButtonGroup.h"
#endif
#ifndef ROOT_TRootDialog
#include "TRootDialog.h"
#endif
#ifndef ROOT_TGCanvas
#include "TGCanvas.h"
#endif
#ifndef ROOT_TGFSContainer
#include "TGFSContainer.h"
#endif
#ifndef ROOT_TGButton
#include "TGButton.h"
#endif
#ifndef ROOT_TRootContextMenu
#include "TRootContextMenu.h"
#endif
#ifndef ROOT_TGFSComboBox
#include "TGFSComboBox.h"
#endif
#ifndef ROOT_TGLabel
#include "TGLabel.h"
#endif
#ifndef ROOT_TGListView
#include "TGListView.h"
#endif
#ifndef ROOT_TGSplitter
#include "TGSplitter.h"
#endif
#ifndef ROOT_TGTextEntry
#include "TGTextEntry.h"
#endif
#ifndef ROOT_TRootCanvas
#include "TRootCanvas.h"
#endif
#ifndef ROOT_TGToolTip
#include "TGToolTip.h"
#endif

#include "Riostream.h"

#endif

void unnamed()
{

   // transient frame
   TGTransientFrame *fRootDialog338 = new TGTransientFrame(gClient->GetRoot(),0,285,85,kMainFrame | kVerticalFrame | kTransientFrame);
   TGLabel *fLabel339 = new TGLabel(fRootDialog338,"(const char*)  title  [default: \"\"]");
   fLabel339->SetTextJustify(36);
   fLabel339->SetMargins(0,0,0,0);
   fLabel339->SetWrapLength(-1);
   fRootDialog338->AddFrame(fLabel339, new TGLayoutHints(kLHintsCenterX | kLHintsTop,0,0,5,0));

   TGFont *ufont;         // will reflect user font changes
   ufont = gClient->GetFont("-*-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");

   TGGC   *uGC;           // will reflect user GC changes
   // graphics context changes
   GCValues_t valEntry340;
   valEntry340.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",valEntry340.fForeground);
   gClient->GetColorByName("#e8e8e8",valEntry340.fBackground);
   valEntry340.fFillStyle = kFillSolid;
   valEntry340.fFont = ufont->GetFontHandle();
   valEntry340.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valEntry340, kTRUE);
   TGTextEntry *fTextEntry340 = new TGTextEntry(fRootDialog338, new TGTextBuffer(31),-1,uGC->GetGC(),ufont->GetFontStruct(),kSunkenFrame | kOwnBackground);
   fTextEntry340->SetMaxLength(4096);
   fTextEntry340->SetAlignment(kTextLeft);
   fTextEntry340->SetText("Event integral (pe ns)");
   fTextEntry340->Resize(260,fTextEntry340->GetDefaultHeight());
   fRootDialog338->AddFrame(fTextEntry340, new TGLayoutHints(kLHintsLeft | kLHintsTop,5,5,5,5));
   fTextEntry340->Connect("TabPressed()", 0, 0, "TabPressed()");

   // horizontal frame
   TGHorizontalFrame *fHorizontalFrame341 = new TGHorizontalFrame(fRootDialog338,285,22,kHorizontalFrame | kFixedWidth);
   TGTextButton *fTextButton342 = new TGTextButton(fHorizontalFrame341,"&OK",1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
   fTextButton342->SetTextJustify(36);
   fTextButton342->SetMargins(0,0,0,0);
   fTextButton342->SetWrapLength(-1);
   fTextButton342->Resize(85,22);
   fHorizontalFrame341->AddFrame(fTextButton342, new TGLayoutHints(kLHintsCenterY | kLHintsExpandX,5,5,0,0));
   TGTextButton *fTextButton343 = new TGTextButton(fHorizontalFrame341,"&Cancel",3,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
   fTextButton343->SetTextJustify(36);
   fTextButton343->SetMargins(0,0,0,0);
   fTextButton343->SetWrapLength(-1);
   fTextButton343->Resize(85,22);
   fHorizontalFrame341->AddFrame(fTextButton343, new TGLayoutHints(kLHintsCenterY | kLHintsExpandX,5,5,0,0));
   TGTextButton *fTextButton344 = new TGTextButton(fHorizontalFrame341,"Online &Help",4,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
   fTextButton344->SetTextJustify(36);
   fTextButton344->SetMargins(0,0,0,0);
   fTextButton344->SetWrapLength(-1);
   fTextButton344->Resize(85,22);
   fHorizontalFrame341->AddFrame(fTextButton344, new TGLayoutHints(kLHintsCenterY | kLHintsExpandX,5,5,0,0));

   fRootDialog338->AddFrame(fHorizontalFrame341, new TGLayoutHints(kLHintsCenterX | kLHintsBottom,0,0,5,5));

   fRootDialog338->SetWindowName("TAxis::SetTitle");
   fRootDialog338->SetIconName("TAxis::SetTitle");
   fRootDialog338->SetMWMHints(kMWMDecorAll | kMWMDecorResizeH | kMWMDecorMenu | kMWMDecorMinimize | kMWMDecorMaximize,
                        kMWMFuncAll | kMWMFuncResize | kMWMFuncMinimize | kMWMFuncMaximize,
                        kMWMInputModeless);
   fRootDialog338->SetWMPosition(670,284);
   fRootDialog338->SetWMSize(285,85);
   fRootDialog338->SetWMSizeHints(285,85,285,85,0,0);
   fRootDialog338->Move(670,284);
   fRootDialog338->MapSubwindows();

   fRootDialog338->Resize(fRootDialog338->GetDefaultSize());
   fRootDialog338->MapWindow();
   fRootDialog338->Resize();
}  

void TabPressed()
{
   cout << "Slot TabPressed()" << endl; 
}
