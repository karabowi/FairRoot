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
// -----                PndGemTrackFinderIdeal source file             -----
// -----                Created 19.03.2009 by R. Karabowicz            -----
// -----              according to the PndDchTrackFinderIdeal          -----
// -------------------------------------------------------------------------

#include "PndGemTrackFinderIdeal.h"

// Fair includes
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"
#include "FairTrackParam.h"
#include "FairRootManager.h"
#include "FairLogger.h"
#include "FairMCPoint.h"

// Pnd includes
#include "PndGemMCPoint.h"
#include "PndDetectorList.h"
#include "PndTrack.h"
#include "PndTrackCand.h"
#include "PndTrackCandHit.h"

// ROOT includes
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TMatrixFSym.h"

// C++ includes
#include <iostream>
#include <map>
#include <cmath>
using std::cout;
using std::endl;
using std::map;

// -----   Default constructor   -------------------------------------------
PndGemTrackFinderIdeal::PndGemTrackFinderIdeal()
{
  fMCTrackArray = nullptr;
  fMCPointArray = nullptr;
  fNofEvents = 0;
  fVerbose = 0;
  fPrimary = 0;
}

// -----   Destructor   ----------------------------------------------------
PndGemTrackFinderIdeal::~PndGemTrackFinderIdeal() {}

// -----   Init  -----------------------------------------------------------
void PndGemTrackFinderIdeal::Init()
{

  // Get and check FairRootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- " << GetName() << "::Init: "
         << "RootManager not instantised!" << endl;
    return;
  }

  // Get the pointer to the singleton FairRunAna object
  FairRunAna *ana = FairRunAna::Instance();
  if (nullptr == ana) {
    cout << "-E- " << GetName() << "::Init :"
         << " no FairRunAna object!" << endl;
    return;
  }
  // Get the pointer to run-time data base
  FairRuntimeDb *rtdb = ana->GetRuntimeDb();
  if (nullptr == rtdb) {
    cout << "-E- " << GetName() << "::Init :"
         << " no runtime database!" << endl;
    return;
  }

  // Get MCTrack array
  fMCTrackArray = (TClonesArray *)ioman->GetObject("MCTrack");
  if (!fMCTrackArray) {
    LOG(error) << " " << GetName() << "::Init: No MCTrack array!";
    return;
  }

  // Get PndGemPoint (MCPoint) array
  fMCPointArray = (TClonesArray *)ioman->GetObject("GEMPoint");
  if (!fMCPointArray) {
    LOG(error) << " " << GetName() << "::Init: No MCPoint array!";
    return;
  }
  if (fVerbose)
    cout << "fMCPointArray #: " << fMCPointArray->GetEntriesFast() << endl;

  // Geometry loading
  // TFile *infile = ioman->GetInFile();
  // TGeoManager *geoMan = (TGeoManager*) infile->Get("FAIRGeom");
  // fGemStructure = PndGemStructure::Instance(geoMan);

  LOG(info) << " " << GetName() << ": Intialization successfull";
}

// -----   Public method DoFind   ------------------------------------------
Int_t PndGemTrackFinderIdeal::DoFind(TClonesArray *hitArray, TClonesArray *trackArray, TClonesArray *trackCandArray)
{

  // Count events
  fNofEvents++;
  if (fVerbose) {
    cout << endl << endl << endl << endl;
    cout << "=======================================================" << endl;
    LOG(info) << "        Event No: " << fNofEvents;
    cout << "=======================================================" << endl;

    LOG(info) << " " << GetName() << "::DoFind ";
    cout << "-------------------------------------------------------" << endl;
    cout << "     ### Start DoFind" << endl;
    cout << "-------------------------------------------------------" << endl;
  }

  // Check pointers
  if (!fMCTrackArray) {
    cout << "-E- PndGemTrackFinderIdeal::DoFind: "
         << "MCTrack array missing! " << endl;
    return -1;
  }

  if (!fMCPointArray) {
    cout << "-E- " << GetName() << "::DoFind: "
         << "MCPoint array missing! " << endl;
    return -1;
  }

  if (!hitArray) {
    cout << "-E- " << GetName() << "::DoFind: "
         << "Hit arrays missing! " << endl;
    return -1;
  }

  // Initialise control counters
  Int_t nNoMCTrack = 0;
  Int_t nNoTrack = 0;
  Int_t nNoGemPoint = 0;
  Int_t nNoGemHit = 0;

  // Create pointers to GemHit and GemPoint
  PndGemHit *gemHit = nullptr;
  FairMCPoint *mcPoint = nullptr;
  PndMCTrack *mcTrack = nullptr;
  PndTrack *gemTrack = nullptr;
  PndTrackCand *gemTrackCand = nullptr;
  PndTrackCandHit tcHit;

  // Declare variables outside the loop
  Int_t ptIndex = 0;      // MC point index
  Int_t mcTrackIndex = 0; // MC track index
  Int_t trackIndex = 0;   // Gem track index
  Int_t relDetID = -1;    // 3000;   //

  // Create STL map from MCTrack index to number of hits
  std::map<Int_t, Int_t> hitMap;
  std::map<Int_t, Double_t> trackPMap;
  std::map<Int_t, Int_t>::iterator itHitMap;

  if (fVerbose) {
    // Size of fMCTrackArray
    cout << "# MC Tracks: " << fMCTrackArray->GetEntriesFast() << endl;
    // Size of fMCTrackArray
    cout << "# MC Points: " << fMCPointArray->GetEntriesFast() << endl;
  }

  // Number of Gem hits
  Int_t nGemHits = hitArray->GetEntriesFast();
  if (fVerbose > 2)
    cout << "# GemHits: " << nGemHits << endl;

  for (Int_t iHit = 0; iHit < nGemHits; iHit++) {
    // Get the pointer to Gem hit
    gemHit = (PndGemHit *)hitArray->At(iHit);
    if (gemHit->GetDetectorID() > relDetID) {
      if (nullptr == gemHit)
        continue;

      // Get point index
      ptIndex = gemHit->GetRefIndex();

      if (ptIndex == -1)
        continue;

      // Get pointer to MC point
      mcPoint = (FairMCPoint *)fMCPointArray->At(ptIndex);
      if (nullptr == mcPoint)
        continue;

      // Get MC track index
      mcTrackIndex = mcPoint->GetTrackID();

      trackPMap[mcTrackIndex] = TMath::Sqrt(mcPoint->GetPx() * mcPoint->GetPx() + mcPoint->GetPy() * mcPoint->GetPy() + mcPoint->GetPz() * mcPoint->GetPz());

      // Increment the number of hits
      hitMap[mcTrackIndex] += 1;
    }
  }

  // Create STL map from MCTrack index to GemTrack index
  map<Int_t, Int_t> trackMap;

  // Loop over reconstructable MCTracks and create corresponding GemTrack
  Int_t nMCacc = 0;  // Accepted MC tracks
  Int_t nTracks = 0; // Reconstructable MC tracks
  Int_t nMCTracks = fMCTrackArray->GetEntriesFast();

  if (fVerbose > 2) {
    cout << "# of MC Tracks (nMCTracks): " << nMCTracks << endl;
  }

  for (Int_t iMCTrack = 0; iMCTrack < nMCTracks; iMCTrack++) {
    mcTrack = (PndMCTrack *)fMCTrackArray->At(iMCTrack);
    if (!mcTrack)
      continue;

    // Either Primary or All MC tracks are considered
    if (fPrimary) {
      if (mcTrack->GetMotherID() != -1 && fabs((mcTrack->GetStartVertex()).Z()) > 1.)
        continue;

      if (fVerbose > 2) {
        cout << "MC track #: " << iMCTrack << "\t MotherID: " << mcTrack->GetMotherID() << "\t StartVertex Z: " << mcTrack->GetStartVertex().Z() << endl;
      }
    }
    //----------------------------------------------------
    nMCacc++;

    if (fVerbose > 2) {
      cout << iMCTrack << ": #GemPoints in MCTrack: " << mcTrack->GetNPoints(PndDetectorId::kGEM) << " -> " << hitMap[iMCTrack] << endl;
    }
    if (!hitMap[iMCTrack])
      continue;

    gemTrackCand = new ((*trackCandArray)[nTracks]) PndTrackCand();

    // Loop over hits. Get corresponding MCPoint and MCTrack index
    for (Int_t iHit = 0; iHit < nGemHits; iHit++) {
      gemHit = (PndGemHit *)hitArray->At(iHit);
      if (!gemHit) {
        nNoGemHit++;
        continue;
      }

      ptIndex = gemHit->GetRefIndex();
      if (ptIndex < 0)
        continue; // fake or background hit
      mcPoint = (FairMCPoint *)fMCPointArray->At(ptIndex);

      if (!mcPoint) {
        nNoGemPoint++;
        continue;
      }

      mcTrackIndex = mcPoint->GetTrackID();

      if (mcTrackIndex < 0 || mcTrackIndex > nMCTracks) {
        cout << "-E- " << GetName() << "::DoFind: "
             << "MCTrack index out of range. " << mcTrackIndex << " " << nMCTracks << endl;
        nNoMCTrack++;
        continue;
      }
      if (mcTrackIndex != iMCTrack)
        continue;

      gemTrackCand->AddHit(FairRootManager::Instance()->GetBranchId("GEMHit"), iHit, gemHit->GetPosition().Mag());

      if (fVerbose > 3) {
        cout << "GEM hit " << iHit << " from GEM point " << ptIndex << " (" << mcTrackIndex << ") "
             << "added to GEM track " << trackIndex << endl;
      }
    }

    gemTrackCand->Sort();

    //    cout << "gemTrack " << nTracks << " has " << gemTrackCand->GetNHits() << " hits" << endl;
    PndTrackCandHit tch = gemTrackCand->GetSortedHit(0);

    gemHit = (PndGemHit *)hitArray->At(tch.GetHitId());

    TVector3 pos;
    TVector3 mom;

    ptIndex = gemHit->GetRefIndex();
    mcPoint = (FairMCPoint *)fMCPointArray->At(ptIndex);

    gemHit->Position(pos);
    mcPoint->Momentum(mom);

    mcTrack = (PndMCTrack *)fMCTrackArray->At(mcPoint->GetTrackID());

    int pdg = mcTrack->GetPdgCode();
    double charge = 0.;
    if (pdg < 100000000)
      charge = TDatabasePDG::Instance()->GetParticle(pdg)->Charge() / 3.;

    //     gemTrackCand->setMcTrackId(iMCTrack);

    FairTrackParP firstPar(pos, mom, TVector3(0.5, 0.5, 0.5), 0.1 * mom, (Int_t)charge, pos, TVector3(1., 0., 0.), TVector3(0., 1., 0.));
    FairTrackParP lastPar(pos, mom, TVector3(0.5, 0.5, 0.5), 0.1 * mom, (Int_t)charge, pos, TVector3(1., 0., 0.), TVector3(0., 1., 0.));

    gemTrack = new ((*trackArray)[nTracks]) PndTrack(firstPar, lastPar, *gemTrackCand);

    gemTrack->SetRefIndex(iMCTrack);

    trackMap[iMCTrack] = nTracks;

    nTracks++;
  }

  if (fVerbose) {
    cout << endl;
    cout << "-------------------------------------------------------" << endl;
    LOG(info) << "      " << GetName() << ": Event summary      -I-";
    cout << "-------------------------------------------------------" << endl;
    cout << "Total Gem hits:  " << nGemHits << endl;
    cout << "MC tracks total: " << nMCTracks << ", accepted: " << nMCacc << ", reconstructable: " << nTracks << endl;
    if (nNoGemHit)
      cout << "GemHits not found   : " << nNoGemHit << endl;
    if (nNoGemPoint)
      cout << "GemPoints not found : " << nNoGemPoint << endl;
    if (nNoMCTrack)
      cout << "MCTracks not found  : " << nNoMCTrack << endl;
    if (nNoTrack)
      cout << "GemTracks not found : " << nNoTrack << endl;
    cout << "------------------------------------------------------" << endl;
    cout << endl;
  } else {
    //     cout << "All: "        << nMCTracks
    // 	 << ", Accepted: "      << nMCacc
    // 	 << ", Reconstructed: " << nTracks << endl;
  }

  return nTracks;
}

ClassImp(PndGemTrackFinderIdeal)
