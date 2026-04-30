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

#include "PndRichDbTask.h"
#include "PndRichReco.h"

// fairroot
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairGeanePro.h"
#include "FairGeaneUtil.h"
#include "FairTrackParP.h"
#include "FairLogger.h"

// general
#include <iostream>
#include <cmath>

using namespace std;

// -----   Default constructor   -------------------------------------------
PndRichDbTask::PndRichDbTask() : FairTask("Rich Reco task")
{
  fPersistence = kTRUE;
  fVerbose = 1;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndRichDbTask::~PndRichDbTask() {}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndRichDbTask::Init()
{

  FairRootManager *ioman = FairRootManager::Instance();

  if (!ioman) {
    cout << "-E- PndRichDbTask: "
         << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  LOG(info) << " PndRichDbTask: Intialisation successfull ";
  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndRichDbTask::Exec(Option_t *)
{
  if (fVerbose > 0) {
    // cout << "==================== EVENT " << evt << endl;
  }
}

void PndRichDbTask::FinishEvent() {}

void PndRichDbTask::FinishTask() {}

ClassImp(PndRichDbTask)
