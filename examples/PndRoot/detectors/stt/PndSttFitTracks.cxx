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
// -----                    PndSttFitTracks source file                -----
// -------------------------------------------------------------------------

#include "PndSttFitTracks.h"

#include "PndSttTrackFitter.h"
#include "PndSttTrack.h"
#include "PndSttTube.h"
#include "PndSttHit.h"
#include "PndSttMapCreator.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLink.h"
#include "FairLogger.h"

#include "PndDetectorList.h"

#include "TClonesArray.h"

#include <iostream>

using std::cout;
using std::endl;
using std::string;

// -----   Default constructor   -------------------------------------------
PndSttFitTracks::PndSttFitTracks()
{
  fFitter = nullptr;
  fNofTracks = 0;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndSttFitTracks::PndSttFitTracks(const char *name, const char *title, PndSttTrackFitter *fitter) : FairTask(name)
{
  fFitter = fitter;
  fNofTracks = 0;
  fCollectionsComplete = kFALSE;
  SetTitle(title);
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndSttFitTracks::~PndSttFitTracks()
{
  fHitCollectionNames.clear();
}
// -------------------------------------------------------------------------

// -----   Public method Init (abstract in base class)  --------------------
InitStatus PndSttFitTracks::Init()
{
  // Check for Track fitter
  if (!fFitter) {
    LOG(error) << " PndSttFitTracks: No track fitter selected!";
    return kERROR;
  }

  // Get and check FairRootManager
  FairRootManager *ioman = FairRootManager::Instance();

  if (!ioman) {
    cout << "-E- PndSttFitTracks::Init: "
         << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  // Get SttTrack array CHECK add
  fTrackCandArray = (TClonesArray *)ioman->GetObject("STTTrackCand"); //=>SG
  if (!fTrackCandArray) {
    LOG(error) << " PndSttFitTracks::Init: No SttTrackCand array!";
    return kERROR;
  }

  // Create and register SttTrack array
  fTrackArray = new TClonesArray("PndSttTrack", 100);
  ioman->Register("STTTrack", "STT", fTrackArray, kTRUE); // fPersistence); // CHECK

  // CHECK added
  PndSttMapCreator mapper(fSttParameters);
  fTubeArray = mapper.FillTubeArray();

  // Call the Init method of the track fitter
  fFitter->Init();
  fFitter->SetTubeArray(fTubeArray);

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// CHECK added
void PndSttFitTracks::SetParContainers()
{
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar *)rtdb->getContainer("PndGeoSttPar");
}

// -----   Public method Exec   --------------------------------------------
void PndSttFitTracks::Exec(Option_t *)
{
  AddAllCollections();

  if (!fTrackCandArray)
    return; // =>SG
  fTrackArray->Clear();

  Int_t nTracks = fTrackCandArray->GetEntriesFast();

  for (Int_t iTrack = 0; iTrack < nTracks; iTrack++) {
    PndTrackCand *pTrackCand = (PndTrackCand *)fTrackCandArray->At(iTrack);
    if (!pTrackCand)
      continue;

    Int_t size = fTrackArray->GetEntriesFast();
    new ((*fTrackArray)[size]) PndSttTrack();
    PndSttTrack *pTrack = (PndSttTrack *)fTrackArray->At(size);
    pTrack->SetTrackCandIndex(iTrack);
    pTrack->SetLink(FairLink("SttTrackCand", iTrack));

    fFitter->DoFit(pTrackCand, pTrack);
  }

  // reset hit coordinates
  for (int i = 0; i < fHitArray->GetEntriesFast(); i++) {
    PndSttHit *currenthit = (PndSttHit *)fHitArray->At(i);
    if (!currenthit)
      continue;
    // tubeID  CHECK added
    Int_t tubeID = currenthit->GetTubeID();
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);
    currenthit->SetX(tube->GetPosition().X());
    currenthit->SetY(tube->GetPosition().Y());
    currenthit->SetZ(tube->GetPosition().Z());
  }
}
// -------------------------------------------------------------------------

// -----   Public method Finish   ------------------------------------------
void PndSttFitTracks::Finish() {}
// -------------------------------------------------------------------------

void PndSttFitTracks::AddHitCollectionName(char *hitCollectionName)
{
  string newHitName(hitCollectionName);

  fHitCollectionNames.push_back(newHitName);
}

void PndSttFitTracks::AddHitCollection(char const *hitCollectionName)
{
  // Get and check FairRootManager
  FairRootManager *ioman = FairRootManager::Instance();

  if (!ioman) {
    cout << "-E- PndSttFindTracks::AddHitCollection: "
         << "RootManager not instantised!" << endl;
  }

  // Get hit Array
  fHitArray = (TClonesArray *)ioman->GetObject(hitCollectionName);

  if (!fHitArray) {
    LOG(warn) << " PndSttFindTracks::AddHitCollection: No " << hitCollectionName << " STT hit array!";
  }

  fFitter->AddHitCollection(fHitArray);
}

void PndSttFitTracks::AddAllCollections()
{
  if (!fCollectionsComplete) {
    for (size_t counter = 0; counter < fHitCollectionNames.size(); counter++) {
      AddHitCollection(fHitCollectionNames[counter].c_str());
    }
    fCollectionsComplete = kTRUE;
  }
}

ClassImp(PndSttFitTracks)
