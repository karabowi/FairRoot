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
 * PatternMatcher.cxx
 *
 *  Created on: Jun 16, 2017
 *      Author: Michael Papenbrock
 */

#include "PndPatternMatcher.h"

#include <FairRunAna.h>
#include <FairRuntimeDb.h>
#include <FairLogger.h>
#include <PndSttHit.h>

#include "PndPattern.h"
#include <TTreeReaderValue.h>
#include <algorithm>

ClassImp(PndPatternMatcher)

  PndPatternMatcher::PndPatternMatcher()
  : fPartialMatchCand(0)
{
  dbFile = nullptr;
  patternTree = nullptr;
  //  treeReader = nullptr;
  fSttParameters = nullptr;
  fEventHeader = nullptr;
  fSTTHitArray = nullptr;

  fTrackCandName = "PartialMatchCand";
  fPersistence = kTRUE;

  verboseMatches = false;
  fMatchRatio = 0.5;
}

PndPatternMatcher::~PndPatternMatcher() {}

void PndPatternMatcher::SetParContainers()
{
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar *)rtdb->getContainer("PndGeoSttPar");
}
InitStatus PndPatternMatcher::Init()
{
  LoadPatternDB("patternDB.root");

  // Get the running instance of the FairRootManager to access tree branches
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    LOG(error) << " PatternDBGenerator::Init: FairRootManager not instantiated!";
    return kFATAL;
  }

  fEventHeader = (TClonesArray *)ioman->GetObject("EventHeader.");
  if (!fEventHeader) {
    LOG(error) << " PatternDBGenerator:Init: EventHeader not instantiated!";
  }

  // Access the STTHit branch
  fSTTHitArray = (TClonesArray *)ioman->GetObject("STTHit");

  //  Set the output branch
  fPartialMatchCand = ioman->Register(fTrackCandName, "PndTrackCand", "STT", fPersistence);

  return kSUCCESS;
}
void PndPatternMatcher::Exec(Option_t *)
{
  fPartialCand.clear();
  fPartialMatchCand->Delete();
  FindMatch();

  //  Write trackCands into the data stream
  for (auto const &trackCand : fPartialCand) {
    new ((*fPartialMatchCand)[fPartialMatchCand->GetEntriesFast()]) PndTrackCand(trackCand);
  }
}
void PndPatternMatcher::FinishTask() {}

void PndPatternMatcher::FindMatch()
{
  //  Get a list of all tubeIDs in the event
  int nSttHits = fSTTHitArray->GetEntriesFast();
  std::set<int> tubeIDs;
  for (int iHit = 0; iHit < nSttHits; ++iHit) {
    PndSttHit *sttHit = (PndSttHit *)fSTTHitArray->At(iHit);
    int tubeID = sttHit->GetTubeID();
    tubeIDs.insert(tubeID);
  }

  //  std::cout << dbTubeIDs.at(1).size() << std::endl;

  int matches = 0;
  //  Compare each pattern in the data base with the current set of tubeIDs
  for (auto const &dbPattern : dbTubeIDs) {
    std::set<int> intersection;
    intersection.clear();
    //    Determine the intersection between the current database pattern and the event's set of tubeIDs
    std::set_intersection(tubeIDs.begin(), tubeIDs.end(), dbPattern.begin(), dbPattern.end(), std::inserter(intersection, intersection.begin()));
    //    Determine the ratio to which there is an overlap with the database pattern
    float matchRatio = (float)intersection.size() / (float)dbPattern.size();
    //    If the matching ratio is large enough, proceed to create trackCand from intersection
    if (matchRatio > fMatchRatio) {
      //      std::cout<< "match ratio: " << matchRatio << std::endl;
      matches++;

      //      Detailed output in case it is needed (set verboseMatches to true)
      if (verboseMatches) {
        if (intersection.size() > 0) {
          std::cout << "matching tubes: ";
          for (auto const &tube : intersection) {
            std::cout << tube << " ";
          }
          std::cout << std::endl;
        }
      }

      CreateTrackCandFromMatch(intersection);
    }
  }
  if (verboseMatches)
    std::cout << "partial matches found: " << matches << std::endl;
}
void PndPatternMatcher::CreateTrackCandFromMatch(std::set<int> partialMatch)
{
  //  Create a PndTrackCand from the partial match found in FindMatch()
  PndTrackCand trackCand;
  int nSttHits = fSTTHitArray->GetEntriesFast();
  //  Loop over all STT hits
  for (int iHit = 0; iHit < nSttHits; ++iHit) {
    PndSttHit *sttHit = (PndSttHit *)fSTTHitArray->At(iHit);
    int tubeID = sttHit->GetTubeID();
    //    verify if this hit is part of the partial match
    auto search = partialMatch.find(tubeID);
    //    if hit is found in partial match, add it to track cand
    if (search != partialMatch.end()) {
      trackCand.AddHit(sttHit->GetEntryNr(), iHit);
    }
  }
  //  Add new trackCand to vector of all trackCands for this event
  fPartialCand.push_back(trackCand);
}
void PndPatternMatcher::LoadPatternDB(TString filename)
{
  dbFile = new TFile(filename, "READ");
  patternTree = (TTree *)dbFile->Get("trackPatterns");

  dbTubeIDs.clear();

  PndPattern *pattern;
  patternTree->SetBranchAddress("pattern", &pattern);

  Long64_t nPatterns = patternTree->GetEntriesFast();
  for (Long64_t iEntry = 0; iEntry < nPatterns; ++iEntry) {
    patternTree->GetEntry(iEntry);
    std::set<int> tubeIDs = pattern->GetTubeIDs();
    dbTubeIDs.push_back(tubeIDs);
  }
  std::cout << "# DB patterns: " << dbTubeIDs.size() << std::endl;
}
