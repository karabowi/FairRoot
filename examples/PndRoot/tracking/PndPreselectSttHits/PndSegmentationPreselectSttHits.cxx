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

/*
 * PndSegmentationPreselectSttHits.cxx
 *
 *  Created on: 14.09.2021
 *      Author: aalicke
 */

#include "PndSegmentationPreselectSttHits.h"
#include "PndStt2GeoHandler.h"
#include "TH1F.h"
#include "PndSttSkewedHit.h"
#include "FairRootManager.h"
#include "FairLogger.h"

void PndSegmentationPreselectSttHits::Init(TClonesArray *sttHits, PndStt2GeoHandler *geoH)
//void PndSegmentationPreselectSttHits::Init(TClonesArray *sttHits, TClonesArray *tubeArray)
{
  fSttGeoH = geoH;
  SetSttHitsArray(sttHits);
  //fStrawMap = new PndSttStrawMap(tubeArray);
  if (fBranchMap.count("STTCombinedSkewedHits") > 0)
    fCombinedSkewedHitArray = fBranchMap["STTCombinedSkewedHits"];
}

std::vector<std::vector<PndSttHit *>> PndSegmentationPreselectSttHits::PreselectSttHits()
{
  // create a histogram with all phi values
  double gab = 1. / 30. * TMath::Pi(); // 6 deg

  std::vector<std::pair<FairHit *, double>> hits;
  hits.clear();

  for (int i = 0; i < fSttHits->GetEntriesFast(); i++) {
    PndSttHit *hit = (PndSttHit *)fSttHits->At(i);
    if (fSttGeoH->IsSkewedStraw(hit->GetTubeID()))
    //if (fStrawMap->IsSkewedStraw(hit->GetTubeID()))
      continue;
    hits.push_back(std::make_pair((FairHit *)hit, TMath::ATan2(hit->GetY(), hit->GetX())));
  }
  if (fCombinedSkewedHitArray != nullptr) {
    for (int i = 0; i < fCombinedSkewedHitArray->GetEntriesFast(); i++) {
      PndSttSkewedHit *hit = (PndSttSkewedHit *)fCombinedSkewedHitArray->At(i);
      hits.push_back(std::make_pair((FairHit *)hit, TMath::ATan2(hit->GetY(), hit->GetX())));
    }
  }

  // sort the hits by phi
  std::sort(hits.begin(), hits.end(), [](auto &left, auto &right) { return left.second < right.second; });

  // fill hits in groups a cut is made by a gab of min 6 deg
  vector<vector<PndSttHit *>> AllSegments;
  vector<PndSttHit *> Segment;
  if (hits.size() < 2)
    return AllSegments;

  for (int i = 0; i < hits.size() - 1; i++) {
    if (FairRootManager::Instance()->GetBranchName(((hits[i].first)->GetEntryNr()).GetType()) != "STTCombinedSkewedHits")
      Segment.push_back((PndSttHit *)hits[i].first);
    else {
      PndSttSkewedHit *SkewedSttHit = (PndSttSkewedHit *)hits[i].first;
      std::pair<Int_t, Int_t> tubeIDs = SkewedSttHit->GetTubeIDs();
      for (int j = 0; j < fSttHits->GetEntriesFast(); j++) {
        PndSttHit *sttHit = (PndSttHit *)fSttHits->At(j);
        if ((sttHit->GetTubeID() == tubeIDs.first || sttHit->GetTubeID() == tubeIDs.second))
          Segment.push_back(sttHit);
      }
    }
    if (hits[i + 1].second - hits[i].second >= gab) {
      AllSegments.push_back(Segment);
      Segment.clear();
    }
  }
  if (FairRootManager::Instance()->GetBranchName(((hits[hits.size() - 1].first)->GetEntryNr()).GetType()) != "STTCombinedSkewedHits")
    Segment.push_back((PndSttHit *)hits[hits.size() - 1].first);
  else {
    PndSttSkewedHit *SkewedSttHit = (PndSttSkewedHit *)hits[hits.size() - 1].first;
    std::pair<Int_t, Int_t> tubeIDs = SkewedSttHit->GetTubeIDs();
    for (int j = 0; j < fSttHits->GetEntriesFast(); j++) {
      PndSttHit *sttHit = (PndSttHit *)fSttHits->At(j);
      if ((sttHit->GetTubeID() == tubeIDs.first || sttHit->GetTubeID() == tubeIDs.second))
        Segment.push_back(sttHit);
    }
  }
  AllSegments.push_back(Segment);

  if (hits[0].second + TMath::Pi() < gab && TMath::Pi() - hits[hits.size() - 1].second < gab) {
    AllSegments[0].insert(AllSegments[0].end(), AllSegments[AllSegments.size() - 1].begin(), AllSegments[AllSegments.size() - 1].end());
    AllSegments.resize(AllSegments.size() - 1);
  }
  LOG(debug) << "number of preselected groups: " << AllSegments.size();
  return AllSegments;
}
