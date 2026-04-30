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

#include "PndFtsTrackerIdeal.h"

//#include "PndDetectorList.h"
#include "PndMCTrack.h"
#include "PndTrackCand.h"
#include "PndTrack.h"

#include "FairTrackParP.h"
#include "FairMCPoint.h"
#include "FairHit.h"
//#include "FairMCApplication.h"
#include "FairTask.h"
//#include "FairRunAna.h"
//#include "FairGeoNode.h"
//#include "FairGeoVector.h"
//#include "FairGeoMedium.h"
#include "FairRootManager.h"
#include "FairLogger.h"

#include "TObjectTable.h"
#include "TClonesArray.h"
#include "TParticlePDG.h"
#include "TRandom.h"
#include <iostream>

// fts
#include "PndFtsPoint.h"
#include "PndFtsHit.h"
#include "PndFtsTube.h"
#include "PndFtsMapCreator.h"
// fairroot
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

using namespace std;
//________________________________________________________________
PndFtsTrackerIdeal::PndFtsTrackerIdeal()
  : FairTask("FTSTrackfinderIdeal"), fMCTracks(0), fTrackCands(0), fTracks(0), fTrackIds(0), fMinFtsHitsPerTrack(5), fMomSigma(0, 0, 0), fDPoP(0.), fRelative(kFALSE),
    fVtxSigma(0, 0, 0), fEfficiency(1.), fPersistence(kTRUE), fTracksArrayName("FTSTrkIdeal"), pdg(0)
{
  //---
  fTrackCands = new TClonesArray("PndTrackCand");
  fTracks = new TClonesArray("PndTrack");
  fVerbose = 0;
  fMomSigma.SetXYZ(0., 0., 0.);
  fVtxSigma.SetXYZ(0., 0., 0.);
  SetTrackOutput();
  fBranchActive[0] = kTRUE;
  for (int i = 1; i < 4; i++)
    fBranchActive[i] = kFALSE;
}

//_________________________________________________________________
PndFtsTrackerIdeal::~PndFtsTrackerIdeal()
{
  FairRootManager *fManager = FairRootManager::Instance();
  fManager->Write();
}

//_________________________________________________________________
void PndFtsTrackerIdeal::Register()
{
  //---
  FairRootManager::Instance()->Register(fTracksArrayName, "FTSTrk", fTracks, fPersistence);
  FairRootManager::Instance()->Register(fTracksArrayName + "Cand", "FTSTrk", fTrackCands, fPersistence);
  if (fVerbose > 3)
    Info("Register", "Done.");
}

void PndFtsTrackerIdeal::SetParContainers()
{
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fFtsParameters = (PndGeoFtsPar *)rtdb->getContainer("PndGeoFtsPar");
}

//________________________________________________________________
InitStatus PndFtsTrackerIdeal::Init()
{
  // ---
  if (fVerbose > 3)
    Info("Init", "Start initialisation.");

  FairRootManager *fManager = FairRootManager::Instance();

  // Get MC arrays
  fMCTracks = dynamic_cast<TClonesArray *>(fManager->GetObject("MCTrack"));
  if (!fMCTracks) {
    LOG(warn) << "  PndFtsTrackerIdeal::Init: No MCTrack array! Needed for MC Truth";
    return kERROR;
  }

  // FTS
  fMCPoints[0] = dynamic_cast<TClonesArray *>(fManager->GetObject("FTSPoint"));
  if (!fMCPoints[0]) {
    LOG(warn) << "  PndFtsTrackerIdeal::Init: No FTSPoint array!";
    return kERROR;
  }
  fHits[0] = dynamic_cast<TClonesArray *>(fManager->GetObject("FTSHit"));
  if (!fHits[0]) {
    LOG(warn) << "  PndFtsTrackerIdeal::Init: No FTSHit array!";
    return kERROR;
  }
  fBranchIDs[0] = FairRootManager::Instance()->GetBranchId("FTSHit");

  // GEM
  fMCPoints[1] = dynamic_cast<TClonesArray *>(fManager->GetObject("GEMPoint"));
  if (!fMCPoints[1]) {
    LOG(warn) << "  PndFtsTrackerIdeal::Init: No GEMPoint array!";
    fMCPoints[1] = new TClonesArray("FairMCPoint");
  }
  fHits[1] = dynamic_cast<TClonesArray *>(fManager->GetObject("GEMHit"));
  if (!fHits[1]) {
    LOG(warn) << "  PndFtsTrackerIdeal::Init: No GEMHit array!";
    fHits[1] = new TClonesArray("FairHit");
  }
  fBranchIDs[1] = FairRootManager::Instance()->GetBranchId("GEMHit");

  // MVD Pixel
  fMCPoints[2] = dynamic_cast<TClonesArray *>(fManager->GetObject("MVDPoint"));
  if (!fMCPoints[2]) {
    LOG(warn) << "  PndFtsTrackerIdeal::Init: No MVDPoint array!";
    fMCPoints[1] = new TClonesArray("FairMCPoint");
  }
  fHits[2] = dynamic_cast<TClonesArray *>(fManager->GetObject("MVDHitsPixel"));
  if (!fHits[2]) {
    LOG(warn) << "  PndFtsTrackerIdeal::Init: No MVDHitsPixel array!";
    fHits[2] = new TClonesArray("FairHit");
  }
  fBranchIDs[2] = FairRootManager::Instance()->GetBranchId("MVDHitsPixel");

  fMCPoints[3] = fMCPoints[2];
  fHits[3] = dynamic_cast<TClonesArray *>(fManager->GetObject("MVDHitsStrip"));
  if (!fHits[3]) {
    LOG(warn) << "  PndFtsTrackerIdeal::Init: No MVDHitsStrip array!";
    fMCPoints[3] = new TClonesArray("FairHit");
  }
  fBranchIDs[3] = FairRootManager::Instance()->GetBranchId("MVDHitsStrip");

  if (fVerbose > 3)
    Info("Init", "Fetched all arrays.");

  Register();

  pdg = new TDatabasePDG();
  if (fVerbose > 3)
    Info("Init", "End initialisation.");

  PndFtsMapCreator *mapperFts = new PndFtsMapCreator(fFtsParameters);
  fTubeArrayFts = mapperFts->FillTubeArray();

  return kSUCCESS;
}

//_________________________________________________________________
void PndFtsTrackerIdeal::Exec(Option_t *)
{
  Reset();
  if (fVerbose > 3)
    Info("Exec", "Start eventloop.");
  if (fVerbose > 4) {
    Info("Exec", "Print some array properties");
    for (int iii = 0; iii < 4; iii++) {
      std::cout << "fHits[" << iii << "] is branchID " << fBranchIDs[iii] << " with the name " << fHits[iii]->GetName() << " and contains " << fHits[iii]->GetEntriesFast()
                << " entries." << std::endl;
      std::cout << "fMCPoints[" << iii << "] with the name " << fMCPoints[iii]->GetName() << " and contains " << fMCPoints[iii]->GetEntriesFast() << " entries." << std::endl;
    }
  }

  // Do we have enough hits in the FTS in this particular event?
  if (fHits[0]->GetEntriesFast() < fMinFtsHitsPerTrack) {
    if (fVerbose > 3)
      Info("Exec", "Skip the event, since we have less than %i hits in FTS", fMinFtsHitsPerTrack);
    return;
  }

  FairHit *ghit = nullptr;
  std::map<Int_t, FairHit *> firstHit;
  std::map<Int_t, FairHit *> lastHit;
  FairMCPoint *myPoint = nullptr;
  std::map<Int_t, FairMCPoint *> firstPoint;
  std::map<Int_t, FairMCPoint *> lastPoint;
  std::map<Int_t, PndTrackCand *> candlist;

  // Loop over all hits from FTS first (later other detectors, too)
  // When looping over FTS hits create a PndTrackCand with the same index as the MC truth track if we find at least one FTS hit
  // After the FTS Hit Loop remove all candidates which do not have at least fMinFtsHitsPerTrack hits (from FTS)
  // and remove all tracks which are bent in the dipole so much that they turn around and fly towards the barrel again (this leads to problems in the fitter)
  //  I will remove such tracks at the end of the loop on FTS hits. I plan to check all (MC truth) time-ordered FTS hits associated to a given PndTrackCand and check if the
  //  z-component is increasing. If not, I will remove the PndTrackCand.
  // When looping over the other detectors only hits are added to PndTrackCand objects which were created in the loop over FTS hits and NOT removed afterwards

  // Detector loop
  for (Int_t iDet = 0; iDet < 4; iDet++) {
    if (kFALSE == fBranchActive[iDet])
      continue; // skip manually switched off detector
    if (fVerbose > 4)
      Info("Exec", "Use detector %i", iDet);
    // Hit loop
    for (Int_t ih = 0; ih < fHits[iDet]->GetEntriesFast(); ih++) {
      ghit = (FairHit *)fHits[iDet]->At(ih);
      if (!ghit) {
        if (fVerbose > 3)
          Error("Exec", "Have no ghit %i, array size: %i", ih, fHits[iDet]->GetEntriesFast());
        continue;
      }
      if (iDet == 0) {
        if (fStationsDisabled.size() > 0) {
          PndFtsHit *myhit = (PndFtsHit *)fHits[iDet]->At(ih);
          if (fStationsDisabled.find(myhit->GetLayerID()) != fStationsDisabled.end())
            continue;
        }
      }
      Int_t mchitid = ghit->GetRefIndex();
      if (mchitid < 0) {
        if (fVerbose > 3)
          Error("Exec", "Have a negative mcHit %i", mchitid);
        continue;
      }
      myPoint = (FairMCPoint *)(fMCPoints[iDet]->At(mchitid));
      if (!myPoint)
        continue;
      Int_t trackID = myPoint->GetTrackID();
      if (trackID < 0)
        continue;

      if (fVerbose > 5)
        Info("Exec", "Have a Hit %i at Track index %i", ih, trackID);

      // Continue Construction of a track candidate (start with FTS hits)
      // Track candidates and corresponding MC track index are saved in a map
      PndTrackCand *cand = candlist[trackID];
      if (nullptr == cand) {
        if (0 != iDet) {
          if (fVerbose > 5)
            Info("Exec", "Skip Hit %i, it's not connected to a Track in FTS", ih);
          continue; // skip Tracks in MVD/GEM not going to FTS
        }
        if (fVerbose > 5)
          Info("Exec", "Create new PndTrack object %i", trackID);
        cand = new PndTrackCand();
        cand->setMcTrackId(trackID);
        if (fVerbose > 5)
          Info("Exec", "Creating new PndTrack object finished %i", trackID);
      }
      if (fVerbose > 5)
        Info("Exec", "add the hit %i to trackcand %i", ih, trackID);
      cand->AddHit(fBranchIDs[iDet], ih, myPoint->GetTime());
      // Figure out if the current hit is the earliest in the event
      if (!firstHit[trackID] || firstPoint[trackID]->GetTime() > myPoint->GetTime()) {
        firstHit[trackID] = ghit;
        firstPoint[trackID] = myPoint;
      }
      // or the latest one
      if (!lastHit[trackID] || lastPoint[trackID]->GetTime() < myPoint->GetTime()) {
        lastHit[trackID] = ghit;
        lastPoint[trackID] = myPoint;
      }

      candlist[trackID] = cand; // set
    }                           // end loop over hits

    // TODO: Check if the following loops work
    // After the FTS Hit Loop remove all candidates which do not have at least fMinFtsHitsPerTrack hits (from FTS)
    // and remove all tracks which are bent in the dipole so much that they turn around and fly towards the barrel again (this leads to problems in the fitter)
    if (0 == iDet) { // only needed after PndTrkCand contain only FTS hits
      if (fVerbose > 5)
        Info("Exec", "Remove all PndTrkCand which are not realistic to be found by FTS Pattern Recognition");

      // re-iterate over candlist and save key values for PndTrkCand which are not realisitic to be found by FTS PR in a vector
      // these will be deleted after the clean loop
      std::map<Int_t, PndTrackCand *>::iterator candit;
      std::vector<Int_t> keysToDeleteFromCandList;

      if (fVerbose > 10) {
        cout << "print the map keys BEFORE cleaning\n";
        for (candit = candlist.begin(); candit != candlist.end(); ++candit) {
          cout << "trackID == " << candit->first << endl;
        }
      }

      for (candit = candlist.begin(); candit != candlist.end(); ++candit) {
        Int_t trackID = candit->first;
        PndTrackCand *tcand = candit->second;
        if (!tcand) {
          if (fVerbose > 3)
            Warning("Exec", "Have no candidate at %i", trackID);
          continue;
        }
        // remove tcand if it does not have enough hits from FTS (after run with iDet == 0 only FTS hits are filled)
        if ((int)tcand->GetNHits() < fMinFtsHitsPerTrack) { // TODO: Make this criterion more realistic
          if (fVerbose > 9) {
            Info("Exec", "Mark candlist[%i] for deletion because it has only %i FTS hits which is not enough.", trackID, tcand->GetNHits());
          }
          keysToDeleteFromCandList.push_back(trackID);
          continue;
        }

        // check if tcand is deflected too much in dipole and turns around again
        tcand->Sort();

        // Go through all hits of the PndTrkCand and check if their z values are increasing.
        // If not, remove the PndTrkCand from candlist
        Double_t lastz = -100.; // saves the z-position of the last hit
        for (size_t iSortedHit = 0; iSortedHit < tcand->GetNHits(); ++iSortedHit) {
          // TODO: This needs to be checked
          if (fVerbose > 11)
            Info("Exec", "Look at hit iSortedHit == %i", (int)iSortedHit);
          PndTrackCandHit candhit = tcand->GetSortedHit(iSortedHit);
          Int_t hitID = candhit.GetHitId();
          Int_t detID = candhit.GetDetId();
          if (fVerbose > 11) {
            cout << "PndFtsTrackerIdeal at cleaning loop\n";
            cout << "candhit = " << candhit << endl;
            cout << "hitID = " << hitID << endl;
            cout << "detID = " << detID << endl;
          }
          FairHit *hit = nullptr;
          // check if it really comes from the FTS (should always be true)
          if (detID == FairRootManager::Instance()->GetBranchId("FTSHit")) {
            hit = (PndFtsHit *)fHits[iDet]->At(hitID);
            Int_t tubeID = ((PndFtsHit *)hit)->GetTubeID();
            PndFtsTube *tube = (PndFtsTube *)fTubeArrayFts->At(tubeID);
            if (tube->IsSkew()) { // only check and count non-skewed hits
              if (hit->GetRefIndex() == -1) {
                Error("Exec", "Cleaning loop found a hit which was not caused by any MC truth track at iSortedHit == %i", (int)iSortedHit);
              } else {
                PndFtsPoint *pnt = (PndFtsPoint *)fMCPoints[0]->At(hit->GetRefIndex());
                // if(tube->IsSkew()) nofFtsSkewPoints++;
                // else nofFtsSkewPoints++;
                // TODO: count if I have enough non-skewed hits from layers 1+2 and 3-5

                // determine if z-coordinates are increasing
                if (pnt->GetZ() < lastz) {
                  if (fVerbose > 9) {
                    Info("Exec", "Mark candlist[%i] for deletion because the track turns around in the dipole field (its time-sorted hits have decreasing z values at some point).",
                         trackID);
                  }
                  // delete candlist[trackID] check if this works
                  keysToDeleteFromCandList.push_back(trackID);
                  break; // do not look at further hits from that track, look at next track
                } else {
                  lastz = pnt->GetZ();
                }
              }
            } // if IsSkew
          } else
            Error("Exec", "Cleaning loop found a hit from a detector other than FTS at iSortedHit == %i", (int)iSortedHit);

        } // sorted hit loop
        if (fVerbose > 3)
          Info("Exec", "Ended cleaning loop for candidate with trackID %i", trackID);
      } // for candidate cleaning loop

      // now delete all keys from candlist that have previously been saved in the vector keysToDeleteFromCandList
      for (size_t iKey = 0; iKey < keysToDeleteFromCandList.size(); ++iKey) {
        if (fVerbose > 10) {
          std::cout << "Delete key " << keysToDeleteFromCandList[iKey] << std::endl;
        }
        candlist.erase(keysToDeleteFromCandList[iKey]);
      }
      keysToDeleteFromCandList.clear();

      if (fVerbose > 10) {
        cout << "print the map keys AFTER cleaning\n";
        for (candit = candlist.begin(); candit != candlist.end(); ++candit) {
          cout << "trackID == " << candit->first << endl;
        }
      }

    } // if only FTS hits filled

  } // end loop over detectors
  // now we have track candidates
  if (fVerbose > 3)
    Info("Exec", "Insert to TCA (depending on efficiency)");

  // re-iterate and select by efficiency & number of hits
  std::map<Int_t, PndTrackCand *>::iterator candit;
  // FairTrackParP* firstPar=nullptr;
  // FairTrackParP* lastPar=nullptr;
  PndMCTrack *mc = nullptr;
  TVector3 svtx, smom;
  Int_t charge = 0, trackID = -1;

  for (candit = candlist.begin(); candit != candlist.end(); ++candit) {
    PndTrackCand *tcand = candit->second;
    trackID = candit->first;
    if (!tcand) {
      if (fVerbose > 3)
        Warning("Exec", "Have no candidate at %i", trackID);
      continue;
    }
    if (tcand->GetNHits() < 3)
      continue; // Here the total number of hits in the candidate are considered, not just the FTS hits. Note that this is obsolete in case fMinFtsHitsPerTrack is set to 3 or more
    if (0 < fEfficiency && fEfficiency < 1) {
      if (gRandom->Rndm() > fEfficiency)
        continue;
    }
    tcand->Sort();
    mc = (PndMCTrack *)fMCTracks->At(trackID);
    if (mc->GetPdgCode() < 100000000)
      charge = (Int_t)TMath::Sign(1.0, ((TParticlePDG *)pdg->GetParticle(mc->GetPdgCode()))->Charge());
    else
      charge = 1;
    tcand->setMcTrackId(trackID);
    // prepare track parameters
    firstPoint[trackID]->Position(svtx); // set position to first hit
    SmearFWD(svtx, fVtxSigma);
    firstPoint[trackID]->Momentum(smom);
    if (fRelative)
      fMomSigma.SetXYZ(fDPoP * smom.Mag(), fDPoP * smom.Mag(), fDPoP * smom.Mag());
    SmearFWD(smom, fMomSigma);
    FairTrackParP *firstPar = new FairTrackParP(svtx, smom, fVtxSigma, fMomSigma, charge, svtx, TVector3(1., 0., 0.), TVector3(0., 1., 0.));

    lastPoint[trackID]->Position(svtx);
    SmearFWD(svtx, fVtxSigma);
    lastPoint[trackID]->Momentum(smom);
    SmearFWD(smom, fMomSigma);
    FairTrackParP *lastPar = new FairTrackParP(svtx, smom, fVtxSigma, fMomSigma, charge, svtx, TVector3(1., 0., 0.), TVector3(0., 1., 0.));

    if (fVerbose > 3)
      Info("Exec", "Store candidate at %i", trackID);
    // Creates a new track in the TClonesArray.
    if (fVerbose > 3)
      Info("AddTrack", "Adding a Track.");
    TClonesArray &pndtracks = *fTracks;
    TClonesArray &pndtrackcands = *fTrackCands;
    //  TClonesArray &pndtrackids = *fTrackIds;
    Int_t size = pndtrackcands.GetEntriesFast();
    if (pndtracks.GetEntriesFast() != size) {
      Error("Exec", "Arrays out of synchronisation: %i tracks, %i cands. Abort event.", pndtracks.GetEntriesFast(), pndtrackcands.GetEntriesFast());
      return;
    }

    new (pndtrackcands[size]) PndTrackCand(*tcand);
    new (pndtracks[size]) PndTrack(*firstPar, *lastPar, *tcand, 0, 0, 1, mc->GetPdgCode(), trackID, FairRootManager::Instance()->GetBranchId("MCTrack"));
    // delete(tcand);
    delete (firstPar);
    delete (lastPar);
  }

  if (fVerbose > 3)
    Info("Exec", "End eventloop.");
}

//_________________________________________________________________
void PndFtsTrackerIdeal::Finish()
{
  std::cout << " Found  " << fTracks->GetEntriesFast() << " tracks\n";
}

//________________________________________________________________
void PndFtsTrackerIdeal::Reset()
{
  //---
  if (fTracks->GetEntriesFast() != 0)
    fTracks->Delete();
  if (fTrackCands->GetEntriesFast() != 0)
    fTrackCands->Delete();
}

//_________________________________________________________________
Int_t PndFtsTrackerIdeal::SetMinFtsHitsPerTrack(Int_t minFtsHitsPerTrack)
{
  // check that the argument is >= 1
  if (minFtsHitsPerTrack < 1) {
    minFtsHitsPerTrack = 1;
  }
  fMinFtsHitsPerTrack = minFtsHitsPerTrack;
  return fMinFtsHitsPerTrack;
}

//_________________________________________________________________
void PndFtsTrackerIdeal::SmearFWD(TVector3 &vec, const TVector3 &sigma)
{
  // gaussian smearing
  Double_t rannn = 0.;
  rannn = gRandom->Gaus(vec.X(), sigma.X());
  vec.SetX(rannn);

  rannn = gRandom->Gaus(vec.Y(), sigma.Y());
  vec.SetY(rannn);

  rannn = gRandom->Gaus(vec.Z(), sigma.Z());
  vec.SetZ(rannn);

  return;
}

ClassImp(PndFtsTrackerIdeal);
