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

/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
// -------------------------------------------------------------------------
// -----                PndMapSorterTaskT source file             -----
// -------------------------------------------------------------------------

#include "PndMapSorterTask.h"

#include "FairLink.h"        // for FairLink
#include "FairRootManager.h" // for FairRootManager
#include "FairTimeStamp.h"   // for FairTimeStamp

#include <iosfwd>         // for ostream
#include "TClass.h"       // for TClass
#include "TClonesArray.h" // for TClonesArray

#include <iostream> // for operator<<, cout, ostream, etc
#include <vector>   // for vector

InitStatus PndMapSorterTask::ReInit()
{
  return kSUCCESS;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMapSorterTask::Init()
{

  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    std::cout << "-E- PndMapSorterTaskT::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  // Create and register output array
  fInputArray = (TClonesArray *)FairRootManager::Instance()->GetObject(fInputBranch);
  if (fInputArray == nullptr) {
    std::cout << "InputBranch " << fInputBranch << " not available!" << std::endl;
  }

  if (fVerbose > 1) {
    Info("Init", "Registering this branch: %s/%s", fFolder.Data(), fOutputBranch.Data());
  }
  fOutputArray = ioman->Register(fOutputBranch, fInputArray->GetClass()->GetName(), fFolder, fPersistance);

  fSorter = new PndMapSorter(fTimeOffset);

  SetVerbose(2);

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndMapSorterTask::Exec(Option_t *)
{

  // fInputArray = FairRootManager::Instance()->GetTClonesArray(fInputBranch);
  if (fVerbose > 1 && fEntryNr % 10 == 0) {
    LOG(info) << " PndMapSorterTask: " << fEntryNr << " Size PixelArray: " << fInputArray->GetEntriesFast();
  }
  Double_t timeOfLast = 0;
  for (int i = 0; i < fInputArray->GetEntriesFast(); i++) {
    FairTimeStamp *myData = (FairTimeStamp *)fInputArray->At(i);
    myData->SetEntryNr(FairLink(0, fEntryNr, fInputBranch, i));
    if (fVerbose > 2) {
      std::cout << "Sorter filled with: ";
      myData->Print();
      std::cout << std::endl;
    }
    fSorter->AddElement(myData, myData->GetTimeStamp());
    timeOfLast = myData->GetTimeStamp();
  }
  if (fVerbose > 2) {
    fSorter->print();
  }

  fSorter->WriteOutData(timeOfLast);
  std::vector<FairTimeStamp *> sortedData = fSorter->GetOutputData();

  // fOutputArray = FairRootManager::Instance()->GetEmptyTClonesArray(fOutputBranch);
  // std::cout << "SortedData size: " << sortedData.size() << std::endl;
  for (int i = 0; i < sortedData.size(); i++) {
    AddNewDataToTClonesArray(sortedData[i]);
  }
  fSorter->DeleteOutputData();
  fEntryNr++;
}

void PndMapSorterTask::AddNewDataToTClonesArray(FairTimeStamp *data)
{
  // FairRootManager* ioman = FairRootManager::Instance();
  // TClonesArray* myArray = ioman->GetTClonesArray(fOutputBranch);
  (*fOutputArray)[fOutputArray->GetEntries()] = data->Clone();
  delete (data);
}

// -------------------------------------------------------------------------

void PndMapSorterTask::FinishEvent()
{
  fOutputArray->Delete();
}

void PndMapSorterTask::FinishTask()
{
  // fInputArray = FairRootManager::Instance()->GetTClonesArray(fInputBranch);
  if (fVerbose > 2) {
    LOG(info) << " PndMapSorterTaskT::FinishTask Size InputArray: " << fInputArray->GetEntriesFast();
  }
  for (int i = 0; i < fInputArray->GetEntriesFast(); i++) {
    FairTimeStamp *myDigi = (FairTimeStamp *)fInputArray->At(i);
    fSorter->AddElement(myDigi, ((FairTimeStamp *)myDigi)->GetTimeStamp());
  }
  fSorter->print();
  fSorter->WriteOutAll();
  std::vector<FairTimeStamp *> sortedData = fSorter->GetOutputData();

  std::cout << "PndMapSorterTask::FinishTask sortedData.size(): " << sortedData.size() << std::endl;

  for (int i = 0; i < sortedData.size(); i++) {
    if (fVerbose > 2) {
      std::cout << i << " FinishTask : ";
      sortedData[i]->Print();
      std::cout << std::endl;
    }

    AddNewDataToTClonesArray(sortedData[i]);
  }
  fSorter->DeleteOutputData();
  if (fVerbose > 2) {
    fSorter->print();
  }
  FairRootManager::Instance()->SetLastFill();
}

ClassImp(PndMapSorterTask);
