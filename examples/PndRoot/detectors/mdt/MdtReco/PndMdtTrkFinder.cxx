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
// -----                PndMdtTrkFinder source file                  -----
// -----                  Created 16/06/08  by  S.Spataro              -----
// -------------------------------------------------------------------------

#include "PndMdtTrkFinder.h"
#include "PndMdtMuonFilter.h"
#include "PndMdtGeoConstructorFast.h"
#include "PndMdtTrk.h"
#include "PndMdtHit.h"
#include "PndTrack.h"
#include "PndDetectorList.h"

#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairTrackParH.h"
#include "FairLogger.h"

#include "TVector3.h"
#include "TMath.h"
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoBBox.h"
#include "TGeoMatrix.h"
#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndMdtTrkFinder::PndMdtTrkFinder() : FairTask(" MDT Tracklet Finder") {}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMdtTrkFinder::~PndMdtTrkFinder() {}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndMdtTrkFinder::Init()
{
  cout << "-I- PndMdtTrkFinder::Init: "
       << "INITIALIZATION *********************" << endl;

  // FairRun* sim = FairRun::Instance(); //[R.K. 01/2017] unused variable?
  // FairRuntimeDb* rtdb=sim->GetRuntimeDb(); //[R.K. 01/2017] unused variable?

  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndMdtTrkFinder::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  fHitArray = (TClonesArray *)ioman->GetObject("MdtHit");
  if (!fHitArray) {
    cout << "-W- PndMdtTrkFinder::Init: "
         << "No MdtHit array!" << endl;
    return kERROR;
  }

  // Create and register output array
  fTrkArray = new TClonesArray("PndMdtTrk");

  ioman->Register("MdtTrk", "Mdt", fTrkArray, kTRUE);

  LOG(info) << " PndMdtTrkFinder: Intialization successfull";

  return kSUCCESS;
}
// -------------------------------------------------------------------------

//______________________________________________________
void PndMdtTrkFinder::SetParContainers()
{

  // Get run and runtime database
  FairRun *run = FairRun::Instance();
  if (!run)
    Fatal("PndMdtTrkFinder:: SetParContainers", "No analysis run");

  FairRuntimeDb *db = run->GetRuntimeDb();
  if (!db)
    Fatal("PndMdtTrkFinder:: SetParContainers", "No runtime database");

  // Get Mdt Reconstruction parameter container
  // fRecoPar = (PndMdtRecoPar*) db->getContainer("PndMdtRecoPar");
}

//______________________________________________________

// -----   Public method Exec   --------------------------------------------
void PndMdtTrkFinder::Exec(Option_t *)
{
  // Reset output array
  fTrkArray->Delete();

  std::vector<PndMdtTrk *> fTrklets;
  std::vector<PndMdtTrk *>::iterator tit = fTrklets.begin();
  std::vector<PndMdtTrk *>::iterator tnd = fTrklets.end();
  std::map<unsigned long, std::vector<PndMdtHit *>> fTrkHitMap;
  //
  Int_t nHits = fHitArray->GetEntriesFast();

  for (Int_t ihit = 0; ihit < nHits; ++ihit) {
    PndMdtHit *aHit = (PndMdtHit *)fHitArray->At(ihit);

    Bool_t NotUsed = kTRUE;
    for (; tit != tnd; ++tit) {
      // PndMdtTrk* aTrk = *tit; //[R.K. 01/2017] unused variable?
    }

    if (NotUsed) {
      PndMdtTrk *aTrk = new PndMdtTrk();
      unsigned long fPointer = (unsigned long)aTrk;
      fTrkHitMap[fPointer].push_back(aHit); // a track and its hits
      fTrklets.push_back(aTrk);
    }
  }
}

// -----   Private method MdtMapping   --------------------------------------------

// -----   Private method AddTrk   --------------------------------------------
PndMdtTrk *PndMdtTrkFinder::AddTrk(PndMdtTrk *track)
{
  // Creates a new hit in the TClonesArray.

  //  Float_t chi2=0;
  //   for (Int_t ll=1; ll<10; ll++)
  //     {
  //       if (track->GetHitBit(ll)==0) continue;
  //       if (ll==1)
  // 	chi2 = chi2 + (track->GetHit(ll)/1.5)*(track->GetHitAngle(ll)/1.5); // manual correction for the first layer
  //       else
  // 	chi2 = chi2 + track->GetHitAngle(ll)*track->GetHitAngle(ll);
  //     }
  //   track->SetChi2(chi2);

  TClonesArray &trkRef = *fTrkArray;
  Int_t size = trkRef.GetEntriesFast();
  return new (trkRef[size]) PndMdtTrk(*track);
}

ClassImp(PndMdtTrkFinder)
