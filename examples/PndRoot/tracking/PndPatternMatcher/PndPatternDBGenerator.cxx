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
 * PndPatternDBGenerator.cxx
 *
 *  Created on: Nov 8, 2017
 *      Author: Michael Papenbrock
 */

#include <FairRootManager.h>
#include <FairRunAna.h>
#include <FairRuntimeDb.h>
#include <FairLogger.h>
#include <PndSttMapCreator.h>
#include <PndSttTube.h>
#include <PndTrackCand.h>
#include <TClonesArray.h>
#include <TFile.h>
#include "PndPatternDBGenerator.h"

ClassImp(PndPatternDBGenerator)

  PndPatternDBGenerator::PndPatternDBGenerator()
{
  fSttParameters = nullptr;
  fEventHeader = nullptr;
  fTubeArray = nullptr;
  fSttHitArray = nullptr;
  fMCTrackArray = nullptr;
  trackCands = nullptr;
  sttBranchID = -1;
  mcTrackID = -1;

  foutputFile = nullptr;
  fsectorPatternTree = nullptr;
  ftrackPatternTree = nullptr;
  bPattern = nullptr;
  foutputFilename = "patternDB.root";

  nTotalTracks = 0;
  nMultipleMCTrackLinks = 0;
}

PndPatternDBGenerator::~PndPatternDBGenerator() {}

void PndPatternDBGenerator::SetParContainers()
{
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar *)rtdb->getContainer("PndGeoSttPar");
}
InitStatus PndPatternDBGenerator::Init()
{
  std::cout << "Filename for pattern DB: " << foutputFilename << std::endl;
  foutputFile = new TFile(foutputFilename, "RECREATE");
  fsectorPatternTree = new TTree("sectorPatterns", "Tree containing sector patterns");
  fsectorPatternTree->Branch("pattern", &bPattern);
  ftrackPatternTree = new TTree("trackPatterns", "Tree containing track patterns");
  ftrackPatternTree->Branch("pattern", &bPattern);

  //  Get instance of the FairRootManager to access tree branches
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    LOG(error) << " PndPatternDBGenerator::Init: FairRootManager not instantiated!";
    return kFATAL;
  }

  //  Get the EventHeader
  fEventHeader = (TClonesArray *)ioman->GetObject("EventHeader.");
  if (!fEventHeader) {
    LOG(error) << " PndPatternDBGenerator::Init: EventHeader not loaded!";
    return kFATAL;
  }

  //  Access STTHit branch and determine its branch ID
  fSttHitArray = (TClonesArray *)ioman->GetObject("STTHit");
  sttBranchID = ioman->GetBranchId("STTHit");

  //  Initialise STT map and get array of all tubes
  PndSttMapCreator *mapper = new PndSttMapCreator(fSttParameters);
  fTubeArray = mapper->FillTubeArray();

  //  Access MCTrack branch and determine its branch ID
  fMCTrackArray = (TClonesArray *)ioman->GetObject("MCTrack");
  mcTrackID = ioman->GetBranchId("MCTrack");

  //  Get track candidates from ideal track finder
  trackCands = (TClonesArray *)ioman->GetObject("SttMvdGemIdealTrackCand");

  LOG(info) << " PndPatternDBGenerator: Initialisation successful";
  return kSUCCESS;
}
void PndPatternDBGenerator::Exec(Option_t * /*opt*/)
{ //[R.K. 9/2018] unused
  GenerateTrackPatterns();
}
void PndPatternDBGenerator::FinishTask()
{
  fsectorPatternTree->AutoSave();
  ftrackPatternTree->AutoSave();
  foutputFile->Close();

  std::cout << "Total number of tracks found: " << nTotalTracks << std::endl;
  std::cout << "Number of track with multiple MCTrack links: " << nMultipleMCTrackLinks << std::endl;
  float ratio = 100. * (float)nMultipleMCTrackLinks / (float)nTotalTracks;
  std::cout << "Ratio: " << ratio << "%" << std::endl;
}

void PndPatternDBGenerator::GenerateTrackPatterns()
{
  //  This method generates hit patterns for each track

  //  Get FairRootManager instance to access objects through FairLinks
  FairRootManager *ioman = FairRootManager::Instance();

  //  Loop over all track candidates
  short nTrackCands = trackCands->GetEntriesFast();
  for (int iTrackCand = 0; iTrackCand < nTrackCands; ++iTrackCand) {
    //    Get the next track candidate
    PndTrackCand *cand = (PndTrackCand *)trackCands->At(iTrackCand);
    //    Get the FairLinks from the candidate pointing at STTHit branch
    FairMultiLinkedData sttLinks = cand->GetLinksWithType(sttBranchID);
    //    Construct sttHitArray for each track candidate,
    //    taking into account all STTHits of the corresponding track
    HitArray sttHitArray;
    for (int iLink = 0; iLink < sttLinks.GetNLinks(); ++iLink) {
      FairLink sttLink = sttLinks.GetLink(iLink);
      PndSttHit *sttHit = (PndSttHit *)ioman->GetCloneOfLinkData(sttLink);
      sttHitArray.push_back(sttHit);
    }
    //    Get link to corresponding MC track
    FairMultiLinkedData mcTrackLinks = cand->GetLinksWithType(mcTrackID);
    //    Check if only one MC track was found
    nTotalTracks++;
    if (mcTrackLinks.GetNLinks() == 0) {
      std::cout << "WARNING: PndPatternDBGenerator::GenerateTrackPatterns: No MCTrackfound, skipping event!" << std::endl;
      continue;
    }
    if (mcTrackLinks.GetNLinks() > 1) {
      nMultipleMCTrackLinks++;
      std::cout << "WARNING: PndPatternDBGenerator::GenerateTrackPatterns: Found more than one MCTrack link!!!" << std::endl;
    }
    //    Get only first link since only one should be associated with track candidate
    FairLink mcTrackLink = mcTrackLinks.GetLink(0);
    PndMCTrack *mcTrack = (PndMCTrack *)ioman->GetCloneOfLinkData(mcTrackLink);

    PndPatterns sectorPatterns = FillSectorPatterns(sttHitArray, mcTrack);
    AddPatternsToTree(sectorPatterns, fsectorPatternTree);
    PndPatterns trackPatterns = FillTrackPatterns(sttHitArray, mcTrack);
    AddPatternsToTree(trackPatterns, ftrackPatternTree);
  }
}
HitSectorMap PndPatternDBGenerator::FillSectorHitMap(HitArray sttHitArray)
{
  HitSectorMap hitMap;
  for (auto const &sttHit : sttHitArray) {
    short tubeID = sttHit->GetTubeID();
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);
    short sectorID = tube->GetSectorID();
    hitMap.insert(std::make_pair(sectorID, tubeID));
  }
  return hitMap;
}
PndPatterns PndPatternDBGenerator::FillSectorPatterns(HitArray sttHitArray, PndMCTrack *mcTrack)
{
  PndPatterns patterns;
  HitSectorMap hitMap = FillSectorHitMap(sttHitArray);
  TVector3 momentum = mcTrack->GetMomentum();
  for (int iSector = 0; iSector < 6; ++iSector) {
    auto hitMapSectorRange = hitMap.equal_range(iSector);
    PndPattern pattern;
    pattern.SetSectorID(iSector);
    pattern.AddMomentum(momentum);
    for (auto iter = hitMapSectorRange.first; iter != hitMapSectorRange.second; ++iter) {
      int tubeID = iter->second;
      pattern.AddTubeID(tubeID);
    }
    pattern.RaisePatternCount();
    if (!pattern.IsEmpty()) {
      patterns.push_back(pattern);
    }
  }

  return patterns;
}
PndPatterns PndPatternDBGenerator::FillTrackPatterns(HitArray sttHitArray, PndMCTrack *mcTrack)
{
  PndPatterns patterns;
  TVector3 momentum = mcTrack->GetMomentum();
  PndPattern pattern;
  pattern.SetSectorID(-1); // sectorID undefined for whole track patterns
  pattern.AddMomentum(momentum);
  for (auto const &sttHit : sttHitArray) {
    int tubeID = sttHit->GetTubeID();
    pattern.AddTubeID(tubeID);
  }
  pattern.RaisePatternCount();
  if (!pattern.IsEmpty()) {
    patterns.push_back(pattern);
  }

  return patterns;
}
void PndPatternDBGenerator::AddPatternsToTree(PndPatterns patterns, TTree *tree)
{
  for (auto pattern : patterns) {
    bPattern = &pattern;
    tree->Fill();
  }
}
