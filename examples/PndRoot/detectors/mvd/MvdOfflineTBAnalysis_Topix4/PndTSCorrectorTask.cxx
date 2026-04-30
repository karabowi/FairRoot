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
// -----                PndTSCorrectorTaskT source file             -----
// -------------------------------------------------------------------------

#include "PndTSCorrectorTask.h"

#include "PndSdsHit.h"

#include "FairLink.h"        // for FairLink
#include "FairRootManager.h" // for FairRootManager
#include "FairTimeStamp.h"   // for FairTimeStamp

#include <iosfwd>         // for ostream
#include "TClass.h"       // for TClass
#include "TClonesArray.h" // for TClonesArray

#include <iostream> // for operator<<, cout, ostream, etc
#include <vector>   // for vector

InitStatus PndTSCorrectorTask::ReInit()
{
  return kSUCCESS;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndTSCorrectorTask::Init()
{

  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    std::cout << "-E- PndTSCorrectorTaskT::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  // Create and register output array
  fInputArray = (TClonesArray *)FairRootManager::Instance()->GetObject(fInputBranch);

  if (fVerbose > 1) {
    Info("Init", "Registering this branch: %s/%s", fFolder.Data(), fOutputBranch.Data());
  }
  fOutputArray = ioman->Register(fOutputBranch, fInputArray->GetClass()->GetName(), fFolder, fPersistance);

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndTSCorrectorTask::Exec(Option_t *)
{

  //  fInputArray = (FairRootManager::Instance()->GetTClonesArray(fInputBranch);
  if (fVerbose > 1 && ++fEntryNr % 1000 == 0) {
    LOG(info) << " PndTSCorrectorTask: Event " << fEntryNr << " Size PixelArray: " << fInputArray->GetEntriesFast();
  }
  Double_t timeOfLast = 0;
  for (int i = 0; i < fInputArray->GetEntriesFast(); i++) {
    PndSdsHit *myData = (PndSdsHit *)fInputArray->At(i);
    myData->SetTimeStamp(myData->GetTimeStamp() + fTSCorrectionMap[myData->GetSensorID()]);
    (*fOutputArray)[fOutputArray->GetEntries()] = myData->Clone();
    // delete(myData);
  }
  fOutputArray->Sort();
}

// -------------------------------------------------------------------------

void PndTSCorrectorTask::FinishEvent()
{
  fOutputArray->Delete();
}

void PndTSCorrectorTask::FinishTask() {}

ClassImp(PndTSCorrectorTask);
