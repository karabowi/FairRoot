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
//      modified by Elisabetta Prencipe 19/05/2014
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndRecoKalmanTask2.h"

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

PndRecoKalmanTask2::PndRecoKalmanTask2(const char *name, Int_t iVerbose)
  : PndPersistencyTask(name, iVerbose), fFitTrackArray(), fTrackInBranchName(""), fTrackOutBranchName(""), fMvdBranchName(""), fCentralTrackerBranchName(""), fFitter(),
    fDafFitter(), fUseGeane(kTRUE), fIdealHyp(kFALSE), fDaf(kFALSE), fPropagateToIP(kFALSE), fPropagateDistance(2.f), fPerpPlane(kFALSE), fNumIt(1), fPDGHyp(-13), fBusyCut(20)
{
  fFitTrackArray = new TClonesArray("PndTrack");
  fFitter = new PndRecoKalmanFit2();
  fDafFitter = new PndRecoDafFit2();
  SetPersistency(kTRUE);
}

PndRecoKalmanTask2::~PndRecoKalmanTask2() {}

InitStatus PndRecoKalmanTask2::Init()
{

  std::cout << " -I- PndRecoKalmanTask2:Init :: Using RKTrackRep" << std::endl;

  if (!fDaf) {
    fFitter->SetGeane(fUseGeane);
    fFitter->SetPropagateToIP(fPropagateToIP);
    fFitter->SetPropagateDistance(fPropagateDistance);
    fFitter->SetPerpPlane(fPerpPlane);
    fFitter->SetNumIterations(fNumIt);
    fFitter->SetMvdBranchName(fMvdBranchName);
    fFitter->SetCentralTrackerBranchName(fCentralTrackerBranchName);
    fFitter->SetVerbose(fVerbose);
    if (!fFitter->Init())
      return kFATAL;
  } else {
    fDafFitter->SetGeane(fUseGeane);
    fDafFitter->SetPropagateToIP(fPropagateToIP);
    fDafFitter->SetPropagateDistance(fPropagateDistance);
    fDafFitter->SetPerpPlane(fPerpPlane);
    fDafFitter->SetMvdBranchName(fMvdBranchName);
    fDafFitter->SetCentralTrackerBranchName(fCentralTrackerBranchName);
    fDafFitter->SetVerbose(fVerbose);
    if (!fDafFitter->Init())
      return kFATAL;
  }

  // Get ROOT Manager
  FairRootManager *ioman = FairRootManager::Instance();

  if (ioman == 0) {
    Error("PndRecoKalmanTask2::Init", "RootManager not instantiated!");
    return kERROR;
  }

  // Get input collection
  fTrackArray = (TClonesArray *)ioman->GetObject(fTrackInBranchName);
  if (fTrackArray == nullptr) {
    Error("PndRecoKalmanTask2::Init", "track-array not found!");
    return kERROR;
  }
  if (fIdealHyp) {
    pdg = new TDatabasePDG();
    //    fTrackIDArray=(TClonesArray*) ioman->GetObject(fTrackInIDBranchName);
    //    if(fTrackIDArray==0)
    //    {
    //      Error("PndRecoKalmanTask2::Init","track ID array not found! It is not possible to run ideal particle hypothesis");
    //      return kERROR;
    //    }

    fMCTrackArray = (TClonesArray *)ioman->GetObject("MCTrack");
    if (fMCTrackArray == nullptr) {
      Error("PndRecoKalmanTask2::Init", "MCTrack array not found! It is not possible to run ideal particle hypothesis");
      return kERROR;
    }
  }

  ioman->Register(fTrackOutBranchName, "Gen", fFitTrackArray, GetPersistency());

  return kSUCCESS;
}

void PndRecoKalmanTask2::SetParContainers()
{
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  rtdb->getContainer("PndGeoSttPar");
  rtdb->getContainer("PndGeoFtsPar");
}

void PndRecoKalmanTask2::Exec(Option_t *)
{
  if (fVerbose > 0)
    std::cout << "PndRecoKalmanTask2::Exec " << FairRootManager::Instance()->GetEntryNr() << std::endl;

  fFitTrackArray->Delete();

  Int_t ntracks = fTrackArray->GetEntriesFast();

  // Detailed output
  if (fVerbose > 1)
    std::cout << " -I- PndRecoKalmanTask2: contains " << ntracks << " Tracks." << std::endl;

  // Cut too busy events TODO
  if (ntracks > fBusyCut) {
    std::cout << " -I- PndRecoKalmanTask2::Exec: ntracks=" << ntracks << " Evil Event! skipping" << std::endl;
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
      std::vector<FairLink> mcTrackLinks = prefitTrack->GetSortedMCTracks();
      //      PndTrackID *prefitTrackID = (PndTrackID*) fTrackIDArray->At(itr);
      if (mcTrackLinks.size() > 0) {
        Int_t mcTrackId = mcTrackLinks[0].GetIndex();
        if (mcTrackId != -1) {
          PndMCTrack *mcTrack = (PndMCTrack *)fMCTrackArray->At( // TODO: Replace with GetCloneOfLinkData to run time-based
            mcTrackId);
          if (!mcTrack) {
            PDGCode = 211 * fCharge;
            LOG(info) << " PndRecoKalmanTask2::Exec: MCTrack #" << mcTrackId << " is not existing!! Trying with pion hyp";
          } else {
            PDGCode = mcTrack->GetPdgCode();
          }
          if (PDGCode >= 100000000) {
            PDGCode = 211 * fCharge;
            LOG(info) << " PndRecoKalmanTask2::Exec: Track is an ion (PDGCode>100000000)! Trying with pion hyp";
          } else if ((((TParticlePDG *)pdg->GetParticle(PDGCode))->Charge()) == 0) {
            PDGCode = 211 * fCharge;
            LOG(error) << " PndRecoKalmanTask2::Exec: Track MC charge is 0!!!! Trying with pion hyp";
          }
        } // end of MCTrack ID != -1
        else {
          PDGCode = 211 * fCharge;
          LOG(error) << " PndRecoKalmanTask2::Exec: No MCTrack index in PndTrackID!! Trying with pion hyp";
        }
      } // end of "at least one correlated mc index"
      else {
        PDGCode = 211 * fCharge;
        LOG(error) << " PndRecoKalmanTask2::Exec: No Correlated MCTrack id in PndTrackID!! Trying with pion hyp";
      }
    } // end of ideal hyp condition
    else {
      PDGCode = fPDGHyp * fCharge;
    }

    PndTrack fitTrack;
    PndTrack *fitTrackPointer = 0;
    // bool usePrefit = false; //[R.K.03/2017] unused variable
    if (PDGCode != 0) {
      if (fDaf) {
        fitTrackPointer = (fDafFitter->Fit(prefitTrack, PDGCode));
        fitTrack = *fitTrackPointer;
      } else {
        fitTrackPointer = (fFitter->Fit(prefitTrack, PDGCode));
        fitTrack = *fitTrackPointer;
      }
    } else {
      fitTrack = *prefitTrack;
      fitTrack.SetFlag(-22);
      // usePrefit = true; //[R.K.03/2017] unused variable
      LOG(info) << " PndRecoKalmanTask2::Exec: Kalman cannot run on this track because of the bad MonteCarlo PDG code";
    }

    new (trkRef[size])
      PndTrack(fitTrack.GetParamFirst(), fitTrack.GetParamLast(), fitTrack.GetTrackCand(), fitTrack.GetFlag(), fitTrack.GetChi2(), fitTrack.GetNDF(), fitTrack.GetPidHypo(), itr,
               FairRootManager::Instance()->GetBranchId(fTrackInBranchName)); // PndTrack* pndTrack =  //[R.K.03/2017] unused variable
  }

  if (fVerbose > 0)
    std::cout << "Fitting done" << std::endl;

  return;
}

void PndRecoKalmanTask2::SetParticleHypo(TString h)
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
    LOG(info) << " PndRecoKalmanTask2::SetParticleHypo: Not recognised PID set -> Using default MUON hypothesis";
    fPDGHyp = -13; // Muon is default.
  }
}

void PndRecoKalmanTask2::SetParticleHypo(Int_t h)
{
  switch (abs(h)) {
  case 11: fPDGHyp = -11; break;
  case 13: fPDGHyp = -13; break;
  case 211: fPDGHyp = 211; break;
  case 321: fPDGHyp = 321; break;
  case 2212: fPDGHyp = 2212; break;
  default:
    LOG(info) << " PndRecoKalmanTask2::SetParticleHypo: Not recognised PID set -> Using default MUON hypothesis";
    fPDGHyp = -13;
    break;
  }
}
ClassImp(PndRecoKalmanTask2);
