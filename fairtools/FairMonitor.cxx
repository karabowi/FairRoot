/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *              GNU Lesser General Public Licence (LGPL) version 3,             *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
/*
 * FairMonitor.cxx
 *
 *  Created on: Aug 01, 2015
 *      Author: r.karabowicz
 */

#include "FairMonitor.h"
#include "FairLogger.h"
#include "FairSystemInfo.h"

#include "TArrow.h"
#include "TAxis.h"
#include "TBox.h"
#include "TCanvas.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TH1F.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TList.h"
#include "TMath.h"
#include "TNamed.h"
#include "TPaveText.h"
#include "TROOT.h"
#include "TStopwatch.h"
#include "TString.h"
#include "TTask.h"

#include "TVirtualMC.h"

#include <iomanip>
#include <iostream>
#include <iterator>
#include <utility>

FairMonitor* FairMonitor::instance = NULL;

//_____________________________________________________________________________
FairMonitor::FairMonitor()
  : TNamed("FairMonitor","Monitor for FairRoot")
  , fRunMonitor(kFALSE)
  , fDrawCanvas(kFALSE)
  , fRunTime(0.)
  , fRunMem(0.)
  , fTimerMap()
  , fMemoryMap()
  , fHistList(new TList())
  , fCanvas()
  , fNoTaskRequired(0)
  , fNoTaskCreated(0)
  , fCurrentTask(0)
  , fTaskRequired()
  , fTaskCreated()
  , fTaskCreatedTemp()
  , fObjectMap()
  , fTaskMap()
  , fObjectPos()
  , fTaskPos()
  , fStepCounter(0)
  , fNumberOfMains(0)
{
}
//_____________________________________________________________________________

//_____________________________________________________________________________
FairMonitor::~FairMonitor()
{
}
//_____________________________________________________________________________
  
//_____________________________________________________________________________
FairMonitor* FairMonitor::GetMonitor()
{
  if (!instance) {
    instance = new FairMonitor();
  }
  return instance;
}
//_____________________________________________________________________________

//_____________________________________________________________________________
void FairMonitor::StartTimer(const TTask* tTask, const char* identStr) {
  if ( !fRunMonitor ) return;

  TString tempString = Form("timer_%p_%s_%s",tTask,tTask->GetName(),identStr);

  typedef std::map<TString, TStopwatch>::iterator ttMapIter;

  ttMapIter itt = fTimerMap.find(tempString);
  if ( itt == fTimerMap.end() ) {
    fTimerMap.insert(std::pair<TString, TStopwatch> (tempString,TStopwatch()));
    itt = fTimerMap.find(tempString);
  }
  fTimerMap[tempString].Start();
}
//_____________________________________________________________________________

//_____________________________________________________________________________
void FairMonitor::StopTimer(const TTask* tTask, const char* identStr) {
  if ( !fRunMonitor ) return;

  TString tempString = Form("timer_%p_%s_%s",tTask,tTask->GetName(),identStr);

  typedef std::map<TString, TStopwatch>::iterator ttMapIter;

  ttMapIter itt = fTimerMap.find(tempString);
  if ( itt == fTimerMap.end() ) {
    LOG(info) << "FairMonitor::StopTimer() called without matching StartTimer()";
    return;
  }
  //  itt->second
  fTimerMap[tempString].Stop();

  Double_t time = fTimerMap[tempString].RealTime();
  RecordInfo(tTask,Form("%s_TIM",identStr),time);

  if ( tempString.EndsWith("_EXEC") )
    fRunTime += time;
}
//_____________________________________________________________________________

//_____________________________________________________________________________
void FairMonitor::StartMemoryMonitor(const TTask* tTask, const char* identStr) {
  if ( !fRunMonitor ) return;
  FairSystemInfo sysInfo;
  Int_t memoryAtStart = static_cast<Int_t>(sysInfo.GetCurrentMemory());

  TString memMon = Form("mem_%p_%s_%s",tTask,tTask->GetName(),identStr);
  
  typedef std::map<TString, Int_t>::iterator tiMapIter;

  tiMapIter iti = fMemoryMap.find(memMon);
  if ( iti == fMemoryMap.end() ) 
    fMemoryMap.insert(std::pair<TString, Int_t> (memMon,memoryAtStart));
  else
    iti->second = memoryAtStart;
}
//_____________________________________________________________________________

//_____________________________________________________________________________
void FairMonitor::StopMemoryMonitor(const TTask* tTask, const char* identStr) {
  if ( !fRunMonitor ) return;
  FairSystemInfo sysInfo;
  Int_t memoryAtEnd = static_cast<Int_t>(sysInfo.GetCurrentMemory());

  TString memMon = Form("mem_%p_%s_%s",tTask,tTask->GetName(),identStr);
  
  typedef std::map<TString, Int_t>::iterator tiMapIter;

  tiMapIter iti = fMemoryMap.find(memMon);
  if ( iti == fMemoryMap.end() ) 
    LOG(warn) << "FairMonitor::StopMemoryMonitor() Could not find corresponding entry for \"" << memMon.Data() << "\".";
  else {
    RecordInfo(tTask,Form("%s_MEM",identStr),memoryAtEnd-iti->second);
    
    if ( memMon.EndsWith("_EXEC") )
      fRunMem += memoryAtEnd-iti->second;
  } 
}
//_____________________________________________________________________________

//_____________________________________________________________________________
void FairMonitor::RecordInfo(const TTask* tTask, const char* identStr, Double_t value) {
  if ( !fRunMonitor ) return;
  
  TString tempString = Form("hist_%p_%s_%s",tTask,tTask->GetName(),identStr);

  Int_t nofHists = fHistList->GetEntries();
  Int_t ihist = 0;
  for ( ihist = 0 ; ihist < nofHists ; ihist++ ) {
    if ( !tempString.CompareTo(fHistList->At(ihist)->GetName()) ) {
      break;
    }
  }
  if ( ihist == nofHists ) {
    TString titleString = Form("Histogram %s for %s",identStr,tTask->GetName());
    fHistList->Add(new TH1F(tempString,titleString,1000,0,1000));
  }
  TH1F* tempHist = (static_cast<TH1F*>((fHistList->At(ihist))));
  Int_t nofEntries = tempHist->GetEntries();
  if ( nofEntries > tempHist->GetNbinsX() ) 
    tempHist->SetBins(tempHist->GetNbinsX()*10, 0, tempHist->GetXaxis()->GetXmax()*10);

  tempHist->SetBinContent(nofEntries+1,value);
}
//_____________________________________________________________________________

//_____________________________________________________________________________
void FairMonitor::RecordRegister(const char* name, const char* folderName, Bool_t toFile) {
  if ( !fRunMonitor ) return;

  LOG(debug) << "*** FM::RecordRegister(" << name << ", " << folderName << (toFile?", kTRUE)":", kFALSE") << " for task >>" << fCurrentTask << "<< (" << (fCurrentTask?fCurrentTask->GetName():"") << ")";
  if ( fCurrentTask == 0 ) {
    fNoTaskCreated ++; 
    LOG(info) << "*** FM::RecordRegister(" << name << ", " << folderName << (toFile?", kTRUE)":", kFALSE") << " WITHOUT TASK";
    return;
  }
  TString tempString = Form("%p_%s",fCurrentTask,fCurrentTask->GetName());
  if ( toFile ) {
    fTaskCreated.insert(std::pair<TString, TString>(tempString,TString(name)));
  }
  else {
    fTaskCreatedTemp.insert(std::pair<TString, TString>(tempString,TString(name)));
  }
}
//_____________________________________________________________________________

//_____________________________________________________________________________
void FairMonitor::RecordGetting(const char* name) {
  if ( !fRunMonitor ) return;

  LOG(debug) << "*** FM::RecordGetting(" << name << ") for task >>" << fCurrentTask << "<< (" << (fCurrentTask?fCurrentTask->GetName():"") << ")";

  if ( fCurrentTask == 0 ) {
    fNoTaskRequired ++;
    return;
  }
  TString tempString = Form("%p_%s",fCurrentTask,fCurrentTask->GetName());
  fTaskRequired.insert(std::pair<TString, TString>(tempString,TString(name)));
}
//_____________________________________________________________________________

//_____________________________________________________________________________
void FairMonitor::PrintTask(TTask* tempTask, Int_t taskLevel) const {
  if ( !fRunMonitor ) return;

  Int_t nofHists = fHistList->GetEntries();
  TString tempString = "";

  Double_t timInt = -1.;
  Double_t timEnt = 0.;
  Int_t memInt = -1.;

  char byteChar[4] = {'B','k','M','G'};

  for ( Int_t ihist = 0 ; ihist < nofHists ; ihist++ ) {
    tempString = Form("%s",fHistList->At(ihist)->GetName());
    if ( tempString.Contains(Form("%p",tempTask)) ) {
      if ( tempString.Contains("EXEC_TIM") ) {
	timInt = (static_cast<TH1F*>(fHistList->At(ihist))->Integral());
	timEnt = (static_cast<TH1F*>(fHistList->At(ihist))->GetEntries());
      }
      if ( tempString.Contains("EXEC_MEM") ) {
	memInt = (static_cast<TH1F*>(fHistList->At(ihist))->Integral());
      }
    }
  }
  if ( timInt < 0 ) {
    LOG(warn) << "FairMonitor::PrintTask(), task \"" << tempTask->GetName() << "\" not found!"; 
  }
  else {
    //    LOG(info) << "\"" << tempTask->GetName() << "\" --> TIME integral =  " << timInt << " -- MEMORY integral = " << memInt;
    TString printString = Form("%f", timInt/timEnt);
    for ( Int_t itemp = printString.Length() ; itemp < 10 ; itemp++ ) 
      printString.Insert(0,' ');
    if (printString.Length() > 10 ) printString.Remove(11,100);
    printString += " s/ev |";
    Double_t timePerc = 100.*timInt/fRunTime;
    if ( timePerc < 100. ) printString += ' ';
    if ( timePerc <  10. ) printString += ' ';
    tempString = Form("%f",timePerc);
    for ( Int_t itemp = tempString.Length() ; itemp < 8 ; itemp++ ) 
      printString += ' ';
    printString += tempString;
    printString.Remove(27,100);
    Int_t byteIdent = 0;
    while ( memInt > 1024 ) {
      memInt = memInt/1024;
      byteIdent++;
    }
    tempString = Form("%4d",memInt);
    printString += " % ] ";
    if ( memInt < 0 )
      printString += "- - -";
    else {
      printString += tempString;
      printString += byteChar[byteIdent];
    }
    printString += " \"";
    for ( Int_t ilev = 0 ; ilev < taskLevel ; ilev++ )
      printString += "  ";
    printString += tempTask->GetName();
    if ( printString.Length() > 80 )
      printString.Remove(80,100);
    printString += "\"";
    Int_t timeFrac = static_cast<Int_t>((timePerc/100.*30.));

    printString.Insert(37,"\033[0m");
    switch ( byteIdent ) {
    case 0:  printString.Insert(32,"\033[42m"); break;
    case 1:  printString.Insert(32,"\033[43m"); break;
    default: printString.Insert(32,"\033[41m"); break;
    }
    printString.Insert(timeFrac,"\033[0m");
    if ( timePerc < 30 )
      printString.Insert(0,"[\033[42m");
    else if ( timePerc < 90 )
      printString.Insert(0,"[\033[43m");
    else
      printString.Insert(0,"[\033[41m");
    LOG(INFO) << printString.Data();
  }

  TList* subTaskList = tempTask->GetListOfTasks();
  if ( !subTaskList ) return;
  for ( Int_t itask = 0 ; itask < subTaskList->GetEntries() ; itask++ ) {
    TTask* subTask = static_cast<TTask*>(subTaskList->At(itask));
    if ( subTask ) 
      PrintTask(subTask,taskLevel+1);
  }
}
//_____________________________________________________________________________

//_____________________________________________________________________________
void FairMonitor::Print(Option_t*) const {
  if ( !fRunMonitor ) {
    LOG(warn) << "FairMonitor was disabled. Nothing to print!";
    return;
  }

  LOG(info) << "- Total Run Time: " << fRunTime << " s ---------------------------------------------------------";
  TTask* mainFairTask = static_cast<TTask*>((gROOT->GetListOfBrowsables()->FindObject("FairTaskList")));
  if ( mainFairTask ) 
    PrintTask(mainFairTask,0);
  LOG(info) << "-------------------------------------------------------------------------------------";
  
}
//_____________________________________________________________________________

//_____________________________________________________________________________
void FairMonitor::PrintTask(TString specString) const {
  if ( !fRunMonitor ) {
    LOG(warn) << "FairMonitor was disabled. Nothing to print!";
    return;
  }

  TString unitString = "";

  Int_t nofHists = fHistList->GetEntries();
  for ( Int_t ihist = 0 ; ihist < nofHists ; ihist++ ) {
    TString histString = Form("%s",fHistList->At(ihist)->GetName());
    if ( histString.Contains(specString) ) {
      histString.Replace(0,histString.First('_')+1,"");
      histString.Replace(0,histString.First('_')+1,"");
      Double_t integral = (static_cast<TH1F*>((fHistList->At(ihist)))->Integral());
      Double_t entries  = (static_cast<TH1F*>((fHistList->At(ihist)))->GetEntries());
//      Double_t valPent  = integral/entries;
      if ( histString.EndsWith("_TIM") ) unitString = " s";
      if ( histString.EndsWith("_MEM") ) unitString = " B";
      LOG(info) << histString.Data() << " >>>>> " << integral << unitString.Data() << " / " << entries << " ent = " << integral/entries << unitString.Data() << " / ent";
    }
  }
}
//_____________________________________________________________________________

//_____________________________________________________________________________
void FairMonitor::Draw(Option_t*) {
  if ( !fRunMonitor ) {
    LOG(warn) << "FairMonitor was disabled. Nothing to print!";
    return;
  }

  typedef std::multimap<TString, TString>::iterator bnMapIter;

  typedef std::map<TString, Int_t>::iterator tiMapIter;
  tiMapIter iti;
  
  TTask* mainFairTask = static_cast<TTask*>((gROOT->GetListOfBrowsables()->FindObject("FairTaskList")));
  if ( !mainFairTask ) return;

  GetTaskMap(mainFairTask);

  for(bnMapIter itb = fTaskRequired.begin(); itb != fTaskRequired.end() ; itb++) {
    iti = fTaskMap.find(itb->first);
    if ( iti == fTaskMap.end() )
      fTaskMap.insert(std::pair<TString, Int_t> (itb->first,0));
    iti = fObjectMap.find(itb->second);
    if ( iti == fObjectMap.end() )
      fObjectMap.insert(std::pair<TString, Int_t> (itb->second,0));
  }
  for(bnMapIter itb = fTaskCreated.begin(); itb != fTaskCreated.end() ; itb++) {
    iti = fTaskMap.find(itb->first);
    if ( iti == fTaskMap.end() )
      fTaskMap.insert(std::pair<TString, Int_t> (itb->first,0));
    iti = fObjectMap.find(itb->second);
    if ( iti == fObjectMap.end() )
      fObjectMap.insert(std::pair<TString, Int_t> (itb->second,0));
  }
  for(bnMapIter itb = fTaskCreatedTemp.begin(); itb != fTaskCreatedTemp.end() ; itb++) {
    iti = fTaskMap.find(itb->first);
    if ( iti == fTaskMap.end() )
      fTaskMap.insert(std::pair<TString, Int_t> (itb->first,0));
    iti = fObjectMap.find(itb->second);
    if ( iti == fObjectMap.end() )
      fObjectMap.insert(std::pair<TString, Int_t> (itb->second,0));
  }
  
  AnalyzeObjectMap(mainFairTask);

  Int_t maxHierarchyNumber = 0;
  for ( iti = fObjectMap.begin() ; iti != fObjectMap.end() ; iti++ ) {
    if ( maxHierarchyNumber < iti->second ) 
      maxHierarchyNumber = iti->second; 
  }

  LOG(debug) << "Max hierarchy number is " << maxHierarchyNumber;

  fCanvas = new TCanvas("MonitorCanvas","Fair Monitor",10,10,960,600);
  fCanvas->cd();

  fCanvas->SetFillStyle(4000);
  fCanvas->Range(0,0,960,800);
  fCanvas->SetFillColor(0);
  fCanvas->SetBorderSize(0);
  fCanvas->SetBorderMode(0);
  fCanvas->SetFrameFillColor(0);

  for ( Int_t ihier = 0 ; ihier < maxHierarchyNumber+1 ; ihier++ ) {
    Int_t nofHier = 0;
    for ( iti = fTaskMap.begin() ; iti != fTaskMap.end() ; iti++ ) {
      if ( iti->second == ihier ) {
	nofHier ++;
      }
    }
    LOG(debug) << "There are " << nofHier << " tasks on level " << ihier << ".";

    if ( ihier == 0 ) {
      Double_t iObj = 0.;
      for ( iti = fTaskMap.begin() ; iti != fTaskMap.end() ; iti++ ) {
	if ( iti->second == ihier ) {
	  std::pair<Double_t, Double_t> tempPos(50,600-iObj*40);
	  fTaskPos.insert(std::pair<TString, std::pair<Double_t, Double_t> > (iti->first,tempPos));
	  iObj+=1.;
	}
      }
    }
    else {
      Int_t iObj = 0;
      Int_t secLine = 0;
      Int_t secLineEven = 0;
      if ( nofHier > 9 ) {
	secLine = 1;
	if ( nofHier%2 == 0 ) secLineEven = 1;
      }
      Int_t startingPosition = 575 - nofHier/(1+secLine)*45-secLine*(1-secLineEven)*45;

      Double_t topEdge = 800.-800.*(2.*static_cast<Double_t>(ihier)-0.5)/(static_cast<Double_t>(2*maxHierarchyNumber+1))+secLine*15;
      LOG(debug) << "for level " << ihier << " will put top edge at " << topEdge 
		<< ". " << (secLineEven?"Two lines":"One line") << (secLineEven?" with offset":"");
      for ( iti = fTaskMap.begin() ; iti != fTaskMap.end() ; iti++ ) {
	if ( iti->second == ihier ) {
	  std::pair<Double_t, Double_t> tempPos(startingPosition+iObj*90/(1+secLine)-secLineEven*(iObj%2)*45,topEdge-15-secLine*(iObj%2)*35);
 	  fTaskPos.insert(std::pair<TString, std::pair<Double_t, Double_t> > (iti->first,tempPos));
	  iObj+=1;
	}
      }
    }

    nofHier = 0;
    for ( iti = fObjectMap.begin() ; iti != fObjectMap.end() ; iti++ ) {
      if ( TMath::Abs(iti->second) == ihier ) {
	nofHier ++;
      }
    }
    LOG(debug) << "There are " << nofHier << " objects on level " << ihier << ".";

    Int_t iObj = 0;
    Int_t secLine = 0;
    Int_t secLineEven = 0;
    if ( nofHier > 9 ) {
      secLine = 1;
      if ( nofHier%2 == 0 ) secLineEven = 1;
    }
    Int_t startingPosition = 575 - nofHier/(1+secLine)*45-secLine*(1-secLineEven)*45;

    Double_t topEdge = 800.-800.*(2.*static_cast<Double_t>(ihier)+0.5)/(static_cast<Double_t>(2*maxHierarchyNumber+1))+secLine*15;
    LOG(debug) << "for level " << ihier << " will put top edge at " << topEdge 
	      << ". " << (secLineEven?"Two lines":"One line") << (secLineEven?" with offset":"");
    for ( iti = fObjectMap.begin() ; iti != fObjectMap.end() ; iti++ ) {
      if ( TMath::Abs(iti->second) == ihier ) {
	std::pair<Double_t, Double_t> tempPos(startingPosition+iObj*90/(1+secLine)-secLineEven*(iObj%2)*45,topEdge-15-secLine*(iObj%2)*35);
	fObjectPos.insert(std::pair<TString, std::pair<Double_t, Double_t> > (iti->first,tempPos));
	iObj+=1;
      }
    }
    
  }

  typedef std::map<TString, std::pair<Double_t, Double_t> >::iterator tddMapIter;
  tddMapIter itt;
  tddMapIter ito;
  
  for(bnMapIter itb = fTaskRequired.begin(); itb != fTaskRequired.end() ; itb++) {
    itt = fTaskPos.find(itb->first);
    ito = fObjectPos.find(itb->second);
    if ( itt != fTaskPos.end() && ito != fObjectPos.end() ) {
      std::pair<Double_t, Double_t> taskPos = itt->second;
      std::pair<Double_t, Double_t> objectPos = ito->second;
      TArrow* tempArrow = new TArrow(objectPos.first,objectPos.second-15,taskPos.first,taskPos.second+15,0.01,"|>");
      tempArrow->Draw();
    }
  }
  for(bnMapIter itb = fTaskCreated.begin(); itb != fTaskCreated.end() ; itb++) {
    itt = fTaskPos.find(itb->first);
    ito = fObjectPos.find(itb->second);
    if ( itt != fTaskPos.end() && ito != fObjectPos.end() ) {
      std::pair<Double_t, Double_t> taskPos = itt->second;
      std::pair<Double_t, Double_t> objectPos = ito->second;
      TArrow* tempArrow = new TArrow(taskPos.first,taskPos.second-15,objectPos.first,objectPos.second+15,0.01,"|>");
      tempArrow->Draw();
    }
  }
  for(bnMapIter itb = fTaskCreatedTemp.begin(); itb != fTaskCreatedTemp.end() ; itb++) {
    itt = fTaskPos.find(itb->first);
    ito = fObjectPos.find(itb->second);
    if ( itt != fTaskPos.end() && ito != fObjectPos.end() ) {
      std::pair<Double_t, Double_t> taskPos = itt->second;
      std::pair<Double_t, Double_t> objectPos = ito->second;
      TArrow* tempArrow = new TArrow(taskPos.first,taskPos.second-15,objectPos.first,objectPos.second+15,0.01,"|>");
      tempArrow->Draw();
    }
  }

  for ( itt = fTaskPos.begin() ; itt != fTaskPos.end() ; itt++ ) {
    std::pair<Double_t, Double_t> taskPos = itt->second;
    TBox* bkgBox = new TBox(taskPos.first-40,taskPos.second-15,taskPos.first+40,taskPos.second+15);
    bkgBox->SetFillColor(kGreen-9);
    bkgBox->Draw();

    TString tempString = Form("hist_%s_%s",itt->first.Data(),"EXEC_TIM");
    Int_t nofHists = fHistList->GetEntries();
    for ( Int_t ihist = 0 ; ihist < nofHists ; ihist++ ) {
      if ( !tempString.CompareTo(fHistList->At(ihist)->GetName()) ) {
	Double_t timeInt = (static_cast<TH1F*>(fHistList->At(ihist))->Integral());
	Double_t timeFrac = 80.*timeInt/fRunTime;
	TBox* barBox = new TBox(taskPos.first-40,taskPos.second-15,taskPos.first-40+timeFrac,taskPos.second+15);
	barBox->SetFillColor(kRed);
	barBox->Draw();
      }
    }

    tempString = itt->first;
    tempString.Replace(0,tempString.First('_')+1,"");
    if ( tempString.Length() > 16 ) tempString.Replace(16,tempString.Length(),"");
    TLatex* taskText = new TLatex(taskPos.first,taskPos.second,tempString.Data());
    taskText->SetTextAlign(22);
    taskText->SetTextSize(0.015);
    taskText->Draw();
  }

  for ( ito = fObjectPos.begin() ; ito != fObjectPos.end() ; ito++ ) {
    std::pair<Double_t, Double_t> objectPos = ito->second;
    iti = fObjectMap.find(ito->first);
    TPaveText* paveText = new TPaveText(objectPos.first-40,objectPos.second-15,objectPos.first+40,objectPos.second+15,"nb");
    paveText->SetFillColor(kMagenta-9);
    if ( iti != fObjectMap.end() )
      if ( iti->second < 0 ) 
	paveText->SetFillColor(kGray);
    paveText->SetShadowColor(0);
    paveText->SetTextSize(0.015);
    TString tempString = ito->first;
    if ( tempString.Length() > 16 ) tempString.Replace(16,tempString.Length(),"");
    paveText->AddText(tempString);
    paveText->Draw();
  }


}
//_____________________________________________________________________________

//_____________________________________________________________________________
void FairMonitor::DrawHist(TString specString) {
  if ( !fRunMonitor ) {
    LOG(warn) << "FairMonitor was disabled. Nothing to draw!";
    return;
  }

  Int_t nofHists = fHistList->GetEntries();
  Int_t nofDraws = 0;
  TString drawStr = "P";

  TLegend* tempLeg = new TLegend(0.0,0.5,0.5,0.9);

  Double_t histMax = 0.;
  for ( Int_t ihist = 0 ; ihist < nofHists ; ihist++ ) {
    TString histString = Form("%s",fHistList->At(ihist)->GetName());
    if ( histString.Contains(specString) ) {
      TH1F* tempHist = (static_cast<TH1F*>((fHistList->At(ihist))));
      if ( histMax < tempHist->GetMaximum() )
	histMax = tempHist->GetMaximum();
    }
  }

  for ( Int_t ihist = 0 ; ihist < nofHists ; ihist++ ) {
    TString histString = Form("%s",fHistList->At(ihist)->GetName());
    if ( histString.Contains(specString) ) {
      TH1F* tempHist = (static_cast<TH1F*>((fHistList->At(ihist))));
      if ( tempHist->GetXaxis()->GetXmax() > tempHist->GetEntries() )
	tempHist->SetBins(tempHist->GetEntries(),0,tempHist->GetEntries());
      tempHist->SetMarkerColor(nofDraws%6+2);
      tempHist->SetMarkerStyle(nofDraws%4+20);
      tempHist->SetAxisRange(0.,histMax*1.1,"Y");
      tempHist->Draw(drawStr);
      TString tempName = tempHist->GetName();
      tempName.Remove(0,tempName.First('_')+1);
      tempName.Remove(0,tempName.First('_')+1);
      tempLeg->AddEntry(tempHist,tempName,"p");
      nofDraws++;
      drawStr = "Psame";
    }
  }
  if ( nofDraws > 1 ) 
    tempLeg->Draw();
}
//_____________________________________________________________________________

//_____________________________________________________________________________
void FairMonitor::StoreHistograms(TFile* sinkFile)
{
  if ( !fRunMonitor ) {
    return;
  }
  Bool_t wasBatch = gROOT->IsBatch();
  if ( !fDrawCanvas && !wasBatch ) // default: switching to batch mode if draw canvas disabled
    gROOT->SetBatch(kTRUE);
  this->Draw();
  if ( !fDrawCanvas && !wasBatch ) // default: switching to batch mode if draw canvas disabled
    gROOT->SetBatch(kFALSE);

  TFile* histoFile = sinkFile;
  if ( fOutputFileName.Length() > 1 && fOutputFileName != sinkFile->GetName() ) {
      histoFile = TFile::Open(fOutputFileName,"recreate");
  }

  histoFile->mkdir("MonitorResults");
  histoFile->cd("MonitorResults");
  TIter next(fHistList);
  while ( TH1* thist = (static_cast<TH1*>(next())) ) {
    thist->SetBins(thist->GetEntries(),0,thist->GetEntries());
    thist->Write();
  }
  if ( fCanvas )
    fCanvas->Write();

  if ( histoFile != sinkFile ) {
    histoFile->Close();
    delete histoFile;
  }
}
//_____________________________________________________________________________

//_Private function to fill the map of the tasks_______________________________
void FairMonitor::GetTaskMap(TTask* tempTask) {
  TString tempString = Form("%p_%s",tempTask,tempTask->GetName());
  fTaskMap.insert(std::pair<TString, Int_t> (tempString,0));

  TList* subTaskList = tempTask->GetListOfTasks();
  if ( !subTaskList ) return;
  for ( Int_t itask = 0 ; itask < subTaskList->GetEntries() ; itask++ ) {
    TTask* subTask = static_cast<TTask*>(subTaskList->At(itask));
    if ( subTask ) 
      GetTaskMap(subTask);
  }
}
//_____________________________________________________________________________

//_Private function to analyze the object list:________________________________
// assign to each of the tasks and objects (TClonesArray, pressumably)
// one number that places them in a hierarchy
// objects number 0 are the ones in the input array
// tasks get number from the abs(highest number object they read) increased by one
// tasks number i create objects number i (if they go to output file) or
// tasks number i create objects number -i (if they are not persistent)
void FairMonitor::AnalyzeObjectMap(TTask* tempTask) {
  TString tempString = Form("%p_%s",tempTask,tempTask->GetName());

  Int_t hierarchyNumber = 0;
  
  typedef std::multimap<TString, TString>::iterator bnMapIter;
  typedef std::map<TString, Int_t>::iterator tiMapIter;
  tiMapIter iti;

  LOG(debug) << "TASK \"" << tempTask->GetName() << "\" NEEDS:";
  for(bnMapIter itb = fTaskRequired.begin(); itb != fTaskRequired.end() ; itb++) {
    if ( itb->first != tempString ) continue;
    LOG(debug) << "   \"" << itb->second.Data() << "\"";
    iti = fObjectMap.find(itb->second);
    if ( iti == fObjectMap.end() ) continue;
    if ( hierarchyNumber <= TMath::Abs(iti->second) )
      hierarchyNumber = TMath::Abs(iti->second)+1;
  }

  //  hierarchyNumber++;

  LOG(debug) << "WILL GET hierarchyNumber = " << hierarchyNumber;

  iti = fTaskMap.find(tempString);
  if ( iti != fTaskMap.end() ) 
    iti->second = hierarchyNumber;

  LOG(debug) << "     \"" << tempTask->GetName() << "\" CREATES:";
  for(bnMapIter itb = fTaskCreated.begin(); itb != fTaskCreated.end() ; itb++) {
    if ( itb->first != tempString ) continue;
    LOG(debug) << " + \"" << itb->second.Data() << "\"";
    iti = fObjectMap.find(itb->second);
    if ( iti == fObjectMap.end() ) continue;
    iti->second = hierarchyNumber;
  }

  for(bnMapIter itb = fTaskCreatedTemp.begin(); itb != fTaskCreatedTemp.end() ; itb++) {
    if ( itb->first != tempString ) continue;
    LOG(debug) << " - \"" << itb->second.Data() << "\"";
    iti = fObjectMap.find(itb->second);
    if ( iti == fObjectMap.end() ) continue;
    iti->second = -hierarchyNumber;
  }


  TList* subTaskList = tempTask->GetListOfTasks();
  if ( !subTaskList ) return;
  for ( Int_t itask = 0 ; itask < subTaskList->GetEntries() ; itask++ ) {
    TTask* subTask = static_cast<TTask*>(subTaskList->At(itask));
    if ( subTask ) 
      AnalyzeObjectMap(subTask);
  }

}
//_____________________________________________________________________________

//_____________________________________________________________________________
void FairMonitor::RecordMCStepping() {
    if ( !fRunMonitor ) return;

    fStepCounter++;
    auto stack=gMC->GetStack();
    assert(stack);
    fTrackset.insert(stack->GetCurrentTrackNumber());
    fPdgset.insert(gMC->TrackPid());
    int copyNo;
    auto id = gMC->CurrentVolID(copyNo);
    if (fVolumeToSteps.find(id) == fVolumeToSteps.end()){
      fVolumeToSteps.insert(std::pair<int, int>(id, 1));
    }
    else {
      fVolumeToSteps[id]++;
    }
    if (fVolumeToStepsGlobal.find(id) == fVolumeToStepsGlobal.end()){
      fVolumeToStepsGlobal.insert(std::pair<int, int>(id, 1));
      fVolumeToTimeGlobal.insert(std::pair<int, double>(id, 0.));
    }
    else {
      fVolumeToStepsGlobal[id]++;
    }
    if (fIdToVolName.find(id) == fIdToVolName.end()) {
      fIdToVolName.insert(std::pair<int, char const*>(id, gMC->CurrentVolName()));
    }

    int mainId = -1;
    if ( fVolumeToMain.find(id) == fVolumeToMain.end() ) { // id seen for the first time
        // get main name - second name in path
        TString path = gMC->CurrentVolPath();
        path.Remove(0,1); // remove '/'
        path.Remove(0,path.First('/')+1); // remove "cave_1/"
        if ( path.First('/') != -1 )
            path.Remove(path.First('/'),path.Length()); // remove from first '/' till the end
        if ( fNameToMain.find(path) == fNameToMain.end() ) { // main not yet known
            fMainToSteps.insert(std::pair<int, int> (fNameToMain.size(),0));
            fMainToTime.insert(std::pair<int, double> (fNameToMain.size(),0.));
            fMainToName.insert(std::pair<int, TString>(fNameToMain.size(),path)); // insert main to name converter
            fNameToMain.insert(std::pair<TString, int>(path,fNameToMain.size())); // insert name to main converter
        }
        mainId = fNameToMain[path]; // get mainId
        fVolumeToMain.insert(std::pair<int, int>(id, mainId)); // insert id to mainId converter
    }
    else {
        mainId = fVolumeToMain[id];
    }
    fMainToSteps[mainId]++;

    // to monitor time
    /*    typedef std::map<TString, TStopwatch>::iterator ttMapIter;

    ttMapIter itt = fTimerMap.find(Form("%d",id));
    if ( itt == fTimerMap.end() ) {
        fTimerMap.insert(std::pair<TString, TStopwatch> (Form("%d",id),TStopwatch()));
    }
    fTimerMap[Form("%d",id)].Start();*/
    fStopWatch.Start();
}
//_____________________________________________________________________________

//_____________________________________________________________________________
void FairMonitor::FinishMCStepping() {
    auto stack=gMC->GetStack();
    assert(stack);
    int copyNo;
    auto id = gMC->CurrentVolID(copyNo);
    int mainId = fVolumeToMain[id];

    /*    typedef std::map<TString, TStopwatch>::iterator ttMapIter;

    ttMapIter itt = fTimerMap.find(Form("%d",id));
    if ( itt == fTimerMap.end() ) {
        LOG(info) << "FairMonitor::StopTimer() called without matching StartTimer()";
        return;
    }
    //  itt->second
    fTimerMap[Form("%d",id)].Stop();

    Double_t time = fTimerMap[Form("%d",id)].RealTime();
*/
    fStopWatch.Stop();
    Double_t time = fStopWatch.RealTime();

    fVolumeToTimeGlobal[id] = fVolumeToTimeGlobal[id] + time;
    fMainToTime[mainId]    = fMainToTime[mainId] + time;

}
//_____________________________________________________________________________

//_____________________________________________________________________________
void FairMonitor::FlushMCStepping() {
    if ( !fRunMonitor ) return;

    std::cerr << "did " << fStepCounter << " steps \n";
    std::cerr << "transported " << fTrackset.size() << " different tracks \n";
    std::cerr << "transported " << fPdgset.size() << " different types \n";
    // summarize steps per volume
    for (auto& p : fVolumeToSteps) {
        std::cout << " VolName " << fIdToVolName[p.first] << " COUNT " << p.second << "\n";
    }
    // for (auto& p : fMainToSteps) {
    //     std::cout << " MainName " << fMainToName[p.first] << " COUNT " << p.second << "\n";
    // }
    fStepCounter = 0;
    fTrackset.clear();
    fPdgset.clear();
    fVolumeToSteps.clear();
}
//_____________________________________________________________________________

//_____________________________________________________________________________
void FairMonitor::StoreMCStepping() {
    if ( !fRunMonitor ) return;

    TFile* prevFile = gFile;

    TFile* outFile = NULL;
    if ( fOutputFileName.Length() > 1 && fOutputFileName != gFile->GetName() ) {
        outFile = TFile::Open(fOutputFileName,"recreate");
    }

    gFile->mkdir("MonitorResults");
    gFile->cd("MonitorResults");

    TH1F* stepCountHistogram = new TH1F("stepCountHistogram","Step logger results;volume number;number of steps",fVolumeToStepsGlobal.size(),0.5,fVolumeToStepsGlobal.size()+0.5);
    uint binNumber = 0;
    for (auto& p : fVolumeToStepsGlobal) {
        std::cout << " VolName " << fIdToVolName[p.first] << " COUNT " << p.second << "\n";
        stepCountHistogram->SetBinContent(++binNumber,p.second);
        stepCountHistogram->GetXaxis()->SetBinLabel  (  binNumber,fIdToVolName[p.first]);
    }
    stepCountHistogram->DrawClone();
    stepCountHistogram->Write();

    TH1F* stepTimerHistogram = new TH1F("stepTimerHistogram","Step logger results;volume number;real time [s]",fVolumeToStepsGlobal.size(),0.5,fVolumeToStepsGlobal.size()+0.5);
    binNumber = 0;
    for (auto& p : fVolumeToTimeGlobal) {
        stepTimerHistogram->SetBinContent(++binNumber,p.second);
        stepTimerHistogram->GetXaxis()->SetBinLabel  (  binNumber,fIdToVolName[p.first]);
    }
    stepTimerHistogram->DrawClone();
    stepTimerHistogram->Write();

    TH1F* mainCountHistogram = new TH1F("mainCountHistogram","Main logger results;volume number;number of mains",fMainToName.size(),0.5,fMainToName.size()+0.5);
    binNumber = 0;
    for (auto& p : fMainToSteps ) {
        mainCountHistogram->SetBinContent(++binNumber,p.second);
        mainCountHistogram->GetXaxis()->SetBinLabel  (  binNumber,fMainToName[p.first]);
    }
    mainCountHistogram->DrawClone();
    mainCountHistogram->Write();

    TH1F* mainTimerHistogram = new TH1F("mainTimerHistogram","Main logger results;volume number;real time [s]",fMainToName.size(),0.5,fMainToName.size()+0.5);
    binNumber = 0;
    for (auto& p : fMainToTime) {
         mainTimerHistogram->SetBinContent(++binNumber,p.second);
         mainTimerHistogram->GetXaxis()->SetBinLabel  (  binNumber,fMainToName[p.first]);
     }
     mainTimerHistogram->DrawClone();
     mainTimerHistogram->Write();

    if ( outFile ) {
        outFile->Close();
        delete outFile;
    }

    gDirectory = prevFile;
}
//_____________________________________________________________________________

ClassImp(FairMonitor)

