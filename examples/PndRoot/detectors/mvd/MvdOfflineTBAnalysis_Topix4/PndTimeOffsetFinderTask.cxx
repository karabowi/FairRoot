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
// -----                PndTimeOffsetFinderTaskT source file             -----
// -------------------------------------------------------------------------

#include "PndTimeOffsetFinderTask.h"

#include "FairLink.h"        // for FairLink
#include "FairRootManager.h" // for FairRootManager
#include "FairTimeStamp.h"   // for FairTimeStamp
#include "FairRunAna.h"

#include "PndSdsHit.h"

#include <iosfwd>         // for ostream
#include "TClass.h"       // for TClass
#include "TClonesArray.h" // for TClonesArray

#include <iostream> // for operator<<, cout, ostream, etc
#include <iomanip>
#include <vector> // for vector

InitStatus PndTimeOffsetFinderTask::ReInit()
{
  return kSUCCESS;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndTimeOffsetFinderTask::Init()
{

  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    std::cout << "-E- PndTimeOffsetFinderTaskT::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  // Create and register output array
  fInputArray = (TClonesArray *)FairRootManager::Instance()->GetObject("MVDHitsPixel");

  // if(fVerbose>1) { Info("Init","Registering this branch: %s/%s",fFolder.Data(),fOutputBranch.Data()); }
  // fOutputArray = ioman->Register(fOutputBranch, fInputArray->GetClass()->GetName(), fFolder, fPersistance);

  fHc0c1 = new TH1D("fHc0c1", "fHc0c1", 2000000, -5000000, 15000000);
  fHc0c2 = new TH1D("fHc0c2", "fHc0c2", 2000000, -5000000, 15000000);
  fHc0c3 = new TH1D("fHc0c3", "fHc0c3", 2000000, -5000000, 15000000);
  fHc1c2 = new TH1D("fHc1c2", "fHc1c2", 2000000, -5000000, 15000000);
  fHc1c3 = new TH1D("fHc1c3", "fHc1c3", 2000000, -5000000, 15000000);
  fHc2c3 = new TH1D("fHc2c3", "fHc2c3", 2000000, -5000000, 15000000);

  FairRootManager::Instance()->InitTSBuffer(fInputBranch, fFunctor);
  FairTSBufferFunctional *buffer = FairRootManager::Instance()->GetTSBuffer(fInputBranch);
  if (buffer != 0) {
    buffer->SetBranchIndex(fStartIndex);
    std::cout << "BranchIndex: " << buffer->GetBranchIndex() << std::endl;
  }

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndTimeOffsetFinderTask::Exec(Option_t *)
{
  if (FairRunAna::Instance()->IsTimeStamp() != kTRUE) {
    LOG(error) << " PndTimeOffsetFinderTask::Exec not running in time based mode.";
    return;
  }
  //	if (fFirstExecute){
  //		std::cout << "PndTimeOffsetFinderTask::Exec first execute called with time " << fStartTime << std::endl;
  //		fFirstExecute = kFALSE;
  //
  //		if (fStartIndex > 0){
  //			fInputArray = FairRootManager::Instance()->GetData(fInputBranch, fFunctor, 0);
  //
  //			FairTSBufferFunctional* buffer = FairRootManager::Instance()->GetTSBuffer(fInputBranch);
  //			if (buffer != 0){
  //				buffer->SetBranchIndex(fStartIndex);
  //				std::cout << "BranchIndex: " << buffer->GetBranchIndex() << std::endl;
  //			}
  //		}
  //		fInputArray = FairRootManager::Instance()->GetData(fInputBranch, fFunctor, fStartTime);
  //
  //		//std::cout << "fInputArray size: " << fInputArray->GetEntriesFast() << std::endl;
  //		fInputArray->Delete();
  //		fLastTimeStamp = fStartTime;
  //	}
  fInputArray = FairRootManager::Instance()->GetData(fInputBranch, fFunctor, fLastTimeStamp + fTimeOffset);
  // std::cout << "Exec called: " << fEntryNr++ << std::endl;
  for (int i = 0; i < fInputArray->GetEntriesFast(); i++) {
    PndSdsHit *second = (PndSdsHit *)fInputArray->At(i);
    //	std::cout << i << " : " << std::setw(12) << second->GetTimeStamp() << std::endl;
  }
  PndSdsHit *first;
  // std::cout << "NEntries " << fInputArray->GetEntriesFast() << std::endl;
  if (fInputArray->GetEntriesFast() > 1) {
    first = (PndSdsHit *)fInputArray->At(0);
    //		std::cout << "first: " << *first << std::endl;
    if (first != 0)
      fLastTimeStamp = first->GetTimeStamp();
  }
  fLastTimeStamp += fTimeOffset;

  if (fLastTimeStamp > fTerminateTime) {
    std::cout << "PndTimeOffsetFinderTask::Exec fLastTimeStamp " << fLastTimeStamp << " > TerminateTime " << fTerminateTime << std::endl;
    FairRootManager::Instance()->TerminateTSBuffer(fInputBranch);
    return;
  }
  if (fEntryNr++ % 1000 == 0)
    std::cout << "EventProcessed: " << fEntryNr << " fLastTimeStamp: " << fLastTimeStamp << " TeminateTime " << fTerminateTime << std::endl;

  if (fInputArray->GetEntriesFast() > 1) {
    // PndSdsHit* first = (PndSdsHit*)fInputArray->At(0);
    for (int i = 1; i < fInputArray->GetEntriesFast(); i++) {
      PndSdsHit *second = (PndSdsHit *)fInputArray->At(i);
      //	std::cout << "first: " << std::setw(12) << first->GetTimeStamp() << " second: " << std::setw(12) << second->GetTimeStamp() <<
      //			" diff " << second->GetTimeStamp() - first->GetTimeStamp() << std::endl;
      if (first->GetSensorID() == second->GetSensorID())
        continue;

      double diff;
      if (first->GetSensorID() < second->GetSensorID())
        diff = second->GetTimeStamp() - first->GetTimeStamp();
      else
        diff = first->GetTimeStamp() - second->GetTimeStamp();

      if ((first->GetSensorID() == 0 && second->GetSensorID() == 1) || (first->GetSensorID() == 1 && second->GetSensorID() == 0))
        fHc0c1->Fill(diff);
      else if ((first->GetSensorID() == 0 && second->GetSensorID() == 2) || (first->GetSensorID() == 2 && second->GetSensorID() == 0))
        fHc0c2->Fill(diff);
      else if ((first->GetSensorID() == 0 && second->GetSensorID() == 3) || (first->GetSensorID() == 3 && second->GetSensorID() == 0))
        fHc0c3->Fill(diff);
      else if ((first->GetSensorID() == 1 && second->GetSensorID() == 2) || (first->GetSensorID() == 2 && second->GetSensorID() == 1))
        fHc1c2->Fill(diff);
      else if ((first->GetSensorID() == 1 && second->GetSensorID() == 3) || (first->GetSensorID() == 3 && second->GetSensorID() == 1))
        fHc1c3->Fill(diff);
      else if ((first->GetSensorID() == 2 && second->GetSensorID() == 3) || (first->GetSensorID() == 3 && second->GetSensorID() == 2))
        fHc2c3->Fill(diff);
    }
    TClonesArray *tempArray = new TClonesArray(fInputArray->GetClass()->GetName());
    tempArray->AbsorbObjects(fInputArray, 1, fInputArray->GetEntriesFast() - 1);
    fInputArray->Delete();
    fInputArray->AbsorbObjects(tempArray, 0, tempArray->GetEntriesFast() - 1);
    delete (tempArray);
    // std::cout << "RemoveFirst: " << fInputArray->GetEntriesFast() << std::endl;
  }
}

// -------------------------------------------------------------------------

void PndTimeOffsetFinderTask::FinishEvent()
{
  // fOutputArray->Delete();
}

void PndTimeOffsetFinderTask::FinishTask()
{
  fHc0c1->Write();
  fHc0c2->Write();
  fHc0c3->Write();
  fHc1c2->Write();
  fHc1c3->Write();
  fHc2c3->Write();
}

ClassImp(PndTimeOffsetFinderTask);
