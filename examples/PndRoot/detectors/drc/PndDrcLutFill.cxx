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

// -----------------------------------------
// PndDrcLutFill.cpp
//
// Created on: 08.07.2013
// Author: R.Dzhygadlo at gsi.de
// -----------------------------------------

#include "PndDrcLutFill.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "PndMCTrack.h"
#include "PndDrcBarPoint.h"
#include "PndDrcPDPoint.h"
#include "PndDrcHit.h"
#include "PndDrcPDHit.h"
#include "PndDrcLutNode.h"

// -----   Default constructor   -------------------------------------------
PndDrcLutFill::PndDrcLutFill() : FairTask("PndDrcLutFill")
{
  fOutputFile = "luttab.root";
}

// -----   Standard constructors   -----------------------------------------
PndDrcLutFill::PndDrcLutFill(int verbose) : FairTask("PndDrcLutFill", verbose)
{
  fVerbose = verbose;
  fOutputFile = "luttab.root";
}

PndDrcLutFill::PndDrcLutFill(int verbose, TString outfilename) : FairTask("PndDrcLutFill", verbose)
{
  fVerbose = verbose;
  fOutputFile = outfilename;
}

// -----   Destructor   ----------------------------------------------------
PndDrcLutFill::~PndDrcLutFill() {}

// -----   Initialization   ------------------------------------------------
InitStatus PndDrcLutFill::Init()
{
  LOG(info) << "PndDrcLutFill: tast initialization started";
  nevents = 0;
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    LOG(error) << "PndDrcLutFill: RootManager not instantiated";
    return kFATAL;
  }

  // Get input array
  fMCArray = (TClonesArray *)ioman->GetObject("MCTrack");
  if (!fMCArray) {
    LOG(error) << "PndDrcLutFill: no MCTrack array";
    return kERROR;
  }

  // Get bar points array
  fBarPointArray = (TClonesArray *)ioman->GetObject("DrcBarPoint");
  if (!fBarPointArray) {
    LOG(error) << "PndDrcLutFill: no DrcBarPoint array";
    return kERROR;
  }

  // Get Photon point array
  fPDPointArray = (TClonesArray *)ioman->GetObject("DrcPDPoint");
  if (!fPDPointArray) {
    LOG(error) << "PndDrcLutFill: no DrcPDPoint array";
    return kERROR;
  }

  // Get ev points array
  fEVPointArray = (TClonesArray *)ioman->GetObject("DrcEVPoint");
  if (!fEVPointArray) {
    LOG(error) << "PndDrcLutFill: no DrcEVPoint array";
    return kERROR;
  }

  // Get digi array
  fDigiArray = (TClonesArray *)ioman->GetObject("DrcDigi");
  if (!fDigiArray) {
    LOG(error) << "PndDrcLutFill: no DrcDigi array";
    return kERROR;
  }
  // Get input array
  fPDHitArray = (TClonesArray *)ioman->GetObject("DrcPDHit");
  if (!fPDHitArray) {
    LOG(error) << "PndDrcLutFill: no DrcPDHit array";
    return kERROR;
  }

  fFile = TFile::Open(fOutputFile, "RECREATE");
  fTree = new TTree("dircsim", "Look-up table for DIRC");
  for (int l = 0; l < 5; l++) {
    fLut[l] = new TClonesArray("PndDrcLutNode");
    fTree->Branch(Form("LUT%d", l), &fLut[l], 256000, 0);
  }

  InitLut();

  fGeo = new PndGeoDrc();
  fBboxNum = fGeo->BBoxNum();
  fPipehAngle = fGeo->PipehAngle();
  fDphi = 2. * (180. - 2 * fPipehAngle) / (double)fGeo->BBoxNum();

  LOG(info) << "PndDrcLutFill: intialization successfull";
  return kSUCCESS;
}

void PndDrcLutFill::InitLut()
{
  int Nnodes = 2000;
  for (int l = 0; l < 5; l++) {
    TClonesArray &fLuta = *fLut[l];
    for (Long64_t n = 0; n < Nnodes; n++) {
      new ((fLuta)[n]) PndDrcLutNode(-1);
    }
  }
}

// -----   Execution of Task   ---------------------------------------------
void PndDrcLutFill::Exec(Option_t *)
{
  nevents++;
  LOG(info) << "PndDrcLutFill: Event #" << nevents;
  fDetectorID = 0;
  ProcessPhotonHit();
}

//--------------Process Photon Hits-----------------------------------------
void PndDrcLutFill::ProcessPhotonHit()
{
  // LUT generated for:
  double lutboxPhi = 10.825;
  TVector3 dir, dirm, vec, posInBar;

  // Loop over PndDrcPDHits
  for (int k = 0; k < fPDHitArray->GetEntriesFast(); k++) {
    fPDHit = static_cast<PndDrcPDHit *>(fPDHitArray->At(k));
    int pointID = fPDHit->GetLink(1).GetIndex();
    int ch = fPDHit->GetSensorId();
    if (pointID == -1)
      continue;

    fPDPoint = static_cast<PndDrcPDPoint *>(fPDPointArray->At(pointID));
    fBarPoint = static_cast<PndDrcBarPoint *>(fBarPointArray->At(fPDPoint->GetBarPointID()));
    int barId = fBarPoint->GetBarId();

    int trackID = fPDPoint->GetTrackID();
    double time = fPDPoint->GetTime();
    int nev = 0;
    double path = 0;

    for (int i = 0; i < fEVPointArray->GetEntriesFast(); i++) {
      fEVPoint = static_cast<PndDrcEVPoint *>(fEVPointArray->At(i));
      if (trackID == fEVPoint->GetTrackID()) {
        vec = fEVPoint->GetNormal();

        if (vec.X() > 0.99)
          path = path * 10 + 1; // right
        if (vec.X() < -0.99)
          path = path * 10 + 2; // left
        if (vec.Y() < -0.99)
          path = path * 10 + 3; // bottom
        if (fabs(vec.Y() - 0.844437) < 0.1)
          path = path * 10 + 4; // top
        if (vec.Y() > 0.99)
          path = path * 10 + 5; // top of the lens
        if (vec.Z() < -0.99)
          path = path * 10 + 9; // face
        if (++nev > 10)
          LOG(warning) << "PndDrcLutFill: too many reflections in Prism";
      }
    }

    fMCTrack = static_cast<PndMCTrack *>(fMCArray->At(trackID));
    dir = fMCTrack->GetMomentum().Unit();
    dir.RotateZ(-lutboxPhi / 180. * TMath::Pi());

    // if (path == 919) {
    //   dir.Print();
    //   fMCTrack->GetStartVertex().Print();
    //   for (int i = 0; i < fEVPointArray->GetEntriesFast(); i++) {
    //     fEVPoint = static_cast<PndDrcEVPoint *>(fEVPointArray->At(i));
    //     if (trackID == fEVPoint->GetTrackID()) {
    //       vec = fEVPoint->GetNormal();
    //       TVector3 p;
    //       fEVPoint->Position(p);
    //       vec.Print();
    //       p.Print();
    //     }
    //   }
    // }

    if (ch > 2000 || ch < 0) {
      LOG(warning) << "PndDrcLutFill: ch is outside of range";
      continue;
    }

    // //======================
    // posInBar = fMCTrack->GetStartVertex();
    // double phi = posInBar.Phi()/TMath::Pi()*180;
    // if(phi < 0) phi = 360 + phi;
    // if(phi >= 0 && phi < 90) barPhi = TMath::Floor(phi/fDphi) *fDphi + fDphi/2.;
    // if(phi >= 90 && phi < 270) barPhi = 90  + fPipehAngle + TMath::Floor((phi-90-fPipehAngle)/fDphi) *fDphi + fDphi/2.;
    // if(phi >= 270 && phi < 360) barPhi = 270 + fPipehAngle + TMath::Floor((phi-270-fPipehAngle)/fDphi) *fDphi + fDphi/2.;

    // dirm.RotateZ(-barPhi/180.*TMath::Pi());
    // TVector3 fnX1 = TVector3 (1,0,0);
    // TVector3 fnY1 = TVector3( 0,1,0);
    // double criticalAngle = asin(1.00028/fGeo->nQuartz());
    // for(int u=0; u<8; u++){
    //   if(u == 0) dir = dirm;
    //   if(u == 1) dir.SetXYZ( dirm.X(), dirm.Y(),-dirm.Z());
    //   if(u == 2) dir.SetXYZ( dirm.X(),-dirm.Y(), dirm.Z());
    //   if(u == 3) dir.SetXYZ(-dirm.X(), dirm.Y(), dirm.Z());
    //   if(u == 4) dir.SetXYZ(-dirm.X(),-dirm.Y(), dirm.Z());
    //   if(u == 5) dir.SetXYZ(-dirm.X(), dirm.Y(),-dirm.Z());
    //   if(u == 6) dir.SetXYZ( dirm.X(),-dirm.Y(),-dirm.Z());
    //   if(u == 7) dir = -dirm;

    //   if(dir.Angle(fnX1) < criticalAngle || dir.Angle(fnY1) < criticalAngle){
    // 	continue;
    //   }
    //   (static_cast<PndDrcLutNode*>((fLut->At(ch))))->AddEntry(dir);
    //   (static_cast<PndDrcLutNode*>((fLut->At(ch))))->AddPathId(pathid);
    // }
    // //======================

    (static_cast<PndDrcLutNode *>((fLut[barId]->At(ch))))->AddEntry(ch, dir, path, 0, time, fPDPoint->GetMomAtEV()); // fPDHit->GetPosition()
  }
}

// -----   Finish Task   ---------------------------------------------------
void PndDrcLutFill::Finish()
{
  fTree->Fill();
  fTree->Write();
  fFile->Write();

  for (int l = 0; l < 5; l++)
    fLut[l]->Clear();

  LOG(info) << "PndDrcLutFill: task finished";
}

ClassImp(PndDrcLutFill)
