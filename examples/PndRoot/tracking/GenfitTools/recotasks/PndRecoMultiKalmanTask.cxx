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

//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndRecoMultiKalmanTask
//      see PndRecoMultiKalmanTask.h for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//      Stefano Spataro, UNI Torino
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndRecoMultiKalmanTask.h"

// C/C++ Headers ----------------------
#include <iostream>
#include <cmath>

// Collaborating Class Headers --------
#include "TClonesArray.h"
#include "PndTrack.h"
#include "FairRootManager.h"
#include "FairGeanePro.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

PndRecoMultiKalmanTask::PndRecoMultiKalmanTask(const char *name, Int_t iVerbose, TString fithypo)
  : PndPersistencyTask(name, iVerbose), fTrackInBranchName(""), fTrackOutBranchName(""), fMvdBranchName(""), fCentralTrackerBranchName(""), fFitWithHypo(fithypo),
    fFitter(new PndRecoKalmanFit()), fUseGeane(kTRUE), fIdealHyp(kFALSE), fPropagateToIP(kFALSE), fPropagateDistance(2.f), fPerpPlane(kFALSE), fTrackRep(0), fNumIt(1), fBusyCut(20)
{
  for (int i = 0; i < 5; i++)
    fFitTrackArrays[i] = new TClonesArray("PndTrack");
  SetPersistency(kTRUE);
  fPDGs[0] = -11;
  fPDGs[1] = -13;
  fPDGs[2] = 211;
  fPDGs[3] = 321;
  fPDGs[4] = 2212;
}

PndRecoMultiKalmanTask::~PndRecoMultiKalmanTask() {}

InitStatus PndRecoMultiKalmanTask::Init()
{
  fFitter->SetGeane(fUseGeane);
  fFitter->SetPropagateToIP(fPropagateToIP);
  fFitter->SetPropagateDistance(fPropagateDistance);
  fFitter->SetPerpPlane(fPerpPlane);
  fFitter->SetNumIterations(fNumIt);
  fFitter->SetMvdBranchName(fMvdBranchName);
  fFitter->SetCentralTrackerBranchName(fCentralTrackerBranchName);
  fFitter->SetVerbose(fVerbose);
  fFitter->SetTrackRep(fTrackRep);
  if (!fFitter->Init())
    return kFATAL;

  // Get ROOT Manager
  FairRootManager *ioman = FairRootManager::Instance();

  if (ioman == 0) {
    Error("PndRecoMultiKalmanTask::Init", "RootManager not instantiated!");
    return kERROR;
  }

  // Get input collection
  fTrackArray = (TClonesArray *)ioman->GetObject(fTrackInBranchName);
  if (fTrackArray == nullptr) {
    Error("PndRecoMultiKalmanTask::Init", "track-array not found!");
    return kERROR;
  }
  unsigned int nfits = 0;
  for (int i = 0; i < 5; i++)
    fHypoFlag[i] = false;
  std::cout << " -I- PndRecoMultiKalmanTask::Init: \"" << fName.Data() << "\"" << std::endl;
  std::cout << " -I- PndRecoMultiKalmanTask::Init: Hyopthesis string is \"" << fFitWithHypo << "\"" << std::endl;
  std::cout << " -I- PndRecoMultiKalmanTask::Init: fTrackOutBranchName string is \"" << fTrackOutBranchName << "\"" << std::endl;
  if (fFitWithHypo.Contains("electron")) {
    nfits++;
    fHypoFlag[0] = true;
    std::cout << " -I- PndRecoMultiKalmanTask::Init: Hyopthesis electron " << std::endl;
    ioman->Register(fTrackOutBranchName + "Electron", "Gen" + fName, fFitTrackArrays[0], GetPersistency());
    std::cout << " -I- PndRecoMultiKalmanTask::Init: Hyopthesis electron registered" << std::endl;
  }
  if (fFitWithHypo.Contains("muon")) {
    nfits++;
    fHypoFlag[1] = true;
    std::cout << " -I- PndRecoMultiKalmanTask::Init: Hyopthesis muon " << std::endl;
    ioman->Register(fTrackOutBranchName + "Muon", "Gen" + fName, fFitTrackArrays[1], GetPersistency());
    std::cout << " -I- PndRecoMultiKalmanTask::Init: Hyopthesis muon registered" << std::endl;
  }
  if (fFitWithHypo.Contains("pion")) {
    nfits++;
    fHypoFlag[2] = true;
    std::cout << " -I- PndRecoMultiKalmanTask::Init: Hyopthesis pion " << std::endl;
    ioman->Register(fTrackOutBranchName + "Pion", "Gen" + fName, fFitTrackArrays[2], GetPersistency());
    std::cout << " -I- PndRecoMultiKalmanTask::Init: Hyopthesis pion registered" << std::endl;
  }
  if (fFitWithHypo.Contains("kaon")) {
    nfits++;
    fHypoFlag[3] = true;
    std::cout << " -I- PndRecoMultiKalmanTask::Init: Hyopthesis kaon " << std::endl;
    ioman->Register(fTrackOutBranchName + "Kaon", "Gen" + fName, fFitTrackArrays[3], GetPersistency());
    std::cout << " -I- PndRecoMultiKalmanTask::Init: Hyopthesis kaon registered" << std::endl;
  }
  if (fFitWithHypo.Contains("proton")) {
    nfits++;
    fHypoFlag[4] = true;
    std::cout << " -I- PndRecoMultiKalmanTask::Init: Hyopthesis proton " << std::endl;
    ioman->Register(fTrackOutBranchName + "Proton", "Gen" + fName, fFitTrackArrays[4], GetPersistency());
    std::cout << " -I- PndRecoMultiKalmanTask::Init: Hyopthesis proton registered" << std::endl;
  }
  if (nfits == 0) {
    std::cout << " -I- PndRecoMultiKalmanTask::Init: No hypotheses given, running kalman filter with all 5 hypothesis" << std::endl;
    for (int i = 0; i < 5; i++)
      fHypoFlag[i] = true;
    ioman->Register(fTrackOutBranchName + "Electron", "Gen" + fName, fFitTrackArrays[0], GetPersistency());
    ioman->Register(fTrackOutBranchName + "Muon", "Gen" + fName, fFitTrackArrays[1], GetPersistency());
    ioman->Register(fTrackOutBranchName + "Pion", "Gen" + fName, fFitTrackArrays[2], GetPersistency());
    ioman->Register(fTrackOutBranchName + "Kaon", "Gen" + fName, fFitTrackArrays[3], GetPersistency());
    ioman->Register(fTrackOutBranchName + "Proton", "Gen" + fName, fFitTrackArrays[4], GetPersistency());
  }
  std::cout << " -I- PndRecoMultiKalmanTask::Init: \"" << fName.Data() << "\" initilaised well with hypoflags {" << fHypoFlag[0] << "," << fHypoFlag[1] << "," << fHypoFlag[2]
            << "," << fHypoFlag[3] << "," << fHypoFlag[4] << ","
            << "}" << std::endl;

  return kSUCCESS;
}

void PndRecoMultiKalmanTask::SetParContainers()
{
  std::cout << " -I- PndRecoMultiKalmanTask:SetParContainers: \"" << fName.Data() << "\" " << std::endl;
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar *)rtdb->getContainer("PndGeoSttPar");
  fFtsParameters = (PndGeoFtsPar *)rtdb->getContainer("PndGeoFtsPar");
  std::cout << " -I- PndRecoMultiKalmanTask:SetParContainers: done" << std::endl;
}

void PndRecoMultiKalmanTask::Exec(Option_t *)
{
  if (fVerbose > 0)
    std::cout << "PndRecoMultiKalmanTask::Exec" << std::endl;

  for (int i = 0; i < 5; i++)
    fFitTrackArrays[i]->Delete();

  Int_t ntracks = fTrackArray->GetEntriesFast();

  // Detailed output
  if (fVerbose > 1)
    std::cout << " -I- PndRecoMultiKalmanTask: contains " << ntracks << " Tracks." << std::endl;

  // Cut too busy events TODO
  if (ntracks > fBusyCut) {
    std::cout << " -I- PndRecoMultiKalmanTask::Exec: ntracks=" << ntracks << " Evil Event! skipping" << std::endl;
    return;
  }

  for (Int_t itr = 0; itr < ntracks; ++itr) {
    if (fVerbose > 1)
      std::cout << "starting track" << itr << std::endl;
    PndTrack *prefitTrack = (PndTrack *)fTrackArray->At(itr);
    Int_t fCharge = prefitTrack->GetParamFirst().GetQ();

    for (int i = 0; i < 5; i++) {
      if (fHypoFlag[i]) {
        std::cout << "PndRecoMultiKalmanTask::Exec(): Start Hypothesis " << fPDGs[i] << std::endl;
        Int_t PDGCode = fPDGs[i] * fCharge;
        PndTrack *fitTrack = fFitter->Fit(prefitTrack, PDGCode);
        TClonesArray &trkRef = *fFitTrackArrays[i];
        Int_t size = trkRef.GetEntriesFast();
        new (trkRef[size]) PndTrack(fitTrack->GetParamFirst(), fitTrack->GetParamLast(), fitTrack->GetTrackCand(), fitTrack->GetFlag(), fitTrack->GetChi2(), fitTrack->GetNDF(),
                                    fitTrack->GetPidHypo(), itr, FairRootManager::Instance()->GetBranchId(fTrackInBranchName));
      }
    }

  } // end of track loop

  if (fVerbose > 0)
    std::cout << "Fitting done" << std::endl;

  return;
}

ClassImp(PndRecoMultiKalmanTask);
