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
 * PndGFRecoFit.cxx
 *
 *  Created on: 20.04.2022
 *      Author: tstockmanns
 */

#include <PndGFRecoFit.h>
#include "PndGenfitField2.h"
#include "PndGenfitAdapters2.h"
#include "TGeoMaterialInterface.h"
#include "TGeoManager.h"
#include "FairLogger.h"

#include "FairTrackParH.h"

#include "FieldManager.h"
#include "MaterialEffects.h"
#include "Track.h"
#include "KalmanFitter.h"
#include "KalmanFitterRefTrack.h"

PndGFRecoFit::PndGFRecoFit()
{
  // TODO Auto-generated constructor stub
}

PndGFRecoFit::~PndGFRecoFit()
{
  if (fPro0 != nullptr) {
    delete fPro0;
  }
  if (fFitter != nullptr) {
    delete fFitter;
  }

  if (fHitFactory != nullptr) {
    delete fHitFactory;
  }
}

void PndGFRecoFit::Init()
{
  PndGFRecoHitFactoryCreator creator;
  creator.Init();
  fHitFactory = creator.GetFactory();

  genfit::FieldManager::getInstance()->init(new PndGenfitField2());
  genfit::MaterialEffects::getInstance()->init(new genfit::TGeoMaterialInterface());

  fPro0 = new FairGeanePro();

  if (fFitter == nullptr) {
    SetDefaultFitter();
  }
  if (fVerbose < 2)
    genfit::Exception::quiet(true);
}

PndTrack *PndGFRecoFit::Fit(PndTrack *prefit, int pdgcode)
{
  PndTrack *tAfter = nullptr;
  if (fVerbose > 0)
    LOG(info) << "PndGFRecoFit::Fit";
  if (fabs(prefit->GetParamFirst().GetPz()) < 1e-9) {
    tAfter = prefit;
    tAfter->SetFlag(-10);
    LOG(warning) << "Pz value close to 0. Set tAfter = prefit " << prefit->GetParamFirst().GetPz();
    return tAfter; // flag -10 : pz==0
  }

  // Int_t fCharge = prefit->GetParamFirst().GetQ(); //[R.K. 01/2017] unused variable?
  TVector3 StartPos(prefit->GetParamFirst().GetX(), prefit->GetParamFirst().GetY(), prefit->GetParamFirst().GetZ());
  TVector3 StartMom(prefit->GetParamFirst().GetPx(), prefit->GetParamFirst().GetPy(), prefit->GetParamFirst().GetPz());

  TMatrixDSym covSeed(6);
  covSeed(0, 0) = prefit->GetParamFirst().GetDX() * prefit->GetParamFirst().GetDX();
  covSeed(1, 1) = prefit->GetParamFirst().GetDY() * prefit->GetParamFirst().GetDY();
  covSeed(2, 2) = prefit->GetParamFirst().GetDZ() * prefit->GetParamFirst().GetDZ();

  covSeed(3, 3) = prefit->GetParamFirst().GetDPx() * prefit->GetParamFirst().GetDPx();
  covSeed(4, 4) = prefit->GetParamFirst().GetDPy() * prefit->GetParamFirst().GetDPy();
  covSeed(5, 5) = prefit->GetParamFirst().GetDPz() * prefit->GetParamFirst().GetDPz();

  FairTrackParP par = prefit->GetParamFirst();
  Int_t ierr = 0;
  FairTrackParH helix(&par, ierr);

  if (fVerbose == 0)
    fPro0->SetPrintErrors(kFALSE);
  FairTrackParH fRes;

  if (fPropagateToIP) {
    // Calculating params at PCA to Origin

    fPro0->SetPoint(TVector3(0, 0, 0));
    fPro0->PropagateToPCA(1, -1);
    Bool_t rc = fPro0->Propagate(&helix, &fRes, pdgcode);
    if (rc) {
      StartPos.SetXYZ(fRes.GetX(), fRes.GetY(), fRes.GetZ());
      StartMom.SetXYZ(fRes.GetPx(), fRes.GetPy(), fRes.GetPz());

      covSeed(0, 0) = fRes.GetDX() * fRes.GetDX();
      covSeed(1, 1) = fRes.GetDY() * fRes.GetDY();
      covSeed(2, 2) = fRes.GetDZ() * fRes.GetDZ();

      covSeed(3, 3) = fRes.GetDPx() * fRes.GetDPx();
      covSeed(4, 4) = fRes.GetDPy() * fRes.GetDPy();
      covSeed(5, 5) = fRes.GetDPz() * fRes.GetDPz();
    }
  } else if (fPropagateDistance > 0.f) {
    // Calculating params at fPropagateDistance cm before the first hit
    //		FairTrackParP par = prefit->GetParamFirst();
    //		Int_t ierr = 0;
    //		FairTrackParH *helix = new FairTrackParH(&par, ierr);
    //		FairGeanePro *fPro0 = new FairGeanePro();
    //		if (fVerbose == 0)
    //			fPro0->SetPrintErrors(kFALSE);
    //		FairTrackParH *fRes = new FairTrackParH();
    fPro0->PropagateToLength(-fPropagateDistance);
    //    std::cout << "PndRekoKalmanFit2::Fit helix: ";
    //    helix.GetMomentum().Print();
    //    helix.GetPosition().Print();
    //    std::cout << std::endl;
    Bool_t rc = fPro0->Propagate(&helix, &fRes, pdgcode);
    if (rc) {
      StartPos.SetXYZ(fRes.GetX(), fRes.GetY(), fRes.GetZ());
      StartMom.SetXYZ(fRes.GetPx(), fRes.GetPy(), fRes.GetPz());

      covSeed(0, 0) = fRes.GetDX() * fRes.GetDX();
      covSeed(1, 1) = fRes.GetDY() * fRes.GetDY();
      covSeed(2, 2) = fRes.GetDZ() * fRes.GetDZ();

      covSeed(3, 3) = fRes.GetDPx() * fRes.GetDPx();
      covSeed(4, 4) = fRes.GetDPy() * fRes.GetDPy();
      covSeed(5, 5) = fRes.GetDPz() * fRes.GetDPz();
    }
  }

  //	  TVector3 plane_v1, plane_v2;
  //	  if (fPerpPlane) {
  //	    plane_v1 = StartMom.Orthogonal();
  //	    plane_v2 = StartPos.Cross(plane_v1);
  //	  } else {
  //	    plane_v1.SetXYZ(1., 0., 0.);
  //	    plane_v2.SetXYZ(0., 1., 0.);
  //	  }

  if (StartMom.Mag2() == 0) {
    LOG(warning) << "*** PndRecoKalmanFit2::Fit\tMomentum seed is ZERO. Cannot fit. ***" << std::endl;
    return prefit;
  }

  PndTrackCand trackCand = prefit->GetTrackCand();

  genfit::AbsTrackRep *rep = new genfit::RKTrackRep(pdgcode);
  // rep->setDebugLvl(fVerbose);
  // PndTrackCand does not store seed, then PndTrackCand2Genfit2TrackCand cannot convert the seed.
  // You need to set the seed afterwards, taking it from PndTrack (setCovSeed/setPosMomSeedAndPdgCode)
  genfit::TrackCand *gfCand = PndTrackCand2Genfit2TrackCand(&trackCand); // TODO: link TrackCand to track
  gfCand->setCovSeed(covSeed);
  gfCand->setPosMomSeedAndPdgCode(StartPos, StartMom, pdgcode);

  genfit::Track *trk = new genfit::Track(*gfCand, *fHitFactory, rep);
  // Start Fitter
  try {
    fFitter->processTrack(trk);
  } catch (genfit::Exception &e) {
    //		delete(trk);
    LOG(warning) << "*** PndGFRecoFit::Fit"
                 << "\t"
                 << "FITTER EXCEPTION ***";
    LOG(warning) << e.what();
  }
  if (fVerbose > 0) {
    LOG(info) << "*** PndGFRecoFit::Fit"
              << "\t"
              << "SUCCESSFULL FIT!";
    if (fVerbose > 2)
      trk->getFitStatus()->Print();
  }

  try {
    tAfter = (PndTrack *)Genfit2Track2PndTrack(trk);
  } catch (genfit::Exception &e) {
    LOG(warning) << "*** PndGFRecoFit::Fit"
                 << "\t"
                 << "CONVERSION EXCEPTION ***" << std::endl;
    LOG(warning) << e.what();
    tAfter = prefit;
    tAfter->SetFlag(-2); // flag -2: conversion failed
    //		delete(trk);
  }
  if (fGFdisplay != nullptr) {
    fGFdisplay->addEvent(trk);
  } else {
    delete (trk);
  }

  if (fVerbose > 0) {
    LOG(info) << "*** PndGFRecoFit::Fit"
              << "\t"
              << "Fitting done";
    LOG(info) << "Track: ";
    tAfter->Print();
  }
  // delete (rep);
  return tAfter;
}

void PndGFRecoFit::SetDefaultFitter()
{
  fFitter = new genfit::KalmanFitterRefTrack();
  fFitter->setMinIterations(2);
  fFitter->setMaxIterations(2);
}
