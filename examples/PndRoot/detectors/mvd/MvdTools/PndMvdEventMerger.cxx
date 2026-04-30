//****************************************************************************
//*                   This file is part of PandaRoot.                        *
//*                                                                          *
//*            PandaRoot is distributed under the terms of the               *
//*              GNU General Public License (GPL) version 3,                 *
//*                 copied verbatim in the file "LICENSE".                   *
//*                                                                          *
//*  Copyright (C) 2006 - 2024 FAIR GmbH and copyright holders of PandaRoot  *
//*     The copyright holders are listed in the file "COPYRIGHTHOLDERS".     *
//*               The authors are listed in the file "AUTHORS".              *
//****************************************************************************

#include "PndMvdEventMerger.h"
#include "TFile.h"
#include "TTree.h"

#include <iostream>
#include "PndSdsMCPoint.h"
#include "PndSdsDigiStrip.h"
#include "PndSdsDigiPixel.h"
#include "FairLogger.h"
using std::cout;
using std::endl;

PndMvdEventMerger::PndMvdEventMerger()
{
  // fBuffer     = 0;
  // fBuffer = new TObjArray();
}
PndMvdEventMerger::PndMvdEventMerger(TString fileName, TString branchName, Int_t nEvents, Int_t nMerged)
{
  //	fBuffer = new TObjArray();
  if (!FillBuffer(fileName, branchName, nEvents, nMerged))
    cout << "-E- PndMvdEventMerger: Error in filling buffer";
}
PndMvdEventMerger::~PndMvdEventMerger()
{
  // fBuffer->Delete();
  // delete fBuffer;
  // fBuffer = 0;
}

TClonesArray *PndMvdEventMerger::GetEvent(Int_t iEvent)
{
  // if ( !fBuffer)
  //	std::cout << "-E- CbmMvdPileupManager::GetEvent: No event buffer!";

  if ((unsigned int)iEvent > fBuffer.size()) {
    LOG(warn) << " CbmMvdPileupManager::GetEvent: Event " << iEvent << " not present in buffer! ";
    cout << "                                   Returning nullptr pointer! " << endl;
    return nullptr;
  }

  TClonesArray *pArray = (TClonesArray *)fBuffer[iEvent];

  if (!pArray) {
    cout << "-W CbmMvdPileupManager::GetEvent: Returning nullptr pointer!" << endl;
    return nullptr;
  }

  return pArray;
}

Int_t PndMvdEventMerger::FillBuffer(TString fileName, TString branchName, Int_t nEvents, Int_t nMerged)
{
  // if ( !fBuffer)
  //	cout << "-E- Fill Buffer: No event buffer!";

  // fBuffer->Delete();

  TClonesArray *pointArray = nullptr;
  TClonesArray *mergedPointsArray = nullptr;
  // TFile* saveGFile = gFile;

  TFile *bgfile = new TFile(fileName);
  if (!bgfile) {
    LOG(warn) << " CbmMvdPileupManager::FillBuffer:  Background file " << fileName << " could noy be opened! ";
    return 0;
  }
  LOG(info) << " CbmMvdPileupManager::FillBuffer: Opening file ";
  cout << fileName << endl;

  TTree *bgtree = (TTree *)bgfile->Get("pndsim");
  if (!bgtree) {
    cout << "-W- CbmMvdPileupManager::FillBuffer:  "
         << "Could not find pndsim tree in background file " << endl;
    return 0;
  }

  Int_t nEventsInFile = bgtree->GetEntries();
  LOG(info) << " CbmMvdPileupManager::FillBuffer: " << nEventsInFile << " events in file";
  Int_t nBuffer = TMath::Min(nEvents, nEventsInFile);
  LOG(info) << " CbmMvdPileupManager::FillBuffer: Buffering " << nBuffer << " events";

  Int_t nMergedBuffer = TMath::Min(nMerged, nEventsInFile);

  bgtree->SetBranchAddress(branchName, &pointArray);

  for (Int_t iEvent = 0; iEvent < nMergedBuffer; iEvent++) {
    bgtree->GetEntry(iEvent);
    if (iEvent == 0)
      mergedPointsArray = (TClonesArray *)pointArray->Clone();
    else {
      std::cout << "New size of mergedPointsArray: " << AddTClonesArray(mergedPointsArray, pointArray) << std::endl;
    }
  }

  // fBuffer = new TObjArray(nBuffer,0);
  for (Int_t iEvent = 0; iEvent < nBuffer; iEvent++) {
    std::cout << "Adding Event: " << iEvent << " to buffer!" << std::endl;
    std::cout << "Merged PointsArray Size: " << mergedPointsArray->GetEntriesFast() << std::endl;
    // std::cout << fBuffer << std::endl;
    fBuffer.push_back((TClonesArray *)mergedPointsArray);
  }

  //	delete bgtree;
  bgfile->Close();
  //	delete bgfile;
  //	saveGFile->cd();

  return nBuffer;
}

Int_t PndMvdEventMerger::AddTClonesArray(TClonesArray *target, TClonesArray *source)
{
  Int_t origSize = target->GetEntriesFast();
  for (int i = 0; i < source->GetEntriesFast(); i++) {
    std::cout << source->GetClass()->GetName() << std::endl;
    TString myClassName(source->GetClass()->GetName());
    if (myClassName.Contains("PndSdsMCPoint"))
      new ((*target)[origSize + i]) PndSdsMCPoint(*(PndSdsMCPoint *)(source->At(i)));
    else if (myClassName.Contains("PndSdsDigiPixel"))
      new ((*target)[origSize + i]) PndSdsDigiPixel(*(PndSdsDigiPixel *)(source->At(i)));
    else if (myClassName.Contains("PndSdsDigiStrip"))
      new ((*target)[origSize + i]) PndSdsDigiStrip(*(PndSdsDigiStrip *)(source->At(i)));
    //		else if (myClassName.Contains("PndTpcPoint"))
    //			new ((*target)[origSize + i])PndTpcPoint(*(PndTpcPoint*)(source->At(i)));
    //		else if (myClassName.Contains("PndTpcCluster"))
    //			new ((*target)[origSize + i])PndTpcCluster(*(PndTpcCluster*)(source->At(i)));
    else
      std::cout << "The Class: " << myClassName << " is unknown!" << std::endl;
  }
  return target->GetEntriesFast();
}

ClassImp(PndMvdEventMerger);
