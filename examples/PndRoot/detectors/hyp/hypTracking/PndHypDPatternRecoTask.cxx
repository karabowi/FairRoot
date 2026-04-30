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
//      Implementation of class PndHypDPatternRecoTask
//      see PndHypDPatternRecoTask.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndHypDPatternRecoTask.h"

// C/C++ Headers ----------------------
#include <algorithm>
#include <iostream>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "GFTrack.h"
#include "GFTrackCand.h"
#include "FairMCPoint.h"
#include "FairRunAna.h"
#include "FairLogger.h"
//#include "PndTofPoint.h"
#include "TGeoManager.h"
#include "PndHypPoint.h"
#include "PndHypHit.h"
#include "PndSdsMCPoint.h"
#include "PndSdsHit.h"
#include "PndMCTrack.h"
#include "LSLTrackRep.h"
#include "FairField.h"
#include "PndFieldAdaptor.h"
#include "GFFieldManager.h"
#include "GeaneTrackRep.h"
#include "GFKalman.h"
#include "GFException.h"
#include "FairGeanePro.h"
#include "FairTrackParP.h"
#include "TRandom.h"

// Class Member definitions -----------

PndHypDPatternRecoTask::PndHypDPatternRecoTask()
  : FairTask("Ideal Pattern Reco"), fPersistence(kFALSE), fUseGeane(kFALSE), fUseMVD(false), fVtx(false), fMCvalue(kFALSE), fVtxName(0), fEventNr(0)
{
}

PndHypDPatternRecoTask::~PndHypDPatternRecoTask() {}

InitStatus PndHypDPatternRecoTask::Init()
{
  // Get ROOT Manager
  FairRootManager *ioman = FairRootManager::Instance();

  if (ioman == 0) {
    Error("PndHypDPatternRecoTask::Init", "RootManager not instantiated!");
    return kERROR;
  }

  // open hit arrays
  std::map<unsigned int, TString>::iterator iter = fHitBranchNameMap.begin();
  while (iter != fHitBranchNameMap.end()) {

    TClonesArray *ar = (TClonesArray *)ioman->GetObject(iter->second);

    if (ar == 0) {
      Error("PndHypDPRTask::Init", "point-array %s not found!", iter->second.Data());
    } else {
      std::cout << " hit array " << iter->second << std::endl;
      fHitBranchMap[iter->first] = ar;
    }

    ++iter;
  } // end loops over hit types

  // open MCTruth array
  fMcArray = (TClonesArray *)ioman->GetObject("MCTrack");
  if (fMcArray == nullptr) {
    Error("PndHypDPRTask::Init", "mctrack-array not found!");
    return kERROR;
  }

  fPointArray = (TClonesArray *)ioman->GetObject("HypPoint");
  if (fPointArray == nullptr) {
    Error("PndHypDPRTask::Init", "hit-array not found!");
    return kERROR;
  }

  if (fUseMVD == true) {
    fSdsArray = (TClonesArray *)ioman->GetObject("MVDPoint");
    if (fSdsArray == nullptr) {
      Error("PndHypDPRTask::Init", "hit-array not found!");
      return kERROR;
    }
  }

  // create and register output array
  fTrackArray = new TClonesArray("GFTrack");
  ioman->Register("Track", "GenFit", fTrackArray, fPersistence);

  // GeanePro will get Geometry and BField from the Run
  fGeanePro = new FairGeanePro();

  LOG(info) << " gGeoManager = " << gGeoManager;

  return kSUCCESS;
}

void PndHypDPatternRecoTask::Exec(Option_t *)
{
  std::cout << "PndHypDPatternRecoTask::Exec" << std::endl;
  // Reset output Array
  if (fTrackArray == nullptr)
    Fatal("PndHypDPatternReco::Exec)", "No TrackArray");
  fTrackArray->Delete();

  // use McId to distinguish data from different tracks
  std::map<unsigned int, GFTrackCand *> candmap;
  std::cout << "<<<<< Event " << fEventNr++ << " <<<" << std::endl;
  FairMCPoint *point;
  FairHit *hit;

  std::map<unsigned int, TClonesArray *>::iterator iter = fHitBranchMap.begin();
  while (iter != fHitBranchMap.end()) {
    // TClonesArray* pointArray = iter->second;//_hitBranchMap[iter->first];
    fHitArray = iter->second;
    // loop over points
    Int_t np = fHitArray->GetEntriesFast();

    for (Int_t ip = 0; ip < np; ++ip) {
      // PndHypPoint* point=(PndHypPoint*)hitArray->At(ip);

      if (iter->first == 2) {
        hit = (PndHypHit *)fHitArray->At(ip);
        point = (PndHypPoint *)fPointArray->At(hit->GetRefIndex());
      }

      if (fUseMVD == true) {
        if (iter->first == 3) {
          hit = (PndSdsHit *)fHitArray->At(ip);
          point = (PndSdsMCPoint *)fSdsArray->At(hit->GetRefIndex());
        }
      }

      if (point == 0)
        continue;

      unsigned int id = point->GetTrackID();
      // cut on insane ids
      if (id > 10000)
        continue;
      // look for track in mc map
      if (candmap[id] == nullptr) {
        // create new track
        // std::cout<<"Creating new track candidate with id="<<id<<std::endl;
        candmap[id] = new GFTrackCand;
      }
      // add hit to track
      // set detectorid=2;
      candmap[id]->addHit(iter->first, ip);
    } // end loop over cluster
    ++iter;
  }

  // ------------------------------------------------------------------------
  // try to find some starting values
  // loop over tracks

  std::map<unsigned int, GFTrackCand *>::iterator candIter = candmap.begin();
  while (candIter != candmap.end()) {
    GFTrackCand *cand = candIter->second;
    if (cand->getNHits() < 3) {
      ++candIter;
      continue;
    }

    // Get MCTrack
    PndMCTrack *mc = (PndMCTrack *)fMcArray->At(candIter->first);
    if (mc == 0) {
      Error("PndHypDPRTask::Exec", "MCTrack Id=&i not found!", candIter->first);
      ++candIter;
      continue;
    }

    int pdg = mc->GetPdgCode();
    double q;
    int PDG;

    std::cout << " particle " << pdg << std::endl;

    if (pdg > 5000) {

      q = GetChargeIon(pdg);
      // std::cout<<" particle "<<q<<std::endl;
    } else {
      q = TDatabasePDG::Instance()->GetParticle(pdg)->Charge() / 3.;
      // std::cout<<" particle "<<q<<std::endl;
    }

    TVector3 vtx;
    TString VolName;

    if (fVtx) {
      if (mc->GetMotherID() != -1) {
        ++candIter;
        continue;
      }

      vtx = mc->GetStartVertex();
      std::cout << " pion prim " << mc->GetMotherID() << std::endl;
      std::cout << " pion vertex " << vtx.X() << " " << vtx.Y() << " " << vtx.Z() << std::endl;

      VolName = gGeoManager->FindNode(vtx.X(), vtx.Y(), vtx.Z())->GetName();

      std::cout << " vol name " << VolName.Data() << std::endl;

      if (!(VolName.Contains(fVtxName.Data()))) {
        ++candIter;
        continue;
      }

      std::cout << " vol name " << VolName.Data() << " " << fVtxName.Data() << std::endl;
    }

    TVector3 pos;
    double pre;
    TVector3 mom;

    if (fMCvalue) {
      unsigned int detId, hitId;
      // getting the first or last hit  in the track candidate
      cand->getHit(0, detId, hitId);
      PndHypHit *myHit;
      myHit = (PndHypHit *)fHitArray->At(hitId);
      FairMCPoint *pointF = (FairMCPoint *)fPointArray->At(myHit->GetRefIndex());

      if (pointF == 0) {
        Error("PndHypDPRTask::Exec", "MCTrack Id=&i not found!", candIter->first);
        ++candIter;
        continue;
      }
      std::cout << "Detector no. " << detId << std::endl;

      pointF->Position(pos);
      pointF->Momentum(mom);

    } else {

      pos = mc->GetStartVertex() + TVector3(0.001, 0.001, 0.001);
      pre = mc->GetMomentum().Mag() * 0.02;
      mom = mc->GetMomentum() + TVector3(gRandom->Gaus(0, pre), gRandom->Gaus(0, pre), gRandom->Gaus(0, pre));

      std::cout << "MC starting values " << mom.Mag() << std::endl;
    }

    if (mom.Mag() == 0) {
      Error("PndHypDPRTask::Exec", "Momentum is zero!", candIter->first);
      ++candIter;
      continue;
    }

    TVector3 poserr(0.01, 0.01, 0.005);
    TVector3 momerr = 0.3 * mom; //  10% error
    momerr += TVector3(0.1, 0.1, 0.1);

    TVector3 u(1., 0., 0.); //=mom.Orthogonal();
    // u.SetMag(1.);
    TVector3 v(0., 1., 0.); //=mom.Cross(u);
    // v.SetMag(1.);

    // create track-representation object and initialize with start values
    GFAbsTrackRep *rep = 0;
    // setting the magnetic field properly
    fField = FairRunAna::Instance()->GetField();
    PndFieldAdaptor *b = new PndFieldAdaptor(fField);
    GFFieldManager *fb;
    // fb->getInstance();
    // fb->init(b);

    if (fUseGeane) {
      GFDetPlane pl(pos, u, v);
      if (q > 0)
        PDG = 211;
      if (q < 0)
        PDG = -211;

      GeaneTrackRep *grep = new GeaneTrackRep(fGeanePro, pl, mom, poserr, momerr, q, PDG);

      // grep->setPropDir(1); // propagate in flight direction!
      rep = grep;
    } else { // use LSLTrackRep
      // calc momentum projections
      TVector3 dir = mom.Unit();
      double dxdz = dir.X() / dir.Z();
      double dydz = dir.Y() / dir.Z();
      double qp = q / mom.Mag();
      rep = new LSLTrackRep(pos.Z(), pos.X(), pos.Y(), dxdz, dydz, qp, poserr.X(), poserr.Y(), 0.1, 0.1, 0.1, b); // nullptr instead of b field

      // use RKTrackRep
      // rep=new RKtrackRep(pos,mom,poserr,momerr,pdg);
    }

    // create track object
    GFTrack *trk = new ((*fTrackArray)[fTrackArray->GetEntriesFast()]) GFTrack(rep);
    // std::cout<<" particle "<<cand->getNHits()<<std::endl;
    if (cand == nullptr) {
      Error("PndHypDPRTask::Exec", "Momentum is zero!", candIter->first);
      // std::cout<<_trackArray->GetEntriesFast()<<" tracks created"<<std::endl;
      ++candIter;
      continue;
    }
    if (trk != nullptr)
      trk->setCandidate(*cand); // here the candidate is copied!
    // std::cout<<" particle "<<(trk->getNumHits())<<std::endl;

    ++candIter;
  } // end loop over tracks

  std::cout << fTrackArray->GetEntriesFast() << " tracks created" << std::endl;

  // return;
}

Int_t PndHypDPatternRecoTask::GetChargeIon(Int_t ion)
{
  Int_t A, Z, L;

  if (ion > 1000000000 && (ion < 1010000000)) {
    ion -= 1000000000;
    Z = ion / 10000;
    ion -= 10000 * Z;
    A = ion / 10;
    cout << " ion charge " << Z << endl;

    return Z;
  }
  if ((ion > 1010000000 || ion > 1020000000)) {
    ion -= 1000000000;
    L = ion / 10000000;
    ion -= 10000000 * L;
    Z = ion / 10000;
    ion -= 10000 * Z;
    A = ion / 10;
    cout << L << " hypernuclei charge " << Z << endl;

    return Z;
  }
}

ClassImp(PndHypDPatternRecoTask)
