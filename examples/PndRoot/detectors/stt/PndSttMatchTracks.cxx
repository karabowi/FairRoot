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
// -----                  PndSttMatchTracks source file                -----
// -------------------------------------------------------------------------

#include <iostream>

#include "TClonesArray.h"

#include "FairMCPoint.h"
#include "FairRootManager.h"
#include "FairLogger.h"
#include "PndSttMatchTracks.h"
#include "PndSttHit.h"
#include "PndSttTrackMatch.h"
#include "PndTrackCand.h"

#include "TMath.h"
#include <vector>
using namespace std;

// -----   Default constructor   -------------------------------------------
PndSttMatchTracks::PndSttMatchTracks() : PndPersistencyTask("STT track match")
{
  fMatches = nullptr;
  fVerbose = 1;
  fCollectionsComplete = kFALSE;
  SetPersistency(kTRUE);
}
// -------------------------------------------------------------------------

// -----   Constructor with verbosity level   ------------------------------
PndSttMatchTracks::PndSttMatchTracks(Int_t verbose) : PndPersistencyTask("STT track match")
{
  fMatches = nullptr;
  fVerbose = verbose;
  fCollectionsComplete = kFALSE;
  SetPersistency(kTRUE);
}
// -------------------------------------------------------------------------

// -----   Constructor with name, title and verbosity  ---------------------
PndSttMatchTracks::PndSttMatchTracks(const char *name, const char *title, Int_t verbose) : PndPersistencyTask(name)
{
  fMatches = nullptr;
  fVerbose = verbose;
  fCollectionsComplete = kFALSE;
  SetPersistency(kTRUE);
  SetTitle(title);
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndSttMatchTracks::~PndSttMatchTracks()
{
  fHitCollectionNames.clear();
  fPointCollectionNames.clear();
}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndSttMatchTracks::Init()
{

  // Get FairRootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndSttMatchTracks::Init: "
         << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  // Get TrackCand Array
  fTrackCandidates = (TClonesArray *)ioman->GetObject("STTTrackCand");
  if (!fTrackCandidates) {
    LOG(error) << " PndSttMatchTracks::Init: No STTTrackCand array!";
    return kERROR;
  }

  // Create and register SttTrackMatch array
  fMatches = new TClonesArray("PndSttTrackMatch", 100);
  ioman->Register("STTTrackMatch", "STT", fMatches, GetPersistency());

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndSttMatchTracks::Exec(Option_t *)
{
  AddAllCollections();

  if (fHitCollectionList.GetEntries() == 0) {
    cout << "-E- PndSttTrackFinderIdeal::DoFind: "
         << "No hit arrays present, call AddHitCollection() first (at least once)! " << endl;
  }

  if (fPointCollectionList.GetEntries() == 0) {
    cout << "-E- PndSttTrackFinderIdeal::DoFind: "
         << "No point arrays present, call AddHitCollection() first (at least once)! " << endl;
  }

  // Clear output array
  fMatches->Delete();

  // Create some pointers and variables
  PndTrackCand *trackCand = nullptr;
  PndSttHit *mHit = nullptr;
  FairMCPoint *point = nullptr;

  Int_t nHits = 0;
  Int_t nMCTracks = 0;
  Int_t iPoint = 0;
  // Int_t iFlag     = 0; //[R.K. 01/2017] unused variable?
  Int_t iMCTrack = 0;
  Int_t nAll = 0;
  Int_t nTrue = 0;
  Int_t nWrong = 0;
  Int_t nFake = 0;
  Int_t nHitSum = 0;
  Int_t nTrueSum = 0;
  Int_t nWrongSum = 0;
  Int_t nFakeSum = 0;
  Int_t nMCTrackSum = 0;

  // Loop over TracksCand
  Int_t nTracks = fTrackCandidates->GetEntriesFast();

  for (Int_t iTrack = 0; iTrack < nTracks; iTrack++) {
    trackCand = (PndTrackCand *)fTrackCandidates->At(iTrack);

    if (!trackCand) {
      LOG(warn) << " PndSttMatchTracks::Exec: Empty STTTrackCand at " << iTrack;
      continue;
    }

    nHits = trackCand->GetNHits();

    nAll = nTrue = nWrong = nFake = nMCTracks = 0;

    fMatchMap.clear();
    if (fVerbose > 2)
      cout << endl << "Track " << iTrack << ", Hits " << nHits << endl;

    // Loop over Hits of track
    for (Int_t iMHit = 0; iMHit < nHits; iMHit++) {

      PndTrackCandHit candhit = trackCand->GetSortedHit(iMHit);

      // alter here
      mHit = GetHitFromCollections(candhit.GetHitId());

      if (!mHit) {
        cout << "-E- PndSttMatchTracks::Exec: "
             << "No Hit " << iMHit << " for track " << iTrack << endl;
        continue;
      }

      iPoint = mHit->GetRefIndex();

      if (iPoint < 0) {
        nFake++;
        continue;
      }

      // alter here
      point = GetPointFromCollections(candhit.GetHitId());

      if (!point) {
        cout << "-E- PndSttMatchTracks::Exec: "
             << "Empty MCPoint " << iPoint << " from Hit " << iMHit << " (track " << iTrack << ")" << endl;
        continue;
      }

      iMCTrack = point->GetTrackID();

      if (fVerbose > 2)
        cout << "Track " << iTrack << ", hit " << candhit.GetHitId() << ", STTPoint " << iPoint << ", MCTrack " << iMCTrack << endl;
      fMatchMap[iMCTrack]++;
    }

    // Search for best matching MCTrack
    iMCTrack = -1;

    map<Int_t, Int_t>::iterator it;
    for (it = fMatchMap.begin(); it != fMatchMap.end(); it++) {
      if (fVerbose > 2)
        cout << it->second << " common points wth MCtrack " << it->first << endl;
      nMCTracks++;
      nAll += it->second;
      if (it->second > nTrue) {
        iMCTrack = it->first;
        nTrue = it->second;
      }
    }

    nWrong = nAll - nTrue;
    if (fVerbose > 1)
      cout << "-I- PndSttMatchTracks: STTTrack " << iTrack << ", MCTrack " << iMCTrack << ", true " << nTrue << ", wrong " << nWrong << ", fake " << nFake << ", #MCTracks "
           << nMCTracks << endl;

    // Create SttTrackMatch
    new ((*fMatches)[iTrack]) PndSttTrackMatch(iMCTrack, nTrue, nWrong, nFake, nMCTracks);

    // Some statistics
    nHitSum += nHits;
    nTrueSum += nTrue;
    nWrongSum += nWrong;
    nFakeSum += nFake;
    nMCTrackSum += nMCTracks;

  } // Track loop

  // Event statistics

  Double_t qTrue = 0.;
  if (nHitSum)
    qTrue = Double_t(nTrueSum) / Double_t(nHitSum) * 100.;

  if (fVerbose == 3) {
    Double_t qWrong = 0., qFake = 0., qMC = 0.;

    if (nHitSum) {
      qWrong = Double_t(nWrongSum) / Double_t(nHitSum) * 100.;
      qFake = Double_t(nFakeSum) / Double_t(nHitSum) * 100.;
    }
    if (nTracks)
      qMC = Double_t(nMCTrackSum) / Double_t(nTracks);

    cout << endl;
    cout << "-------------------------------------------------------" << endl;
    LOG(info) << "              STT Track Matching                 -I-";
    cout << "Reconstructed STTTracks : " << nTracks << endl;
    ;
    cout << "True  hit assignments   : " << qTrue << " %" << endl;
    cout << "Wrong hit assignments   : " << qWrong << " %" << endl;
    cout << "Fake  hit assignments   : " << qFake << " %" << endl;
    cout << "MCTracks per STTTrack   : " << qMC << endl;
    cout << "--------------------------------------------------------" << endl;
  } else if (fVerbose)
    LOG(info) << " PndSttMatchTracks: rec. " << nTracks << ", quota " << qTrue << " % ";
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void PndSttMatchTracks::AddHitCollectionName(char *hitCollectionName, char *pointCollectionName)
{
  string newPointName(pointCollectionName), newHitName(hitCollectionName);

  fHitCollectionNames.push_back(newHitName);
  fPointCollectionNames.push_back(newPointName);
}

void PndSttMatchTracks::AddHitCollection(char const *hitCollectionName, char const *pointCollectionName)
{
  // Get and check FairRootManager
  FairRootManager *ioman = FairRootManager::Instance();

  if (!ioman) {
    cout << "-E- PndSttFindTracks::AddHitCollection: "
         << "RootManager not instantised!" << endl;
  }

  // Get hit Array
  TClonesArray *fHitArray = (TClonesArray *)ioman->GetObject(hitCollectionName);

  if (!fHitArray) {
    LOG(warn) << " PndSttFindTracks::AddHitCollection: No " << hitCollectionName << " STT hit array!";
  }

  // Get point Array
  TClonesArray *fPointArray = (TClonesArray *)ioman->GetObject(pointCollectionName);

  if (!fPointArray) {
    LOG(warn) << " PndSttFindTracks::AddHitCollection: No " << pointCollectionName << " STT hit array!";
  }

  fHitCollectionList.Add(fHitArray);
  fPointCollectionList.Add(fPointArray);
}

void PndSttMatchTracks::AddAllCollections()
{
  if (!fCollectionsComplete) {
    for (size_t counter = 0; counter < fHitCollectionNames.size(); counter++) {
      AddHitCollection(fHitCollectionNames[counter].c_str(), fPointCollectionNames[counter].c_str());
    }
    fCollectionsComplete = kTRUE;
  }
}

PndSttHit *PndSttMatchTracks::GetHitFromCollections(Int_t hitCounter)
{
  PndSttHit *retval = nullptr;

  Int_t relativeCounter = hitCounter;

  for (Int_t collectionCounter = 0; collectionCounter < fHitCollectionList.GetEntries(); collectionCounter++) {
    Int_t size = ((TClonesArray *)fHitCollectionList.At(collectionCounter))->GetEntriesFast();

    if (relativeCounter < size) {
      retval = (PndSttHit *)((TClonesArray *)fHitCollectionList.At(collectionCounter))->At(relativeCounter);
      break;
    } else {
      relativeCounter -= size;
    }
  }
  return retval;
}

FairMCPoint *PndSttMatchTracks::GetPointFromCollections(Int_t hitCounter)
{
  FairMCPoint *retval = nullptr;

  Int_t relativeCounter = hitCounter;

  for (Int_t collectionCounter = 0; collectionCounter < fHitCollectionList.GetEntries(); collectionCounter++) {
    Int_t size = ((TClonesArray *)fHitCollectionList.At(collectionCounter))->GetEntriesFast();

    if (relativeCounter < size) {
      Int_t tmpHit = ((PndSttHit *)((TClonesArray *)fHitCollectionList.At(collectionCounter))->At(relativeCounter))->GetRefIndex();

      retval = (FairMCPoint *)((TClonesArray *)fPointCollectionList.At(collectionCounter))->At(tmpHit);

      break;
    } else {
      relativeCounter -= size;
    }
  }
  return retval;
}

// -----   Public method Finish   ------------------------------------------
void PndSttMatchTracks::Finish() {}
// -------------------------------------------------------------------------

ClassImp(PndSttMatchTracks)
