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

// -------------------------------------------------------------------------
// -----                PndMvdEventMergerTask source file             -----
// -------------------------------------------------------------------------

#include "TClonesArray.h"
#include "TArrayD.h"

#include "FairRootManager.h"
#include "PndMvdEventMergerTask.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

// -----   Default constructor   -------------------------------------------
PndMvdEventMergerTask::PndMvdEventMergerTask() : FairTask("MVD Event Merger")
{
  LOG(error) << " PndMvdEventMergerTask: The default constructor should not be used!";
  fEventNr = 0;
}

PndMvdEventMergerTask::PndMvdEventMergerTask(TString signalBranch, TString bgFile, TString bgBranch, Int_t events, Int_t mergedEvents, Bool_t signalIsBg)
  : FairTask("MVD Event Merger")
{
  fSignalBranch = signalBranch;
  fBgFile = bgFile;
  fBgBranch = bgBranch;
  fNMergedEvents = mergedEvents;
  fSignalIsBg = signalIsBg;
  fEventNr = 0;
  fNEvents = events;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMvdEventMergerTask::~PndMvdEventMergerTask()
{
  delete (fMerger);
}
// -------------------------------------------------------------------------

// -----   Initialization  of Parameter Containers -------------------------
void PndMvdEventMergerTask::SetParContainers()
{
  // called before Init()
  // Get Base Container
  // FairRun* ana = FairRun::Instance();
  // FairRuntimeDb* rtdb=ana->GetRuntimeDb();
}

InitStatus PndMvdEventMergerTask::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMvdEventMergerTask::Init()
{
  // FairRun* ana = FairRun::Instance();
  FairRootManager *ioman = FairRootManager::Instance();

  if (!ioman) {
    std::cout << "-E- PndMvdEventMergerTask::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  if (fSignalIsBg == false) {
    fSignalArray = (TClonesArray *)ioman->GetObject(fSignalBranch);

    if (!fSignalArray) {
      std::cout << "-W- PndMvdEventMergerTask::Init: "
                << "No MVDPoint array!" << std::endl;
      return kERROR;
    }
  }

  // Create and register output array
  fMergedArray = new TClonesArray("PndTpcCluster");
  ioman->Register("PndTpcClusterMerged", "TPC", fMergedArray, kTRUE);

  fMerger = new PndMvdEventMerger(fBgFile, fBgBranch, fNEvents, fNMergedEvents);

  LOG(info) << " PndMvdEventMergerTask: Intialisation successfull";

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndMvdEventMergerTask::Exec(Option_t *)
{

  TClonesArray *bg;
  if (fEventNr < fNMergedEvents)
    bg = fMerger->GetEvent(fEventNr);
  else
    bg = fMerger->GetEvent(0);

  fMergedArray->Delete();

  if (fSignalIsBg == false) {
    fMerger->AddTClonesArray(bg, fSignalArray);
  }
  for (int i = 0; i < bg->GetEntriesFast(); i++) {
    new ((*fMergedArray)[i]) PndTpcCluster(*(PndTpcCluster *)bg->At(i));
  }

  // Event summary

  if (fVerbose > 0) {
    LOG(info) << " PndMvdEventMergerTask: Merged " << fMergedArray->GetEntriesFast() << " events in Event Nr: " << fEventNr;
  }
  fEventNr++;
}

ClassImp(PndMvdEventMergerTask);
