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


#include "PndGFRecoTask.h"
#include "PndMCTrack.h"

#include <TDatabasePDG.h>
#include <TParticlePDG.h>

#include "FairRuntimeDb.h"
#include "FairRunAna.h"

PndGFRecoTask::PndGFRecoTask(const char *name, Int_t iVerbose) : PndPersistencyTask(name, iVerbose) {}

PndGFRecoTask::~PndGFRecoTask()
{
  if (fPdg != nullptr) {
    delete (fPdg);
  }
}

void PndGFRecoTask::SetMultiPID()
{
  fPidHypothesis.push_back(PndGFPidHypo(11));
  fPidHypothesis.push_back(PndGFPidHypo(13));
  fPidHypothesis.push_back(PndGFPidHypo(211));
  fPidHypothesis.push_back(PndGFPidHypo(321));
  fPidHypothesis.push_back(PndGFPidHypo(2212));
}

InitStatus PndGFRecoTask::Init()
{
  FairRootManager *ioman = FairRootManager::Instance();

  fPrefitTrack = static_cast<TClonesArray *>(ioman->GetObject(fInputBranchName));
  if (fPrefitTrack == nullptr) {
    LOG(error) << "No input TClonesArray for prefit track " << fInputBranchName.Data();
    return kFATAL;
  }

  if (fIdealPID == true) {
    fPdg = new TDatabasePDG;

    fMCTrack = static_cast<TClonesArray *>(ioman->GetObject("MCTrack"));
    if (fMCTrack == nullptr) {
      LOG(error) << "No input TClonesArray for MC track";
      return kFATAL;
    }
    PndGFPidHypo hyp(fDefaultPID);
    fPidHypothesis.push_back(hyp);
  }

  //  fFitTrack = ioman->Register(fOutputBranchName, "PndTrack", "Gen", GetPersistency());

  if (fPidHypothesis.size() == 0) { // if no PidHypothesis is given
    PndGFPidHypo hyp(fDefaultPID);  // default PID is taken.
    fPidHypothesis.push_back(hyp);
  }

  if (fPidHypothesis.size() == 1) { // if you just have one hypothesis add no additional name to the output branch name
    fPidHypothesis[0].fArray = ioman->Register(fOutputBranchName, "PndTrack", "Gen", GetPersistency());
  } else {
    for (auto &hypo : fPidHypothesis) {
      hypo.fArray = ioman->Register(fOutputBranchName + hypo.fParticleName, "PndTrack", "Gen", GetPersistency());
    }
  }

  fFit.Init();
  if (fUseEventDisplay == true) {
    fFit.UseEventDisplay();
  }

  return kSUCCESS;
}

void PndGFRecoTask::SetFitter(genfit::AbsKalmanFitter *fitter)
{
  fFit.SetFitter(fitter);
}

void PndGFRecoTask::SetParContainers()
{
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  rtdb->getContainer("PndGeoSttPar");
  rtdb->getContainer("PndGeoFtsPar");
}

void PndGFRecoTask::Exec(Option_t *)
{
  DeleteOldTracks();
  if (fPrefitTrack->GetEntries() > fNBusyCut) {
    LOG(info) << "Event skipped: " << fPrefitTrack->GetEntries() << " tracks > " << fNBusyCut;
    return;
  }

  for (int i = 0; i < fPrefitTrack->GetEntries(); i++) {
    PndTrack *track = static_cast<PndTrack *>(fPrefitTrack->At(i));
    int charge = track->GetParamFirst().GetQ();

    for (auto pidHyp : fPidHypothesis) {
      Int_t PDGCode = 0;
      if (fIdealPID) {
        PDGCode = GetIdealPdgCode(track);
      } // end of ideal hyp condition
      else {
        PDGCode = pidHyp.fPdgNumber * charge;
      }

      PndTrack *fitTrack = fFit.Fit(track, PDGCode);

      new ((*pidHyp.fArray)[pidHyp.fArray->GetEntries()]) PndTrack(*fitTrack);
    }
  }
  if (fUseEventDisplay == true) {
    genfit::EventDisplay::getInstance()->open();
  }
}

void PndGFRecoTask::AddParticleHypo(int pdgCode)
{
  PndGFPidHypo hypo(pdgCode);
  if (hypo.fPdgNumber == 0) {
    LOG(fatal) << "Wrong PiD hypothesis set " << pdgCode;
  } else {
    fPidHypothesis.push_back(hypo);
  }
}

int PndGFRecoTask::GetIdealPdgCode(PndTrack *track)
{
  int charge = track->GetParamFirst().GetQ();
  int PDGCode = 0;
  std::vector<FairLink> mcTrackLinks = track->GetSortedMCTracks();
  //      PndTrackID *prefitTrackID = (PndTrackID*) fTrackIDArray->At(itr);
  if (mcTrackLinks.size() > 0) {
    Int_t mcTrackId = mcTrackLinks[0].GetIndex();
    if (mcTrackId != -1) {
      PndMCTrack *mcTrack = (PndMCTrack *)fMCTrack->At(mcTrackId); // TODO: Replace with GetCloneOfLinkData to run time-based
      if (!mcTrack) {
        PDGCode = fDefaultPID * charge;
        LOG(info) << " PndGFRecoTask::Exec: MCTrack #" << mcTrackId << " is not existing!! Trying with pion hyp";
      } else {
        PDGCode = mcTrack->GetPdgCode();
      }
      if (PDGCode >= 100000000) {
        PDGCode = fDefaultPID * charge;
        LOG(info) << " PndGFRecoTask::Exec: Track is an ion (PDGCode>100000000)! Trying with pion hyp";
      } else if ((static_cast<TParticlePDG *>(fPdg->GetParticle(PDGCode))->Charge()) == 0) {
        PDGCode = fDefaultPID * charge;
        LOG(error) << " PndGFRecoTask::Exec: Track MC charge is 0!!!! Trying with pion hyp";
      }
    } // end of MCTrack ID != -1
    else {
      PDGCode = fDefaultPID * charge;
      LOG(error) << " PndGFRecoTask::Exec: No MCTrack index in PndTrackID!! Trying with pion hyp";
    }
  } // end of "at least one correlated mc index"
  else {
    PDGCode = fDefaultPID * charge;
    LOG(error) << " PndGFRecoTask::Exec: No Correlated MCTrack id in PndTrackID!! Trying with pion hyp";
  }
  return PDGCode;
}
ClassImp(PndGFRecoTask);
