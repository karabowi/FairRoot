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
// -----                PndMdtTrkProducerIdeal source file             -----
// -----                  Created 12/06/08  by  S.Spataro              -----
// -------------------------------------------------------------------------

#include "PndMdtTrkProducerIdeal.h"

#include "PndMdtTrk.h"
#include "PndMdtHit.h"
#include "PndMdtPoint.h"

#include "FairRootManager.h"
#include "FairDetector.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "PndMCTrack.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoNode.h"
#include "TGeoMatrix.h"
#include "TVector3.h"
#include "TRandom.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndMdtTrkProducerIdeal::PndMdtTrkProducerIdeal() : FairTask("Ideal MDT Tracklet Producer") {}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMdtTrkProducerIdeal::~PndMdtTrkProducerIdeal() {}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndMdtTrkProducerIdeal::Init()
{

  cout << "-I- PndMdtTrkProducerIdeal::Init: "
       << "INITIALIZATION *********************" << endl;

  // FairRun* sim = FairRun::Instance(); //[R.K. 01/2017] unused variable?
  // FairRuntimeDb* rtdb=sim->GetRuntimeDb(); //[R.K. 01/2017] unused variable?

  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndMdtTrkProducerIdeal::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fMCArray = (TClonesArray *)ioman->GetObject("MCTrack");
  if (!fMCArray) {
    cout << "-W- PndMdtTrkProducerIdeal::Init: "
         << "No MCTrack array!" << endl;
    return kERROR;
  }

  fPointArray = (TClonesArray *)ioman->GetObject("MdtPoint");
  if (!fPointArray) {
    cout << "-W- PndMdtTrkProducerIdeal::Init: "
         << "No MdtPoint array!" << endl;
    return kERROR;
  }

  fHitArray = (TClonesArray *)ioman->GetObject("MdtHit");
  if (!fHitArray) {
    cout << "-W- PndMdtTrkProducerIdeal::Init: "
         << "No MdtHit array!" << endl;
    return kERROR;
  }

  // Create and register output array
  fTrkArray = new TClonesArray("PndMdtTrk");

  ioman->Register("MdtTrk", "Mdt", fTrkArray, kTRUE);

  LOG(info) << " PndMdtTrkProducerIdeal: Intialization successfull";

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndMdtTrkProducerIdeal::Exec(Option_t *)
{

  // Reset output array
  if (!fTrkArray)
    Fatal("Exec", "No TrkArray");

  fTrkArray->Clear();

  Int_t nHits = fHitArray->GetEntriesFast();
  if (nHits == 0)
    return; // exit if the event contains no Mdt hits

  PndMCTrack *mcTrack = nullptr;
  PndMdtHit *mdtHit = nullptr;
  PndMdtPoint *mdtPoint = nullptr;

  // Loop over MCTrack
  Int_t nMCTrack = fMCArray->GetEntriesFast();

  for (Int_t iMc = 0; iMc < nMCTrack; iMc++) {
    mcTrack = (PndMCTrack *)fMCArray->At(iMc);
    if (mcTrack->GetNPoints(DetectorId::kMDT) == 0)
      continue;

    PndMdtTrk *mdtTrk = new PndMdtTrk();
    Int_t mdtCount = 0;
    for (Int_t iHit = 0; iHit < nHits; iHit++) {
      mdtHit = (PndMdtHit *)fHitArray->At(iHit);
      mdtPoint = (PndMdtPoint *)fPointArray->At(mdtHit->GetRefIndex());
      if (mdtPoint->GetTrackID() != iMc)
        continue;

      mdtTrk->SetHitIndex(mdtHit->GetLayerID(), iHit);
      if (mdtTrk->GetModule() == 0) {
        mdtTrk->SetModule(mdtHit->GetModule());
      } else if (mdtHit->GetModule() != mdtTrk->GetModule()) {
        mdtTrk->SetModule(10);
      }

      mdtCount++;
    } // end of MdtHit loop

    mdtTrk->SetHitCount(mdtCount);
    if (mdtCount > 0)
      AddTrk(mdtTrk);

  } // end of MCTrack loop
}
// -------------------------------------------------------------------------

// -----   Private method AddTrk   --------------------------------------------
PndMdtTrk *PndMdtTrkProducerIdeal::AddTrk(PndMdtTrk *track)
{
  // Creates a new hit in the TClonesArray.

  TClonesArray &trkRef = *fTrkArray;
  Int_t size = trkRef.GetEntriesFast();
  return new (trkRef[size]) PndMdtTrk(*track);
}
// ----

ClassImp(PndMdtTrkProducerIdeal)
