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
// -----                PndMvdIdealTrackingTask source file             -----
// -----                  Created 20/03/07  by R.Kliemt               -----
// -------------------------------------------------------------------------
// libc includes
#include <iostream>

// Root includes
#include "TROOT.h"
#include "TString.h"
#include "TClonesArray.h"
#include "TParticlePDG.h"

// framework includes
#include "FairRootManager.h"
#include "PndHypIdealTrackingTask.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "PndMCTrack.h"

// PndHyp includes
#include "PndHypRecoHit.h"
// #include "PndHypGFTrackCand.h"
#include "GFTrack.h"
#include "GFTrackCand.h"
#include "PndHypHit.h"
#include "PndHypPoint.h"
#include "PndHypCluster.h"
#include "PndHypDigi.h"

// -----   Default constructor   -------------------------------------------
PndHypIdealTrackingTask::PndHypIdealTrackingTask() : FairTask("Digitization task for PANDA PndHyp")
{
  // fBranchName 	= "HYPStripClusterHit";
  fBranchName = "";
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndHypIdealTrackingTask::~PndHypIdealTrackingTask() {}

// -----   Public method Init   --------------------------------------------
InitStatus PndHypIdealTrackingTask::Init()
{
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    std::cout << "-E- PndHypIdealTrackingTask::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  // Get input array
  fHitArray = (TClonesArray *)ioman->GetObject("HYPHit");
  if (!fHitArray) {
    std::cout << "-W- PndHypIdealTrackingTask::Init: "
              << "No fHitarray!" << std::endl;
    return kERROR;
  }

  fClusterStripArray = (TClonesArray *)ioman->GetObject("HYPStripClusterCand");
  if (!fClusterStripArray) {
    std::cout << "-W- PndMvdIdealTrackingTask::Init: "
              << "No fClusterStripArray!" << std::endl;
    return kERROR;
  }

  fDigiStripArray = (TClonesArray *)ioman->GetObject("HypStripDigis");
  if (!fDigiStripArray) {
    std::cout << "-W- PndMvdIdealTrackingTask::Init: "
              << "No fDigiStripArray array!" << std::endl;
    return kERROR;
  }

  fPointArray = (TClonesArray *)ioman->GetObject("HypPoint");
  if (!fPointArray) {
    std::cout << "-W- PndHypStripHitProducer::Init: "
              << "No MVDPoint array!" << std::endl;
    return kERROR;
  }

  // Get MCTruth collection
  fMctruthArray = (TClonesArray *)ioman->GetObject("MCTrack");
  if (fMctruthArray == nullptr) {
    std::cout << "-W- PndHypIdealTrackingTask::Init: "
              << "No McTruth array!" << std::endl;
    return kERROR;
  }

  // Create and register output array
  fTrackOutputArray = new TClonesArray("GFTrackCand");
  ioman->Register("HypTrackCand", "PndHyp ideal tracklets", fTrackOutputArray, kTRUE);

  return kSUCCESS;
}
// -------------------------------------------------------------------------
void PndHypIdealTrackingTask::SetParContainers()
{
  // Get Base Container
  FairRunAna *ana = FairRunAna::Instance();
  FairRuntimeDb *rtdb = ana->GetRuntimeDb();
}

// -----   Public method Exec   --------------------------------------------
void PndHypIdealTrackingTask::Exec(Option_t *)
{
  if (!fTrackOutputArray)
    Fatal("Exec", "No fTrackOutputArray");
  fTrackOutputArray->Delete();

  // CREATE MONTECARLO MAP
  std::map<Int_t, PndMCTrack *> mcmap;
  Int_t nmc = fMctruthArray->GetEntriesFast();
  for (Int_t imc = 0; imc < nmc; ++imc) {
    PndMCTrack *mc = (PndMCTrack *)fMctruthArray->At(imc);
    mcmap[imc] = mc;
    // if(mc->GetPdgCode()>100)cout<<imc<<" "<<mc->GetPdgCode()<<endl;
  }

  // BUILD TRACK Candidates
  std::map<Int_t, GFTrackCand *> trackMap;

  Int_t nPndHypHits = fHitArray->GetEntriesFast();
  cout << nPndHypHits << endl;

  for (Int_t iHit = 0; iHit < nPndHypHits; ++iHit) {
    PndHypHit *hit = (PndHypHit *)fHitArray->At(iHit);
    Int_t clustid = hit->GetRefIndex();
    Int_t digiid, pointid, MCid;

    if (hit->GetBotIndex() > 0) {
      digiid = ((PndHypCluster *)fClusterStripArray->At(clustid))->GetDigiIndex(0);
      pointid = ((PndHypDigi *)fDigiStripArray->At(digiid))->GetIndex();
    }

    // cout<<iHit<<" "<<pointid<<" "<<fPointArray->GetEntriesFast()<<endl;

    // if(pointid >fPointArray->GetEntriesFast())continue;

    PndHypPoint *point = (PndHypPoint *)fPointArray->At(pointid);
    MCid = point->GetTrackID();

    // if(point)cout<<point->GetVolumeID()<<endl;

    // cut on secondaries (deltas) etc
    if (MCid < 0)
      continue;

    cout << MCid << endl;
    TVector3 pos;
    hit->Position(pos);

    // build tracks here:

    // check if track already candidated:
    if (trackMap[MCid] == 0) {
      Int_t size = fTrackOutputArray->GetEntriesFast();
      // caution this should be an intrinsic assignment, so trackMap and
      // fTrackOutputArray contain the same pointers.
      trackMap[MCid] = new ((*fTrackOutputArray)[size]) GFTrackCand();

      Int_t pdgcode;
      if (mcmap[MCid] == 0)
        continue;

      pdgcode = mcmap[MCid]->GetPdgCode();
      cout << pdgcode << endl;

      TParticlePDG pdgpart(pdgcode);
      Int_t mcCharge = (Int_t)pdgpart.Charge();

      TVector3 vtx = mcmap[MCid]->GetStartVertex();
      TVector3 mom = mcmap[MCid]->GetMomentum();

      Double_t invp = 1 / mom.Mag();
      Double_t dxdz = invp * mom.x() * mom.z();
      Double_t dydz = invp * mom.y() * mom.z();
      Double_t sigx = 0.01, sigy = 0.01, sigdxdz = 0.01, sigdydz = 0.01, siginvp = 0.01;

      sigx *= vtx.x();
      sigy *= vtx.y();
      sigdxdz *= dxdz;
      sigdydz *= dydz;
      siginvp *= invp;

      if (fVerbose > 1)
        std::cout << "PndHypIdealTrackingTask::Exec(), Particle Info: " << pdgpart.GetName() << " (" << pdgcode << ") Q=" << mcCharge << std::endl;

      trackMap[MCid]->setCurv(GetTrackCurvature(mcmap[MCid]));
      trackMap[MCid]->setDip(GetTrackDip(mcmap[MCid]));
      trackMap[MCid]->setInverted(false);
    }

    // add the hit index
    //       trackMap[MCid]->addHit(,hit->GetDetectorID(),iHit);
    //       PndHypRecoHit arecohit(hit);

    /// detector number hardcoded TODO: convention
    trackMap[MCid]->addHit(3, iHit);
    cout << iHit << endl;

  } // end for hit

  std::cout << fTrackOutputArray->GetEntriesFast() << " Tracks created" << std::endl;
}

Double_t PndHypIdealTrackingTask::GetTrackCurvature(PndMCTrack *myTrack)
{
  TVector3 p = myTrack->GetMomentum();
  return (2 / TMath::Sqrt(p.Px() * p.Px() + p.Py() * p.Py()));
}

Double_t PndHypIdealTrackingTask::GetTrackDip(PndMCTrack *myTrack)
{
  TVector3 p = myTrack->GetMomentum();
  return (p.Mag() / TMath::Sqrt(p.Px() * p.Px() + p.Py() * p.Py()));
}

// -------------------------------------------------------------------------

ClassImp(PndHypIdealTrackingTask);
