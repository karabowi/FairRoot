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

/////////////////////////////////////////////////////////////
//  PndPreselectSttHitsTask
//  Finds Track
/////////////////////////////////////////////////////////////////

/** PndPreselectSttHitsTask
 *@author Anna Alicke <a.alicke@fz-juelich.de>
 *@since 14.09.2021
 *@version 1.0
 **
 ** PANDA task class for finding tracks based on the triplet finder and the apollonius calculation
 ** Task level RECO
 **/

// Includes from base
#include "FairRunAna.h"
#include <FairField.h>
#include "FairLogger.h"
#include <FairRuntimeDb.h>
// Pnd includes
#include "PndTrackCand.h"
#include "PndPreselectSttHitsTask.h"
#include "PndSegmentationPreselectSttHits.h"

#include "PndSttMapCreator.h"

// -----   Constructor   -------------------------------------------
PndPreselectSttHitsTask::PndPreselectSttHitsTask() : FairTask("ApolloniusTripletTrackFinderTask") {}

// -----   Destructor   ----------------------------------------------------
PndPreselectSttHitsTask::~PndPreselectSttHitsTask() {}
// -----   Private method SetParContainers   -------------------------------

void PndPreselectSttHitsTask::SetParContainers()
{
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar *)rtdb->getContainer("PndGeoSttPar");
}

InitStatus PndPreselectSttHitsTask::Init()
{
  fTrackCandArray = FairRootManager::Instance()->Register("PreselectedSttHits", "PndTrackCand", "tracking", kTRUE);


  //PndSttMapCreator mapper(fSttParameters);
  //fTubeArray = mapper.FillTubeArray();

  if (fPreselector == nullptr) {
    PndSegmentationPreselectSttHits *segPre = new PndSegmentationPreselectSttHits();

    if (std::count(fAdditionalBranchNames.begin(), fAdditionalBranchNames.end(), "STTCombinedSkewedHits") > 0) {
      segPre->SetCombinedSkewed(dynamic_cast<TClonesArray *>(FairRootManager::Instance()->GetObject("STTCombinedSkewedHits")));
    } else {
      std::cout << "STTCombinedSkewedHits not Found!";
      return kFATAL;
    }
    fPreselector = segPre;
  }

  fSTTHits = dynamic_cast<TClonesArray *>(FairRootManager::Instance()->GetObject("STTHit"));
  for (size_t i = 0; i < fAdditionalBranchNames.size(); i++) {
    if (FairRootManager::Instance()->GetObject(fAdditionalBranchNames[i]) != 0) {
      TString name = fAdditionalBranchNames[i];
      TClonesArray *array = dynamic_cast<TClonesArray *>(FairRootManager::Instance()->GetObject(fAdditionalBranchNames[i]));
      fPreselector->AddAdditionalBranches(name, array);
    }
  }

  PndStt2GeoHandler *geoH = PndStt2GeoHandler::Instance(fSttParameters);
  //PndStt2GeoHandler *geoH = new PndStt2GeoHandler(fSttParameters);
  fPreselector->Init(fSTTHits, geoH);

  return kSUCCESS;
}

void PndPreselectSttHitsTask::Exec(Option_t *)
{
  for (int i = 0; i < fSTTHits->GetEntries(); i++) {
    PndSttHit *hit = static_cast<PndSttHit *>(fSTTHits->At(i));
    hit->SetEntryNr(FairLink(-1, FairRootManager::Instance()->GetEntryNr(), FairRootManager::Instance()->GetBranchId("STTHit"), i));
  }

  std::vector<std::vector<PndSttHit *>> hitSolutions = fPreselector->PreselectSttHits();
  std::vector<PndTrackCand> solutions = fPreselector->GetPndTrackCands(hitSolutions);

  fTrackCandArray->Delete();
  for (auto solution : solutions) {
    new ((*fTrackCandArray)[fTrackCandArray->GetEntries()]) PndTrackCand(solution);
  }
}

void PndPreselectSttHitsTask::Finish()
{
  fTrackCandArray->Delete();
}

ClassImp(PndPreselectSttHitsTask);
