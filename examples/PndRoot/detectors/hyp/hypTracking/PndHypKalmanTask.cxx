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
//      Implementation of class PndMvdKalmanTask
//      see PndMvdKalmanTask.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//      Ralf Kliemt, TU Dresden             (Copied for MVD use)
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndHypKalmanTask.h"

// C/C++ Headers ----------------------
#include <algorithm>
#include <iostream>
#include <assert.h>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "FairRunAna.h"
#include "GFTrack.h"
#include "TDatabasePDG.h"
#include "PndHypHit.h"
#include "FairMCPoint.h"
#include "PndSdsMCPoint.h"
#include "PndSdsHit.h"
#include "PndSdsRecoHit.h"
//#include "PndHypRecoHit.h"
#include "PndHypRecoSPHit.h"
#include "PndGeoHandling.h"

#include "FairField.h"
#include "PndFieldAdaptor.h"
#include "GFFieldManager.h"

#include "GFRecoHitFactory.h"
#include "GFKalman.h"
#include "GFException.h"
#include "TH1D.h"
#include "TFile.h"
#include "TGeoTrack.h"
#include "TGeoManager.h"
#include "TLorentzVector.h"
#include "GFDetPlane.h"

#include "TDatabasePDG.h"
#include "FairTrackParH.h"

#include "LSLTrackRep.h"
#include "GeaneTrackRep.h"
#include "FairGeanePro.h"

// Class Member definitions -----------

PndHypKalmanTask::PndHypKalmanTask() : FairTask("Kalman Filter"), fPersistence(kFALSE)
{
  fTrackBranchName = "HypTrackCand";
  // PndGeoHandling::Instance();
}

PndHypKalmanTask::~PndHypKalmanTask()
{
  if (fPH != nullptr)
    delete fPH;
  if (fChi2H != nullptr)
    delete fChi2H;
}

InitStatus PndHypKalmanTask::Init()
{
  // Get ROOT Manager
  FairRootManager *ioman = FairRootManager::Instance();

  if (ioman == 0) {
    Error("PndHypKalmanTask::Init", "RootManager not instantiated!");
    return kERROR;
  }

  // Get input collection
  fTrackArray = (TClonesArray *)ioman->GetObject(fTrackBranchName);

  if (fTrackArray == nullptr) {
    Error("PndHypKalmanTask::Init", "track-array not found!");
    return kERROR;
  }

  // Build hit factory -----------------------------
  fTheRecoHitFactory = new GFRecoHitFactory();

  std::map<unsigned int, TString>::iterator iter = fHitBranchMap.begin();

  while (iter != fHitBranchMap.end()) {

    TClonesArray *har = (TClonesArray *)ioman->GetObject("HypHit");

    // cout<<" "<<iter->second<<endl;

    if (har == 0) {

      Error("PndHypKalmanTask::Init", "Hit array not found");
    } else {

      if (iter->first == 2)
        fTheRecoHitFactory->addProducer(iter->first, new GFRecoHitProducer<PndHypHit, PndHypRecoSPHit>(har));
    }

    TClonesArray *sar = (TClonesArray *)ioman->GetObject("MVDHit");
    if (sar == 0) {

      Error("PndHypKalmanTask::Init", "Hit array not found");
    } else {

      if (iter->first == 3)
        fTheRecoHitFactory->addProducer(iter->first, new GFRecoHitProducer<PndSdsHit, PndSdsRecoHit>(sar));
    }

    ++iter;
  } // end loops over hit types

  // create and register output array
  fTrArray = new TClonesArray("GFTrack");
  ioman->Register("Track", "GenFit", fTrArray, fPersistence);

  // fPro = new FairGeanePro();

  // setup histograms
  fPH = new TH1D("pH", "p", 100, 0.4, 0.6);
  fChi2H = new TH1D("chi2H", "chi2", 100, 0, 20);

  return kSUCCESS;
}

void PndHypKalmanTask::Exec(Option_t *)
{
  std::cout << "PndHypKalmanTask::Exec" << std::endl;
  // Reset output Array
  if (fTrArray == nullptr)
    Fatal("Kalman::Exec)", "No TrackArray");
  fTrArray->Delete();

  Int_t ntracks = fTrackArray->GetEntriesFast();
  // Detailed output

  if (ntracks > 20) {
    std::cout << "ntracks=" << ntracks << " Evil Event! skipping" << std::endl;
    return;
  }

  // Fitting ---------------- can go to another task!
  GFKalman fitter;

  // std::vector<TLorentzVector*> particles;
  // std::vector<Int_t> signs;

  for (Int_t itr = 0; itr < ntracks; ++itr) {

    std::cout << "starting track" << itr << std::endl;
    GFTrackCand *trcnd = (GFTrackCand *)fTrackArray->At(itr);
    unsigned int detid = 12345, index = 12345;

    // setting the magnetic field properly
    fField = FairRunAna::Instance()->GetField();
    PndFieldAdaptor *b = new PndFieldAdaptor(fField);
    GFFieldManager *fb;
    // fb->getInstance();
    // fb->init(b);

    GFAbsTrackRep *rep = 0;

    std::cout << trcnd->getNHits() << std::endl;
    // Starting values for guessing

    Int_t PDGCode = trcnd->getPdgCode();     // 2212;
    TVector3 StartPos = trcnd->getPosSeed(); // TVector3 (1.0,0.0,0.0);//cmn
    TVector3 StartPosErr = TVector3(0, 0, 0);
    TVector3 StartMom = trcnd->getDirSeed(); // TVector3 (1.,0.,1.);
    // StartMom.SetMagThetaPhi(1.05 , 70.*TMath::Pi()/360. , 0.);
    //   StartMom.SetMag(1.1);
    TVector3 StartMomErr = TVector3(0, 0, 0);
    TDatabasePDG *fdbPDG = TDatabasePDG::Instance();
    TParticlePDG *fParticle = fdbPDG->GetParticle(PDGCode);
    Double_t fCharge = fParticle->Charge();
    // calc momentum projections
    TVector3 dir = StartMom.Unit();
    double dxdz = dir.X() / dir.Z();
    double dydz = dir.Y() / dir.Z();

    rep = new LSLTrackRep(StartPos.Z(), StartPos.X(), StartPos.Y(), dxdz, dydz, trcnd->getQoverPseed(), StartPosErr.X(), StartPosErr.Y(), 0.1, 0.1, 0.1,
                          b); // nullptr instead of b field

    // what to guess here?
    /* TVector3 U(1.,0.,0.);
 TVector3 V(0.,1.,0.);

 GFDetPlane start_pl(StartPos,U,V);
 GFAbsTrackRep* rep = new GeaneTrackRep(fPro,
 start_pl,StartMom,
            StartPosErr,StartMomErr,
            fCharge,PDGCode);*/

    // GFTrack* trk= new GFTrack(rep);
    GFTrack *trk = new ((*fTrArray)[fTrArray->GetEntriesFast()]) GFTrack(rep);

    if (trk != nullptr)
      trk->setCandidate(*(GFTrackCand *)fTrackArray->At(itr));

    else {
      std::cout << " "
                << "caca de vaca " << std::endl;
      continue;
    }

    // std::cout<<" "<< trk->getTrackRep(0)->Print()<<std::endl;
    // cout<<" "<<trk->Print()<<endl;

    // GFTrack* trk=(GFTrack*)fTrackArray->At(itr);
    // Load RecoHits
    try {
      trk->addHitVector(fTheRecoHitFactory->createMany(*trcnd));

    } catch (GFException &e) {
      std::cout << e.what();
      throw e;
    }
    // Start Fitter
    try {
      std::cout << "starting fit" << std::endl;
      fitter.processTrack(trk);
    } catch (GFException e) {
      std::cout << e.what() << std::endl;
    }

    // Print Track Parameters after fit
    if (trk->getTrackRep(0)->getStatusFlag() == 0) {
      // trk->getTrackRep(0)->Print();
      GFDetPlane plane(TVector3(0, 0, 0.1), TVector3(1, 0, 0), TVector3(0, 1, 0));
      TVector3 p3 = trk->getTrackRep(0)->getMom(plane);
      Double_t p = trk->getMom().Mag();
      std::cout << " oye" << p << std::endl;

      fPH->Fill(p);

      Double_t chi2 = trk->getChiSqu();
      fChi2H->Fill(chi2);
    }
  }

  std::cout << "Fitting done" << std::endl;

  return;
}

void PndHypKalmanTask::WriteHistograms(const TString &filename)
{
  TFile *file = new TFile(filename, "UPDATE");
  //   if(file->cd("Kalman")==false) file->mkdir("Kalman");
  //   file->cd("Kalman");

  if (file->cd("Kalman"))
    file->Delete("Kalman;*");
  file->mkdir("Kalman");
  file->cd("Kalman");

  fPH->Write();
  delete fPH;
  fPH = nullptr;

  fChi2H->Write();
  delete fChi2H;
  fChi2H = nullptr;

  file->Close();
  delete file;
}

ClassImp(PndHypKalmanTask)
