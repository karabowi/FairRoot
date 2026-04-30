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
// -----                   PndSttFindTracks source file                -----
// -------------------------------------------------------------------------

#include "PndSttFindTracks.h"

#include "PndSttTrackFinder.h"
#include "PndTrackCand.h"
#include "PndTrack.h"
#include "PndSttHelixHit.h"
#include "PndSttTube.h"
#include "PndSttMapCreator.h"

#include "FairHit.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "TClonesArray.h"

#include <iostream>

using std::cout;
using std::endl;
using std::string;

// -----   Default constructor   -------------------------------------------
PndSttFindTracks::PndSttFindTracks() : PndPersistencyTask("STT Find Tracks")
{
  fFinder = nullptr;
  fTrackCandArray = nullptr;
  fHelixHitArray = nullptr;
  fNofTracks = 0;
  fVerbose = 1;
  fCollectionsComplete = kFALSE;
  SetPersistency(kTRUE);
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndSttFindTracks::PndSttFindTracks(PndSttTrackFinder *finder, Int_t verbose) : PndPersistencyTask("STT Find Tracks")
{
  fFinder = finder;
  fTrackCandArray = nullptr;
  fHelixHitArray = nullptr;
  fNofTracks = 0;
  fVerbose = verbose;
  fCollectionsComplete = kFALSE;
  SetPersistency(kTRUE);
  fHelixHitProduction = kFALSE;
}
// -------------------------------------------------------------------------

// -----   Constructor with name and title   -------------------------------
PndSttFindTracks::PndSttFindTracks(const char *name, const char *title, PndSttTrackFinder *finder, Int_t verbose) : PndPersistencyTask(name)
{
  fFinder = finder;
  fTrackCandArray = nullptr;
  fHelixHitArray = nullptr;
  fNofTracks = 0;
  fVerbose = verbose;
  fCollectionsComplete = kFALSE;
  SetPersistency(kTRUE);
  fHelixHitProduction = kFALSE;
  SetTitle(title);
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndSttFindTracks::~PndSttFindTracks()
{
  fTrackCandArray->Delete();
  fHelixHitArray->Delete();
  fHitCollectionNames.clear();
  fPointCollectionNames.clear();
}
// -------------------------------------------------------------------------

// -----   Public method Init (abstract in base class)  --------------------
InitStatus PndSttFindTracks::Init()
{
  fEventCounter = 0;

  // Check for Track finder
  if (!fFinder) {
    LOG(error) << " PndSttFindTracks::Init: No track finder selected!";
    return kERROR;
  }

  FairRootManager *ioman = FairRootManager::Instance();

  if (!ioman) {
    cout << "-E- PndSttFindTracks::AddHitCollection: "
         << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  fTrackCandArray = new TClonesArray("PndTrackCand", 100);
  ioman->Register("STTTrackCand", "STT", fTrackCandArray, GetPersistency());

  fTrackArray = new TClonesArray("PndTrack", 100);
  ioman->Register("STTFoundTrack", "STT", fTrackArray, GetPersistency());

  fHelixHitArray = new TClonesArray("PndSttHelixHit", 100);
  ioman->Register("STTPRHelixHit", "STT", fHelixHitArray, fHelixHitProduction);

  // Set verbosity of track finder
  fFinder->SetVerbose(fVerbose);

  // helix hits?
  fFinder->SetHelixHitProduction(fHelixHitProduction);

  // CHECK added
  PndSttMapCreator mapper(fSttParameters);
  fTubeArray = mapper.FillTubeArray();

  // Call the Init method of the track finder
  fFinder->Init();
  fFinder->SetTubeArray(fTubeArray);

  return kSUCCESS;
}

// CHECK added
void PndSttFindTracks::SetParContainers()
{
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar *)rtdb->getContainer("PndGeoSttPar");
}

// -------------------------------------------------------------------------
void PndSttFindTracks::AddHitCollectionName(char *hitCollectionName, char *pointCollectionName)
{
  //    string
  //	newPointName(pointCollectionName),
  //	newHitName(hitCollectionName);

  //    fHitCollectionNames.push_back(newHitName);
  //    fPointCollectionNames.push_back(newPointName);
  fHitCollectionNames.push_back(hitCollectionName);
  fPointCollectionNames.push_back(pointCollectionName);

  fFinder->SetInputBranchName(hitCollectionName); // this passes the HitBranch Name to
                                                  //  PndSttTrackFinderReal.
}

void PndSttFindTracks::AddHitCollection(char const *hitCollectionName, char const *pointCollectionName)
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

  fFinder->AddHitCollection(fHitArray, fPointArray);
}

void PndSttFindTracks::AddAllCollections()
{
  if (!fCollectionsComplete) {
    for (size_t counter = 0; counter < fHitCollectionNames.size(); counter++) {
      AddHitCollection(fHitCollectionNames[counter].c_str(), fPointCollectionNames[counter].c_str());
    }
    fCollectionsComplete = kTRUE;
  }
}

// -----   Public method Exec   --------------------------------------------
void PndSttFindTracks::Exec(Option_t *)
{
  // CHECK !!! IMPORTANT: event 0 (the first)

  if (fVerbose)
    cout << "******* event # " << fEventCounter << "*******" << endl;
  fEventCounter++;

  AddAllCollections();

  fTrackCandArray->Delete();
  fTrackArray->Delete();
  fHelixHitArray->Delete();

  fNofTracks = fFinder->DoFind(fTrackCandArray, fTrackArray, fHelixHitArray);

  for (Int_t iTrack = 0; iTrack < fTrackCandArray->GetEntriesFast(); iTrack++) {
    PndTrackCand *trackCand = (PndTrackCand *)fTrackCandArray->At(iTrack);
    trackCand->Sort();
  }
}
// -------------------------------------------------------------------------

// -----   Public method Finish   ------------------------------------------
void PndSttFindTracks::Finish()
{
  fFinder->Finish();
}
// -------------------------------------------------------------------------

ClassImp(PndSttFindTracks)
