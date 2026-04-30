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
//      Implementation of class PndRecoMultiKalmanTask2
//      see PndRecoMultiKalmanTask2.h for details
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
#include "PndRecoMultiKalmanTask2.h"

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

PndRecoMultiKalmanTask2::PndRecoMultiKalmanTask2(const char *name, Int_t iVerbose, TString fithypo)
  : PndPersistencyTask(name, iVerbose), fTrackInBranchName(""), fTrackOutBranchName(""), fMvdBranchName(""), fCentralTrackerBranchName(""), fFitWithHypo(fithypo),
    fFitter(new PndRecoKalmanFit2()), fUseGeane(kTRUE), fIdealHyp(kFALSE), fNumIt(1), fBusyCut(20)

{
  for (int i = 0; i < 5; i++)
    fFitTrackArrays[i] = new TClonesArray("PndTrack");
  SetPersistency(kTRUE);
  fPDGs[0] = -11;
  fPDGs[1] = -13;
  fPDGs[2] = 211;
  fPDGs[3] = 321;
  fPDGs[4] = 2212;
  SetVerbose(5); // FIXME
  fVerbose = 5;
  // fFitter->SetVerbose(5); //FIXME
}

PndRecoMultiKalmanTask2::~PndRecoMultiKalmanTask2() {}

InitStatus PndRecoMultiKalmanTask2::Init()
{
  // fFitter->SetGeane(fUseGeane);
  // fFitter->SetPropagateToIP(fPropagateToIP);
  // fFitter->SetPropagateDistance(fPropagateDistance);
  // fFitter->SetPerpPlane(fPerpPlane);
  fFitter->SetNumIterations(fNumIt);
  // fFitter->SetMvdBranchName(fMvdBranchName);
  // fFitter->SetCentralTrackerBranchName(fCentralTrackerBranchName);
  // fFitter->SetVerbose(fVerbose);
  // fFitter->SetTrackRep(fTrackRep);
  if (!fFitter->Init())
    return kFATAL;

  // Get ROOT Manager
  FairRootManager *ioman = FairRootManager::Instance();

  if (ioman == 0) {
    Error("PndRecoMultiKalmanTask2::Init", "RootManager not instantiated!");
    return kERROR;
  }

  // Get input collection
  fTrackArray = (TClonesArray *)ioman->GetObject(fTrackInBranchName);
  if (fTrackArray == nullptr) {
    Error("PndRecoMultiKalmanTask2::Init", "track-array not found!");
    return kERROR;
  }

  unsigned int nfits = 0;
  for (int i = 0; i < 5; i++)
    fHypoFlag[i] = false;
  LOG(info) << " PndRecoMultiKalmanTask2::Init: \"" << fName.Data() << "\"";
  LOG(info) << " PndRecoMultiKalmanTask2::Init: Hyopthesis string is \"" << fFitWithHypo << "\"";
  LOG(info) << " PndRecoMultiKalmanTask2::Init: fTrackOutBranchName string is \"" << fTrackOutBranchName << "\"";
  if (fFitWithHypo.Contains("electron")) {
    nfits++;
    fHypoFlag[0] = true;
    LOG(info) << " PndRecoMultiKalmanTask2::Init: Hyopthesis electron ";
    ioman->Register(fTrackOutBranchName + "Electron", "Gen" + fName, fFitTrackArrays[0], GetPersistency());
    LOG(info) << " PndRecoMultiKalmanTask2::Init: Hyopthesis electron registered";
  }
  if (fFitWithHypo.Contains("muon")) {
    nfits++;
    fHypoFlag[1] = true;
    LOG(info) << " PndRecoMultiKalmanTask2::Init: Hyopthesis muon ";
    ioman->Register(fTrackOutBranchName + "Muon", "Gen" + fName, fFitTrackArrays[1], GetPersistency());
    LOG(info) << " PndRecoMultiKalmanTask2::Init: Hyopthesis muon registered";
  }
  if (fFitWithHypo.Contains("pion")) {
    nfits++;
    fHypoFlag[2] = true;
    LOG(info) << " PndRecoMultiKalmanTask2::Init: Hyopthesis pion ";
    ioman->Register(fTrackOutBranchName + "Pion", "Gen" + fName, fFitTrackArrays[2], GetPersistency());
    LOG(info) << " PndRecoMultiKalmanTask2::Init: Hyopthesis pion registered";
  }
  if (fFitWithHypo.Contains("kaon")) {
    nfits++;
    fHypoFlag[3] = true;
    LOG(info) << " PndRecoMultiKalmanTask2::Init: Hyopthesis kaon ";
    ioman->Register(fTrackOutBranchName + "Kaon", "Gen" + fName, fFitTrackArrays[3], GetPersistency());
    LOG(info) << " PndRecoMultiKalmanTask2::Init: Hyopthesis kaon registered";
  }
  if (fFitWithHypo.Contains("proton")) {
    nfits++;
    fHypoFlag[4] = true;
    LOG(info) << " PndRecoMultiKalmanTask2::Init: Hyopthesis proton ";
    ioman->Register(fTrackOutBranchName + "Proton", "Gen" + fName, fFitTrackArrays[4], GetPersistency());
    LOG(info) << " PndRecoMultiKalmanTask2::Init: Hyopthesis proton registered";
  }
  if (nfits == 0) {
    LOG(info) << " PndRecoMultiKalmanTask2::Init: No hypotheses given, running kalman filter with all 5 hypothesis";
    for (int i = 0; i < 5; i++)
      fHypoFlag[i] = true;
    ioman->Register(fTrackOutBranchName + "Electron", "Gen" + fName, fFitTrackArrays[0], GetPersistency());
    ioman->Register(fTrackOutBranchName + "Muon", "Gen" + fName, fFitTrackArrays[1], GetPersistency());
    ioman->Register(fTrackOutBranchName + "Pion", "Gen" + fName, fFitTrackArrays[2], GetPersistency());
    ioman->Register(fTrackOutBranchName + "Kaon", "Gen" + fName, fFitTrackArrays[3], GetPersistency());
    ioman->Register(fTrackOutBranchName + "Proton", "Gen" + fName, fFitTrackArrays[4], GetPersistency());
  }
  LOG(info) << " PndRecoMultiKalmanTask2::Init: \"" << fName.Data() << "\" initilaised well with hypoflags {" << fHypoFlag[0] << "," << fHypoFlag[1] << "," << fHypoFlag[2] << ","
            << fHypoFlag[3] << "," << fHypoFlag[4] << ","
            << "}" << std::endl;
  return kSUCCESS;
}

void PndRecoMultiKalmanTask2::SetParContainers()
{
  LOG(info) << " PndRecoMultiKalmanTask2:SetParContainers: \"" << fName.Data() << "\" ";
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar *)rtdb->getContainer("PndGeoSttPar");
  fFtsParameters = (PndGeoFtsPar *)rtdb->getContainer("PndGeoFtsPar");
  LOG(info) << " PndRecoMultiKalmanTask2:SetParContainers: done";
}

void PndRecoMultiKalmanTask2::Exec(Option_t *)
{
  if (fVerbose > 0)
    std::cout << "PndRecoMultiKalmanTask2::Exec";

  for (int i = 0; i < 5; i++)
    fFitTrackArrays[i]->Delete();

  Int_t ntracks = fTrackArray->GetEntriesFast();

  // Detailed output
  if (fVerbose > 1)
    LOG(info) << " PndRecoMultiKalmanTask2: contains " << ntracks << " Tracks.";

  // Cut too busy events TODO
  if (ntracks > fBusyCut) {
    LOG(info) << " PndRecoMultiKalmanTask2::Exec: ntracks=" << ntracks << " Evil Event! skipping";
    return;
  }

  for (Int_t itr = 0; itr < ntracks; ++itr) {
    if (fVerbose > 1)
      std::cout << "starting track" << itr << std::endl;
    PndTrack *prefitTrack = (PndTrack *)fTrackArray->At(itr);
    Int_t fCharge = prefitTrack->GetParamFirst().GetQ();

    for (int i = 0; i < 5; i++) {
      if (fHypoFlag[i]) {
        if (fVerbose > 2)
          std::cout << "PndRecoMultiKalmanTask2::Exec(): Start Hypothesis " << fPDGs[i];
        Int_t PDGCode = fPDGs[i] * fCharge;
        PndTrack *fitTrack = nullptr;
        try {
          fitTrack = fFitter->Fit(prefitTrack, PDGCode);
        } catch (...) {
          LOG(warning) << "PndRecoMultiKalmanTask2::Exec(): Track fit aborted";
          continue;
        }
        if (!fitTrack) {
          LOG(warning) << "PndRecoMultiKalmanTask2::Exec(): Caution, we got a track nullptr! <---------";
          continue;
        }
        TClonesArray &trkRef = *fFitTrackArrays[i];
        Int_t size = trkRef.GetEntriesFast();
        new (trkRef[size]) PndTrack(fitTrack->GetParamFirst(), fitTrack->GetParamLast(), fitTrack->GetTrackCand(), fitTrack->GetFlag(), fitTrack->GetChi2(), fitTrack->GetNDF(),
                                    fitTrack->GetPidHypo(), itr, FairRootManager::Instance()->GetBranchId(fTrackInBranchName));
      }
    }

  } // end of track loop

  if (fVerbose > 0)
    LOG(info) << "Track Fitting done";

  return;
}

ClassImp(PndRecoMultiKalmanTask2);
