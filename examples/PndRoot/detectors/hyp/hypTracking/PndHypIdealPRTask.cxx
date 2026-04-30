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
//      Implementation of class PndHypIdealPRTask
//      see PndHypIdealPRTask.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//      Alicia Sanchez HIM modified for hypernuclei
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndHypIdealPRTask.h"

// C/C++ Headers ----------------------
#include <algorithm>
#include <iostream>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "TArrayD.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairField.h"
#include "FairLogger.h"

//#include "../../tof/PndTofPoint.h"
#include "PndHypPoint.h"
#include "PndHypHit.h"
#include "PndMCTrack.h"

//#include "../../recotasks/PndFieldAdaptor.h"

#include "FairGeanePro.h"
#include "FairTrackParP.h"
#include "TRandom.h"

// Class Member definitions -----------

PndHypIdealPRTask::PndHypIdealPRTask() : FairTask("Ideal Pattern Reco"), fPersistence(kFALSE), fEventNr(0) {}

PndHypIdealPRTask::~PndHypIdealPRTask()
{
  if (fPH != nullptr)
    delete fPH;
}

InitStatus PndHypIdealPRTask::ReInit()
{

  InitStatus stat = kERROR;
  return stat;
}

InitStatus PndHypIdealPRTask::Init()
{
  // Get ROOT Manager
  FairRootManager *ioman = FairRootManager::Instance();

  if (ioman == 0) {
    Error("PndHypIdealPRTask::Init", "RootManager not instantiated!");
    return kERROR;
  }

  // open hit arrays
  std::map<unsigned int, TString>::iterator iter = fHitBranchNameMap.begin();
  while (iter != fHitBranchNameMap.end()) {
    TClonesArray *ar = (TClonesArray *)ioman->GetObject(iter->second);
    if (ar == 0) {
      Error("PndHypDPRTask::Init", "point-array %s not found!", iter->second.Data());
    } else {
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
    Error("PndHypDPRTask::Init", "hyp hit-array not found!");
    return kERROR;
  }

  fSdsArray = (TClonesArray *)ioman->GetObject("MVDPoint");
  if (fSdsArray == nullptr) {
    Error("PndHypDPRTask::Init", "mvd hit-array not found!");
    return kERROR;
  }

  // create and register output array
  fTrackArray = new TClonesArray("GFTrackCand");
  ioman->Register("HypTrackCand", "HYP", fTrackArray, fPersistence);

  // setup histograms
  fPH = new TH1D("pH", "p", 1000, 0.02, 0.3);

  LOG(info) << " PndHypIdealPRTask: Initialisation successfull";

  return kSUCCESS;
}

void PndHypIdealPRTask::Exec(Option_t *)
{
  std::cout << "PndHypIdealPRTask::Exec" << std::endl;
  // Reset output Array
  if (fTrackArray == nullptr)
    Fatal("PndHypIdealPR::Exec)", "No TrackArray");
  fTrackArray->Delete();

  // use McId to distinguish data from different tracks
  // std::map<unsigned int,GFTrackCand*> candmap;
  std::cout << "<<<<< Event " << fEventNr++ << " <<<" << std::endl;

  std::map<unsigned int, TClonesArray *>::iterator iter = fHitBranchMap.begin();
  while (iter != fHitBranchMap.end()) {

    fHitArray = iter->second;
    // loop over points
    Int_t np = fHitArray->GetEntriesFast();
    FairMCPoint *point;
    FairHit *hit;

    for (Int_t ip = 0; ip < np; ++ip) {

      if (iter->first == 2) {
        hit = (PndHypHit *)fHitArray->At(ip);

        point = (PndHypPoint *)fPointArray->At(hit->GetRefIndex());
      } else if (iter->first == 3) {
        hit = (PndSdsHit *)fHitArray->At(ip);
        point = (PndSdsMCPoint *)fSdsArray->At(hit->GetRefIndex());
      }
      if (point == 0)
        continue;

      unsigned int id = point->GetTrackID();
      // cut on insane ids
      if (id > 10000)
        continue;
      // look for track in mc map
      AddHitToTrack(id, iter->first, ip);

      /* if(candmap[id]==nullptr){
  // create new track
  //std::cout<<"Creating new track candidate with id="<<id<<std::endl;
  candmap[id]=new GFTrackCand;
      }
      // add hit to track
      // set detectorid=2;
      candmap[id]->addHit(iter->first,ip);
      */
    } // end loop over cluster

    ++iter;
  }

  // ------------------------------------------------------------------------
  // try to find some starting values
  // loop over tracks

  std::map<Int_t, GFTrackCand *>::iterator candIter = fTrackCandMap.begin();
  while (candIter != fTrackCandMap.end()) {
    GFTrackCand *cand = candIter->second;
    if (cand->getNHits() < 3) {
      ++candIter;
      continue;
    }
    TVector3 frmom;
    unsigned int detId, hitId;
    cand->getHit(0, detId, hitId);
    PndHypHit *myHit;
    myHit = (PndHypHit *)fHitArray->At(hitId);
    FairMCPoint *pointF;

    if (detId == 2)
      pointF = (FairMCPoint *)fPointArray->At(myHit->GetRefIndex());
    else {
      LOG(error) << "detId != 2 " << detId;
      return;
    }
    std::cout << "Detector no. " << detId << ": " << *pointF;
    pointF->Momentum(frmom);
    fPH->Fill(frmom.Mag());

    // create track object

    new ((*fTrackArray)[fTrackArray->GetEntriesFast()]) GFTrackCand(*(candIter->second));

    ++candIter;
  } // end loop over tracks

  std::cout << fTrackArray->GetEntriesFast() << " tracks created " << std::endl;

  ClearTrackCandMap();
  fHitArray->Delete();
  fMcArray->Delete();

  // return;
}

void PndHypIdealPRTask::AddHitToTrack(Int_t trackID, Int_t detnum, Int_t iHit)
{

  if (fTrackCandMap[trackID] == nullptr) {
    // create new track
    // std::cout<<"Creating new track candidate with id="<<id<<std::endl;

    GFTrackCand *myTCand = new GFTrackCand();

    // Get MCTrack
    PndMCTrack *mc = (PndMCTrack *)fMcArray->At(trackID);

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

    myTCand->setTrackSeed(mc->GetStartVertex(), mc->GetMomentum(), q / mc->GetMomentum().Mag());

    myTCand->setMcTrackId(trackID);
    myTCand->setPdgCode(pdg);
    fTrackCandMap[trackID] = myTCand;
  }
  // add hit to track
  // set detectorid=2;
  fTrackCandMap[trackID]->addHit(detnum, iHit);
}

Int_t PndHypIdealPRTask::GetChargeIon(Int_t ion)
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

void PndHypIdealPRTask::ClearTrackCandMap()
{
  for (std::map<Int_t, GFTrackCand *>::const_iterator ci = fTrackCandMap.begin(); ci != fTrackCandMap.end(); ci++) {
    delete (ci->second);
  }
  fTrackCandMap.clear();
}

void PndHypIdealPRTask::WriteHistograms()
{

  FairSink *sink = FairRootManager::Instance()->GetSink();

  if (sink->GetSinkType() == kFILESINK) {
    TDirectory::TContext restorecwd{};
    TFile *outfile = dynamic_cast<FairRootFileSink *>(sink)->GetRootFile();
    outfile->mkdir("HypPrefit");
    outfile->cd("HypPrefit");

    outfile->WriteTObject(fPH);
    delete (fPH);
    fPH = nullptr;

    outfile->cd("..");

    outfile->Close();
    delete (outfile);
  }

  //  TFile *file = FairRootManager::Instance()->GetOutFile();
  //  file->cd();
  //  file->mkdir("HypPrefit");
  //  file->cd("HypPrefit");
  //
  //  fPH->Write();
  //  delete fPH;
  //  fPH = nullptr;
  //
  //  file->Close();
  //  delete file;
}

ClassImp(PndHypIdealPRTask)
