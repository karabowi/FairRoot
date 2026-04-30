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

/*
 * PndTrackSmearTask.cpp
 *
 *  Created on: Jun 26, 2013
 *      Author: schumann
 */

// Root includes
#include "TROOT.h"
#include "TClonesArray.h"
#include "FairRootManager.h"
#include "PndTrack.h"
#include "PndTrackSmearTask.h"
#include "TRandom.h"

ClassImp(PndTrackSmearTask);

void PndTrackSmearTask::SetParContainers() {}

InitStatus PndTrackSmearTask::Init()
{

  FairRootManager *ioman = FairRootManager::Instance();

  if (!ioman) {
    std::cout << "-E- PndTrackSmearTask::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  // get track branches
  for (TString branchname : fInputTrackBranches) {
    std::cout << "PndTrackSmearTask::Init(): Adding track branch " << branchname.Data() << std::endl;
    fTracks.push_back((TClonesArray *)ioman->GetObject(branchname));
  }

  // create output branches
  for (TString branchname : fOutputTrackBranches) {
    fOutputTracks.push_back(ioman->Register(branchname, "PndTrack", "Tracks", GetPersistency()));
  }

  return kSUCCESS;
}

void PndTrackSmearTask::Exec(Option_t *)
{

  if (fVerbose > 1) {
    std::cout << "============= Begin PndTrackSmearTask::Exec" << std::endl;
    std::cout << std::endl;
    std::cout << "Array size check: fTracks (" << fTracks.size() << ")  fOutputTracks (" << fOutputTracks.size() << ")  fInputTrackBranches (" << fInputTrackBranches.size() << ")"
              << std::endl;
  }
  for (TClonesArray *arr : fOutputTracks) {
    arr->Delete();
  }

  for (unsigned int i = 0; i < fTracks.size(); i++) {
    TClonesArray *trkArray = fTracks[i];
    if (trkArray == nullptr)
      continue;
    TClonesArray *trkOutArray = fOutputTracks[i];
    if (trkOutArray == nullptr)
      continue;
    TString trkbranchname = fInputTrackBranches[i];
    for (int j = 0; j < trkArray->GetEntriesFast(); j++) {
      PndTrack *myTrack = (PndTrack *)trkArray->At(j);
      // PndTrackCand* myTrackCand=myTrack->GetPndTrackCandViaRef();
      new ((*trkOutArray)[j]) PndTrack(SmearTrackPar(myTrack->GetParamFirst()), SmearTrackPar(myTrack->GetParamLast()), myTrack->GetTrackCand(), myTrack->GetFlag(),
                                       myTrack->GetChi2(), myTrack->GetNDF(), myTrack->GetPidHypo(), j, FairRootManager::Instance()->GetBranchId(trkbranchname));
    }
  }
}

FairTrackParP PndTrackSmearTask::SmearTrackPar(FairTrackParP par)
{
  TVector3 mom = par.GetMomentum();
  Double_t Cov66[6][6];
  par.GetMARSCov(Cov66);
  SmearMom(mom, Cov66);

  FairTrackParP result(par.GetPosition(), mom, Cov66, par.GetQ(), par.GetOrigin(), par.GetJVer(), par.GetKVer());

  return result;
}

void PndTrackSmearTask::SmearMom(TVector3 &vec, Double_t Cov66[6][6])
{
  // gaussian smearing
  Double_t rannn = 0.;
  rannn = gRandom->Gaus(vec.X(), vec.X() * fMomSigma.X());
  vec.SetX(rannn);

  rannn = gRandom->Gaus(vec.Y(), vec.Y() * fMomSigma.Y());
  vec.SetY(rannn);

  rannn = gRandom->Gaus(vec.Z(), vec.Z() * fMomSigma.Z());
  vec.SetZ(rannn);

  Cov66[3][3] += vec.X() * vec.X() * fMomSigma.X() * fMomSigma.X();
  Cov66[4][4] += vec.Y() * vec.Y() * fMomSigma.Y() * fMomSigma.Y();
  Cov66[5][5] += vec.Z() * vec.Z() * fMomSigma.Z() * fMomSigma.Z();

  return;
}

void PndTrackSmearTask::FinishEvent()
{
  // if(fTracks != 0) fTracks->Delete();
}

void PndTrackSmearTask::Finish() {}
