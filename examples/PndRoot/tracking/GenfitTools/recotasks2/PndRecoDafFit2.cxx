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
//      Implementation of class PndRecoDafFit
//      see PndRecoDafFit.h for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Stefano Spataro, UNI Torino
//      modified by Elisabetta Prencipe 19/05/2014
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndRecoDafFit2.h"

// C/C++ Headers ----------------------
#include <algorithm>
#include <iostream>
#include <assert.h>
#include <cmath>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "FairRunAna.h"
#include "TClonesArray.h"

#include "Track.h"
//#include "TDatabasePDG.h"


#include "PndSdsRecoHit2.h"
#include "PndGemRecoHit2.h"
#include "PndSttRecoHit2.h"
#include "PndFtsRecoHit2.h"
#include "PndMdtRecoHit2.h"

#include "PndSttRecoHitProducer2.h"
#include "PndFtsRecoHitProducer2.h"

#include "PndGeoSttPar.h"
#include "PndGeoFtsPar.h"

//#include "PndSttMapCreator.h"
#include "PndStt2GeoHandler.h"
#include "PndFtsMapCreator.h"

#include "PndGenfitAdapters2.h"
#include "PndTrack.h"
#include "PndTrackCand.h"
#include "PndDetectorList.h"
#include "PndGeoHandling.h"
#include "MeasurementFactory.h"
#include "DAF.h"
#include "Exception.h"
#include "TLorentzVector.h"

#include "FairTrackParH.h"

#include <MaterialEffects.h>
#include <TGeoMaterialInterface.h>
#include "TrackCand.h"
#include "RKTrackRep.h"
#include "FieldManager.h"
#include "PndGenfitField2.h"
#include "FairGeanePro.h"

// Class Member definitions -----------

PndRecoDafFit2::PndRecoDafFit2()
  : TNamed("Genfit", "Fit Tracks"), fMvdBranchName(""), fCentralTrackerBranchName(""), fUseGeane(kTRUE), fPropagateToIP(kTRUE), fPerpPlane(kFALSE), fNumIt(1), fVerbose(0)
{
  PndGeoHandling::Instance();
}

Bool_t PndRecoDafFit2::Init()
{
  // Get ROOT Manager
  FairRootManager *ioman = FairRootManager::Instance();
  if (ioman == 0) {
    Error("PndRecoDafFit2::Init", "RootManager not instantiated!");
    return kFALSE;
  }

  // STT map loading
  //TClonesArray *tubeArray = nullptr;
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  PndGeoSttPar *sttParameters = (PndGeoSttPar *)rtdb->getContainer("PndGeoSttPar");
  int geoType = sttParameters->GetGeometryType();
  
  // common for old and new geometry
  TClonesArray *tubeArray = PndStt2GeoHandler::Instance(sttParameters)->GetTubeArray();
  //PndStt2GeoHandler geoH(sttParameters);
  //tubeArray = geoH.GetTubeArray();
  
  //// old STT
  //if (geoType == 1) {
    //PndSttMapCreator *mapper = new PndSttMapCreator(sttParameters);
    //tubeArray = mapper->FillTubeArray();
  //}
  //// new STT
  //else if (geoType == 3) {
    //PndStt2GeoHandler geoH(sttParameters);
    //tubeArray = geoH.GetTubeArray();
  //}
  
  // Build hit factory -----------------------------
  fTheRecoHitFactory = new genfit::MeasurementFactory<genfit::AbsMeasurement>();
  if (fVerbose < 2)
    genfit::Exception::quiet(true);

  TClonesArray *stripar;
  TClonesArray *pixelar;
  if (fMvdBranchName == "Mix") {
    stripar = (TClonesArray *)ioman->GetObject("MVDHitsStripMix");
    if (stripar != 0) {
      fTheRecoHitFactory->addProducer(FairRootManager::Instance()->GetBranchId("MVDHitsStripMix"), new genfit::MeasurementProducer<PndSdsHit, PndSdsRecoHit2>(stripar));
      std::cout << "*** PndRecoDafFit2::Init"
                << "\t"
                << "MVDHitsStripMix array  found" << std::endl;
    }

    pixelar = (TClonesArray *)ioman->GetObject("MVDHitsPixelMix");
    if (pixelar != 0) {
      fTheRecoHitFactory->addProducer(FairRootManager::Instance()->GetBranchId("MVDHitsPixelMix"), new genfit::MeasurementProducer<PndSdsHit, PndSdsRecoHit2>(pixelar));
      std::cout << "*** PndRecoDafFit2::Init"
                << "\t"
                << "MVDHitsPixelMix array  found" << std::endl;
    }
  } else {
    stripar = (TClonesArray *)ioman->GetObject("MVDHitsStrip");
    if (stripar != 0) {
      fTheRecoHitFactory->addProducer(FairRootManager::Instance()->GetBranchId("MVDHitsStrip"), new genfit::MeasurementProducer<PndSdsHit, PndSdsRecoHit2>(stripar));
      std::cout << "*** PndRecoDafFit2::Init"
                << "\t"
                << "MVDHitsStrip array  found" << std::endl;
    }

    pixelar = (TClonesArray *)ioman->GetObject("MVDHitsPixel");
    if (pixelar != 0) {
      fTheRecoHitFactory->addProducer(FairRootManager::Instance()->GetBranchId("MVDHitsPixel"), new genfit::MeasurementProducer<PndSdsHit, PndSdsRecoHit2>(pixelar));
      std::cout << "*** PndRecoDafFit2::Init"
                << "\t"
                << "MVDHitsPixel array  found" << std::endl;
    }
  }

  TClonesArray *sthit; // TClonesArray *sttr;   //[R.K. 01/2017] unused variable?

  if (fCentralTrackerBranchName == "Mix") {
    sthit = (TClonesArray *)ioman->GetObject("STTHitMix");
    if (sthit != 0) {
      fTheRecoHitFactory->addProducer(FairRootManager::Instance()->GetBranchId("STTHitMix"), new PndSttRecoHitProducer2<PndSttHit, PndSttRecoHit2>(sthit, tubeArray));
      std::cout << "*** PndRecoDafFit2::Init"
                << "\t"
                << "STTHitMix array  found" << std::endl;
    }
  } else {
    sthit = (TClonesArray *)ioman->GetObject("STTHit");
    if (sthit != 0) {
      fTheRecoHitFactory->addProducer(FairRootManager::Instance()->GetBranchId("STTHit"), new PndSttRecoHitProducer2<PndSttHit, PndSttRecoHit2>(sthit, tubeArray));
      std::cout << "*** PndRecoDafFit2::Init"
                << "\t"
                << "SttHit array  found" << std::endl;
    }
  }

  TClonesArray *gemar = (TClonesArray *)ioman->GetObject("GEMHit");
  if (gemar != 0) {
    fTheRecoHitFactory->addProducer(FairRootManager::Instance()->GetBranchId("GEMHit"), new genfit::MeasurementProducer<PndGemHit, PndGemRecoHit2>(gemar));
    std::cout << "*** PndRecoDafFit2::Init"
              << "\t"
              << "GEMHit array  found" << std::endl;
  }

  TClonesArray *mdtar = (TClonesArray *)ioman->GetObject("MdtHit");
  if (mdtar != 0) {
    fTheRecoHitFactory->addProducer(FairRootManager::Instance()->GetBranchId("MdtHit"), new genfit::MeasurementProducer<PndMdtHit, PndMdtRecoHit2>(mdtar));
    std::cout << "*** PndRecoDafFit2::Init"
              << "\t"
              << "MdtHit array  found" << std::endl;
  }

  if (fUseGeane) {
    fPro = new FairGeanePro();
  } else {
    Error("PndRecoDafFit2::Init", "Only GEANE Propagation available!!!");
    return kFALSE;
  }

  fGenFitter.setMaxIterations(fNumIt);
  genfit::FieldManager::getInstance()->init(new PndGenfitField2());
  genfit::MaterialEffects::getInstance()->init(new genfit::TGeoMaterialInterface());

  std::cout << "===PndRecoDafFit2::Init() finished ===================================================" << std::endl;

  return kTRUE;
}

PndRecoDafFit2::~PndRecoDafFit2() {}

PndTrack *PndRecoDafFit2::Fit(PndTrack *tBefore, Int_t PDG)
{
  PndTrack *tAfter = nullptr;
  if (fVerbose > 0)
    std::cout << "PndRecoDafFit2::Fit" << std::endl;
  if (fabs(tBefore->GetParamFirst().GetPz()) < 1e-9) {
    tAfter = tBefore;
    tAfter->SetFlag(-10);
    return tAfter; // flag -10 : pz==0
  }

  // Int_t  fCharge= tBefore->GetParamFirst().GetQ(); //[R.K. 01/2017] unused variable?
  Int_t PDGCode = PDG;
  TVector3 StartPos(tBefore->GetParamFirst().GetX(), tBefore->GetParamFirst().GetY(), tBefore->GetParamFirst().GetZ());
  TVector3 StartMom(tBefore->GetParamFirst().GetPx(), tBefore->GetParamFirst().GetPy(), tBefore->GetParamFirst().GetPz());

  TMatrixDSym covSeed(6);
  covSeed(0, 0) = tBefore->GetParamFirst().GetDX() * tBefore->GetParamFirst().GetDX();
  covSeed(1, 1) = tBefore->GetParamFirst().GetDY() * tBefore->GetParamFirst().GetDY();
  covSeed(2, 2) = tBefore->GetParamFirst().GetDZ() * tBefore->GetParamFirst().GetDZ();

  covSeed(3, 3) = tBefore->GetParamFirst().GetDPx() * tBefore->GetParamFirst().GetDPx();
  covSeed(4, 4) = tBefore->GetParamFirst().GetDPy() * tBefore->GetParamFirst().GetDPy();
  covSeed(5, 5) = tBefore->GetParamFirst().GetDPz() * tBefore->GetParamFirst().GetDPz();

  if (fPropagateToIP) {
    // Calculating params at PCA to Origin
    FairTrackParP par = tBefore->GetParamFirst();
    Int_t ierr = 0;
    FairTrackParH *helix = new FairTrackParH(&par, ierr);
    FairGeanePro *fPro0 = new FairGeanePro();
    FairTrackParH *fRes = new FairTrackParH();
    fPro0->SetPoint(TVector3(0, 0, 0));
    fPro0->PropagateToPCA(1, -1);
    Bool_t rc = fPro0->Propagate(helix, fRes, PDGCode);
    if (rc) {
      StartPos.SetXYZ(fRes->GetX(), fRes->GetY(), fRes->GetZ());
      StartMom.SetXYZ(fRes->GetPx(), fRes->GetPy(), fRes->GetPz());

      covSeed(0, 0) = fRes->GetDX() * fRes->GetDX();
      covSeed(1, 1) = fRes->GetDY() * fRes->GetDY();
      covSeed(2, 2) = fRes->GetDZ() * fRes->GetDZ();

      covSeed(3, 3) = fRes->GetDPx() * fRes->GetDPx();
      covSeed(4, 4) = fRes->GetDPy() * fRes->GetDPy();
      covSeed(5, 5) = fRes->GetDPz() * fRes->GetDPz();
    }
  } else if (fPropagateDistance > 0.f) {
    // Calculating params at fPropagateDistance cm before the first hit
    FairTrackParP par = tBefore->GetParamFirst();
    Int_t ierr = 0;
    FairTrackParH *helix = new FairTrackParH(&par, ierr);
    FairGeanePro *fPro0 = new FairGeanePro();
    if (fVerbose == 0)
      fPro0->SetPrintErrors(kFALSE);
    FairTrackParH *fRes = new FairTrackParH();
    fPro0->PropagateToLength(-fPropagateDistance);
    Bool_t rc = fPro0->Propagate(helix, fRes, PDGCode);
    if (rc) {
      StartPos.SetXYZ(fRes->GetX(), fRes->GetY(), fRes->GetZ());
      StartMom.SetXYZ(fRes->GetPx(), fRes->GetPy(), fRes->GetPz());

      covSeed(0, 0) = fRes->GetDX() * fRes->GetDX();
      covSeed(1, 1) = fRes->GetDY() * fRes->GetDY();
      covSeed(2, 2) = fRes->GetDZ() * fRes->GetDZ();

      covSeed(3, 3) = fRes->GetDPx() * fRes->GetDPx();
      covSeed(4, 4) = fRes->GetDPy() * fRes->GetDPy();
      covSeed(5, 5) = fRes->GetDPz() * fRes->GetDPz();
    }
  }

  TVector3 plane_v1, plane_v2;
  if (fPerpPlane) {
    plane_v1 = StartMom.Orthogonal();
    plane_v2 = StartPos.Cross(plane_v1);
  } else {
    plane_v1.SetXYZ(1., 0., 0.);
    plane_v2.SetXYZ(0., 1., 0.);
  }

  PndTrackCand trackCand = tBefore->GetTrackCand();

  genfit::AbsTrackRep *rep = new genfit::RKTrackRep(PDGCode);
  // PndTrackCand does not store seed, then PndTrackCand2Genfit2TrackCand cannot convert the seed.
  // You need to set the seed afterwards, taking it from PndTrack (setCovSeed/setPosMomSeedAndPdgCode)
  genfit::TrackCand *gfCand = PndTrackCand2Genfit2TrackCand(&trackCand);
  gfCand->setCovSeed(covSeed);
  genfit::Track *trk = new genfit::Track(*gfCand, *fTheRecoHitFactory, rep);

  // Start Fitter
  try {
    fGenFitter.processTrack(trk);
  } catch (genfit::Exception &e) {
    std::cout << "*** PndRecoDafFit2::Fit"
              << "\t"
              << "FITTER EXCEPTION ***" << std::endl;
    std::cout << e.what() << std::endl;
  }

  if (fVerbose > 0)
    std::cout << "** PndRecoDafFit2::Fit"
              << "\t"
              << "SUCCESSFULL FIT!" << std::endl;

  try {
    tAfter = (PndTrack *)Genfit2Track2PndTrack(trk);
  } catch (genfit::Exception &e) {
    std::cout << "*** PndRecoDafFit2::Fit"
              << "\t"
              << "CONVERSION EXCEPTION ***" << std::endl;
    std::cout << e.what() << std::endl;
    tAfter = tBefore;
    tAfter->SetFlag(-2); // flag -2: conversion failed
  }

  if (fVerbose > 0)
    std::cout << "*** PndRecoDafFit2::Fit"
              << "\t"
              << "Fitting done" << std::endl;

  return tAfter;
}

ClassImp(PndRecoDafFit2);
