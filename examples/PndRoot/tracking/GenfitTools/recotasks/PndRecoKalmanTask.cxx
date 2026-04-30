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
//      Implementation of class PndRecoKalmanTask
//      see PndRecoKalmanTask.hh for details
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
#include "PndRecoKalmanTask.h"

// C/C++ Headers ----------------------
#include <iostream>
#include <cmath>

// Collaborating Class Headers --------
#include "TClonesArray.h"
#include "TParticlePDG.h"
#include "PndTrack.h"
#include "PndTrackID.h"
#include "PndMCTrack.h"
#include "FairRootManager.h"
#include "FairGeanePro.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

PndRecoKalmanTask::PndRecoKalmanTask(const char *name, Int_t iVerbose)
  : PndPersistencyTask(name, iVerbose), fFitTrackArray(), fTrackInBranchName(""), fTrackOutBranchName(""), fMvdBranchName(""), fCentralTrackerBranchName(""), fFitter(),
    fDafFitter(), fPersistence(kTRUE), fUseGeane(kTRUE), fIdealHyp(kFALSE), fDaf(kFALSE), fPropagateToIP(kFALSE), fPropagateDistance(2.f), fPerpPlane(kFALSE), fTrackRep(0),
    fNumIt(1), fPDGHyp(211), fBusyCut(20)
{
  fFitTrackArray = new TClonesArray("PndTrack");
  fFitter = new PndRecoKalmanFit();
  fDafFitter = new PndRecoDafFit();
  SetPersistency(kTRUE);
}

PndRecoKalmanTask::~PndRecoKalmanTask() {}

InitStatus PndRecoKalmanTask::Init()
{

  switch (fTrackRep) {
  case 0: std::cout << " -I- PndRecoKalmanTask:Init :: Using GeaneTrackRep" << std::endl; break;
  case 1: std::cout << " -I- PndRecoKalmanTask:Init :: Using RKTrackRep" << std::endl; break;
  default: Error("PndRecoKalmanTask::Init", "Not existing Track Representation!!"); return kERROR;
  }

  if (!fDaf) {
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
  } else {
    fDafFitter->SetGeane(fUseGeane);
    fDafFitter->SetPropagateToIP(fPropagateToIP);
    fDafFitter->SetPerpPlane(fPerpPlane);
    fDafFitter->SetMvdBranchName(fMvdBranchName);
    fDafFitter->SetCentralTrackerBranchName(fCentralTrackerBranchName);
    fDafFitter->SetVerbose(fVerbose);
    fDafFitter->SetTrackRep(fTrackRep);
    if (!fDafFitter->Init())
      return kFATAL;
  }

  // Get ROOT Manager
  FairRootManager *ioman = FairRootManager::Instance();

  if (ioman == 0) {
    Error("PndRecoKalmanTask::Init", "RootManager not instantiated!");
    return kERROR;
  }

  // Get input collection
  fTrackArray = (TClonesArray *)ioman->GetObject(fTrackInBranchName);
  if (fTrackArray == nullptr) {
    Error("PndRecoKalmanTask::Init", "track-array not found!");
    return kERROR;
  }
  if (fIdealHyp) {
    pdg = new TDatabasePDG();
    //    fTrackIDArray=(TClonesArray*) ioman->GetObject(fTrackInIDBranchName);
    //    if(fTrackIDArray==0)
    //    {
    //      Error("PndRecoKalmanTask::Init","track ID array not found! It is not possible to run ideal particle hypothesis");
    //      return kERROR;
    //    }

    fMCTrackArray = (TClonesArray *)ioman->GetObject("MCTrack");
    if (fMCTrackArray == nullptr) {
      Error("PndRecoKalmanTask::Init", "MCTrack array not found! It is not possible to run ideal particle hypothesis");
      return kERROR;
    }
  }
  // TODO this is deactivated for backwards compatibility. Should be removed someday [R.K. 12/2018]
  //  if (fIdealHyp) {
  //      ioman->Register(fTrackOutBranchName+"IdHyp","Gen", fFitTrackArray, GetPersistency());
  //  }
  //  else if (abs(fPDGHyp) == 11){
  //      ioman->Register(fTrackOutBranchName+"Electron","Gen", fFitTrackArray, GetPersistency());
  //  }
  //  else if (abs(fPDGHyp) == 13){
  //      ioman->Register(fTrackOutBranchName+"Muon","Gen", fFitTrackArray, GetPersistency());
  //  }
  //  else if (abs(fPDGHyp) == 211){
  //      ioman->Register(fTrackOutBranchName+"Pion","Gen", fFitTrackArray, GetPersistency());
  //  }
  //  else if (abs(fPDGHyp) == 321){
  //      ioman->Register(fTrackOutBranchName+"Kaon","Gen", fFitTrackArray, GetPersistency());
  //  }
  //  else if (abs(fPDGHyp) == 2212){
  //      ioman->Register(fTrackOutBranchName+"Proton","Gen", fFitTrackArray, GetPersistency());
  //  }
  //  else {
  //      std::cout << " -I- PndRecoKalmanTask:Init :: No hypothesis set, using PION hypothesis" << std::endl;
  ioman->Register(fTrackOutBranchName, "Gen", fFitTrackArray, GetPersistency());
  //  }

  return kSUCCESS;
}

void PndRecoKalmanTask::SetParContainers()
{
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  rtdb->getContainer("PndGeoSttPar");
  rtdb->getContainer("PndGeoFtsPar");
}

void PndRecoKalmanTask::Exec(Option_t *)
{
  if (fVerbose > 0)
    std::cout << "PndRecoKalmanTask::Exec" << std::endl;

  fFitTrackArray->Delete();

  Int_t ntracks = fTrackArray->GetEntriesFast();

  // Detailed output
  if (fVerbose > 1)
    std::cout << " -I- PndRecoKalmanTask: contains " << ntracks << " Tracks." << std::endl;

  // Cut too busy events TODO
  if (ntracks > fBusyCut) {
    std::cout << " -I- PndRecoKalmanTask::Exec: ntracks=" << ntracks << " Evil Event! skipping" << std::endl;
    return;
  }

  for (Int_t itr = 0; itr < ntracks; ++itr) {
    if (fVerbose > 1)
      std::cout << "starting track" << itr << std::endl;

    TClonesArray &trkRef = *fFitTrackArray;
    Int_t size = trkRef.GetEntriesFast();

    PndTrack *prefitTrack = (PndTrack *)fTrackArray->At(itr);
    Int_t fCharge = prefitTrack->GetParamFirst().GetQ();
    Int_t PDGCode = 0;
    if (fIdealHyp) {
      // PndTrackID *prefitTrackID = (PndTrackID*) fTrackIDArray->At(itr);
      if (prefitTrack->GetSortedMCTracks().size() > 0) {
        FairLink mcTrackId = prefitTrack->GetSortedMCTracks().at(0);
        if (mcTrackId.GetType() == FairRootManager::Instance()->GetBranchId("MCTrack")) {
          PndMCTrack *mcTrack = (PndMCTrack *)fMCTrackArray->At(mcTrackId.GetIndex()); // todo: replace with GetCloneOfLinkData
          if (!mcTrack) {
            PDGCode = 211 * fCharge;
            LOG(info) << " PndRecoKalmanTask::Exec: MCTrack #" << mcTrackId << " is not existing!! Trying with pion hyp";
          } else {
            PDGCode = mcTrack->GetPdgCode();
          }
          if (PDGCode >= 100000000) {
            PDGCode = 211 * fCharge;
            LOG(info) << " PndRecoKalmanTask::Exec: Track is an ion (PDGCode>100000000)! Trying with pion hyp";
          } else if ((((TParticlePDG *)pdg->GetParticle(PDGCode))->Charge()) == 0) {
            PDGCode = 211 * fCharge;
            LOG(error) << " PndRecoKalmanTask::Exec: Track MC charge is 0!!!! Trying with pion hyp";
          }
        } // end of MCTrack ID != -1
        else {
          PDGCode = 211 * fCharge;
          LOG(error) << " PndRecoKalmanTask::Exec: No MCTrack index in PndTrackID!! Trying with pion hyp";
        }
      } // end of "at least one correlated mc index"
      else {
        PDGCode = 211 * fCharge;
        LOG(error) << " PndRecoKalmanTask::Exec: No Correlated MCTrack id in PndTrackID!! Trying with pion hyp";
      }
    } // end of ideal hyp condition
    else {
      PDGCode = fPDGHyp * fCharge;
    }

    PndTrack *fitTrack;
    if (PDGCode != 0) {
      if (fDaf)
        fitTrack = fDafFitter->Fit(prefitTrack, PDGCode);
      else
        fitTrack = fFitter->Fit(prefitTrack, PDGCode);
    } else {
      fitTrack = prefitTrack;
      fitTrack->SetFlag(-22);
      LOG(info) << " PndRecoKalmanTask::Exec: Kalman cannot run on this track because of the bad MonteCarlo PDG code";
    }

    new (trkRef[size]) PndTrack(fitTrack->GetParamFirst(), fitTrack->GetParamLast(), fitTrack->GetTrackCand(), fitTrack->GetFlag(), fitTrack->GetChi2(), fitTrack->GetNDF(),
                                fitTrack->GetPidHypo(), itr,
                                FairRootManager::Instance()->GetBranchId(fTrackInBranchName)); // PndTrack* pndTrack =  //[R.K.03/2017] unused variable
    delete (fitTrack);
  }

  if (fVerbose > 0)
    std::cout << "Fitting done" << std::endl;

  return;
}

void PndRecoKalmanTask::SetParticleHypo(TString h)
{
  // Set the hypothesis for the fit, charge will be applied later
  if (h.BeginsWith("e") || h.BeginsWith("E")) {
    fPDGHyp = -11; // electrons
  } else if (h.BeginsWith("m") || h.BeginsWith("M")) {
    fPDGHyp = -13; // muons
  } else if (h.BeginsWith("pi") || h.BeginsWith("Pi") || h.BeginsWith("PI")) {
    fPDGHyp = 211; // pions
  } else if (h.BeginsWith("K") || h.BeginsWith("K")) {
    fPDGHyp = 321; // kaons
  } else if (h.BeginsWith("p") || h.BeginsWith("P") || h.BeginsWith("antip")) {
    fPDGHyp = 2212; // protons/antiprotons
  } else {
    LOG(info) << " PndRecoKalmanTask::SetParticleHypo: Not recognised PID set -> Using default PION hypothesis";
    fPDGHyp = 211; // Pion is default.
  }
}

void PndRecoKalmanTask::SetParticleHypo(Int_t h)
{
  switch (abs(h)) {
  case 11: fPDGHyp = -11; break;
  case 13: fPDGHyp = -13; break;
  case 211: fPDGHyp = 211; break;
  case 321: fPDGHyp = 321; break;
  case 2212: fPDGHyp = 2212; break;
  default:
    LOG(info) << " PndRecoKalmanTask::SetParticleHypo: Not recognised PID set -> Using default PION hypothesis";
    fPDGHyp = 211;
    break;
  }
}
ClassImp(PndRecoKalmanTask);
