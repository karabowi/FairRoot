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
 * PndTrackingQA.cxx
 *
 *  Created on: 21.06.2023
 *      Author: tstockmanns
 */

#include "PndTrackingQA.h"

#include "PndMCTrack.h"

#include "PndTrackCand.h"
#include "PndHelixPropagator.h"
#include "PndSttHit.h"
#include "PndSttTube.h"
#include "PndTrackingQAQualityNumbers.h"

#include "FairTrackParP.h"
#include "FairMultiLinkedData_Interface.h"

#include "TClonesArray.h"

ClassImp(PndTrackingQA);

PndTrackingQA::PndTrackingQA(TString trackBranchName, TString idealTrackName, Bool_t pndTrackNotTrackCand)
  : fTrackBranchName(trackBranchName), fIdealTrackName(idealTrackName), fPndTrackNotTrackCand(pndTrackNotTrackCand)
{
  if (fPossibleTrack == 0) {
    LOG(info) << " PndTrackingQA::PndTrackingQA no PossibleTrackFunctor given. Taking Standard!";
    fPossibleTrack = new StandardTrackFunctor();
  }
}

PndTrackingQA::PndTrackingQA(TString trackBranchName, TString idealTrackName, PndTrackFunctor *posTrack, Bool_t pndTrackNotTrackCand)
  : fTrackBranchName(trackBranchName), fIdealTrackName(idealTrackName), fPndTrackNotTrackCand(pndTrackNotTrackCand), fPossibleTrack(posTrack)

{
  if (fPossibleTrack == 0) {
    LOG(info) << " PndTrackingQA::PndTrackingQA no PossibleTrackFunctor given. Taking Standard!";
    fPossibleTrack = new StandardTrackFunctor();
  }
}

PndTrackingQA::~PndTrackingQA() {}

void PndTrackingQA::Init()
{
  ioman = FairRootManager::Instance();
  if (!ioman) {
    std::cout << "-E- PndTrackingQualityTask::Init: "
              << "RootManager not instantiated!" << std::endl;
    return;
  }

  fTrack = dynamic_cast<TClonesArray *>(ioman->GetObject(fTrackBranchName));
  fMCTrack = dynamic_cast<TClonesArray *>(ioman->GetObject("MCTrack"));
  fIdealTrack = dynamic_cast<TClonesArray *>(ioman->GetObject(fIdealTrackName));

  if (fBranchNames.size() == 0) {
    AddHitsBranchName("MVDHitsPixel");
    AddHitsBranchName("MVDHitsStrip");
    AddHitsBranchName("STTHit");
    AddHitsBranchName("GEMHit");
    AddHitsBranchName("FTSHit");
  }

//  if (fVerbose > 0) {
//    std::cout << "-I- PndTrackingQA::Init: PossibleTrackFunctor: ";
//    fPossibleTrack->Print();
//  }
}

void PndTrackingQA::AnalyseEvent(TClonesArray *recoTrackInfo, TClonesArray *mcTrackInfoArray)
{
  fMCInfoMap = FillMCTrackInfo(mcTrackInfoArray);

  //    std::cout << "MCInfoMap: " << std::endl;
  //    for (auto info : fMCInfoMap) {
  //      std::cout << *(info.second) << std::endl;
  //    }

  for (Int_t i = 0; i < fTrack->GetEntriesFast(); i++) {
    //    if (fVerbose > 0) {
    //      std::cout << "----------------------------------" << std::endl;
    //      std::cout << "Analyze Track: " << i << std::endl;
    //    }

    PndTrackCand *trackCand = nullptr;
    FairLink linkToTrack = dynamic_cast<FairMultiLinkedData_Interface *>(fTrack->At(i))->GetEntryNr();
    if (linkToTrack == FairLink()) {
      //      LOG(warning) << "Entry Number for track " << i << " not set. Generating one.";
      linkToTrack = FairLink(-1, ioman->GetEntryNr(), ioman->GetBranchId(fTrackBranchName), i);
      //      LOG(info) << "New Link to Track: " << linkToTrack << std::endl;
    }
    if (fPndTrackNotTrackCand) {
      trackCand = dynamic_cast<PndTrack *>(fTrack->At(i))->GetTrackCandPtr();
    } else {
      trackCand = dynamic_cast<PndTrackCand *>(fTrack->At(i));
    }
    if (trackCand->GetNHits() == 0) {
      LOG(error) << "No Hits in track";
      new ((*recoTrackInfo)[recoTrackInfo->GetEntriesFast()]) PndTrackingQARecoInfo();
      continue;
    }

    std::map<TString, FairMultiLinkedData> trackInfo = AnalyseTrackCand(trackCand);

    FairLink mostProbableMCTrack = GetMostProbableMCTrack(trackInfo);

    if (mostProbableMCTrack == FairLink()) {
      LOG(error) << "No mostProbableMCTrack found for trackCand " << std::endl;
      trackCand->Print();
      continue;
    }

    if (fMCInfoMap.count(mostProbableMCTrack) == 0) {
      LOG(warning) << "No MCInfo for mostProbableMCTrack " << mostProbableMCTrack << " mcTrackInfo.size() " << fMCInfoMap.size();
      LOG(warning) << "Creating dummy MCInfo";
      fMCInfoMap[mostProbableMCTrack] = new PndTrackingQAMCInfo();
    }
    PndTrackingQARecoInfo recoInfo = GetRecoInfoFromRecoTrack(linkToTrack, fMCInfoMap[mostProbableMCTrack]);
    recoInfo.SetQuality(TrackingQA::GetRecoQuality(&recoInfo));

    AssociateRecoTracksToMCTracks(recoInfo);

    new ((*recoTrackInfo)[recoTrackInfo->GetEntriesFast()]) PndTrackingQARecoInfo(recoInfo);
  }

  int i = 0;
  for (auto &mcInfo : fMCInfoMap) {
    //    mcInfo.second->FindBestRecoTrack(recoTrackInfo);
    mcInfo.second->FindBestRecoTrack(recoTrackInfo, [&recoTrackInfo](FairLink &a, FairLink &b) {
      // if (a.GetIndex() == -1 || b.GetIndex() == -1)
      PndTrackingQARecoInfo *aVal = dynamic_cast<PndTrackingQARecoInfo *>(recoTrackInfo->At(a.GetIndex()));
      PndTrackingQARecoInfo *bVal = dynamic_cast<PndTrackingQARecoInfo *>(recoTrackInfo->At(b.GetIndex()));
      double resA = aVal->GetEfficiency();
      double resB = bVal->GetEfficiency();
      return resA > resB;
    });
  }

  for (auto mc : fMCInfoMap) {
    new ((*mcTrackInfoArray)[mcTrackInfoArray->GetEntries()]) PndTrackingQAMCInfo(*(mc.second));
  }
}

std::map<FairLink, PndTrackingQAMCInfo *> PndTrackingQA::FillMCTrackInfo(TClonesArray *mcTrackInfo)
{
  std::map<FairLink, PndTrackingQAMCInfo *> mcInfoMap;

  for (int index = 0; index < fIdealTrack->GetEntriesFast(); index++) {
    PndTrack *idealTrack = dynamic_cast<PndTrack *>(fIdealTrack->At(index));

    if (idealTrack->GetEntryNr() == FairLink())
      idealTrack->SetEntryNr(FairLink(-1, ioman->GetEntryNr(), ioman->GetBranchId(fIdealTrackName), index));

    PndTrackCand *trackCand = idealTrack->GetTrackCandPtr();

    std::map<TString, FairMultiLinkedData> trackInfo = AnalyseTrackCand(trackCand);
    FairLink mostProbableMCTrack = GetMostProbableMCTrack(trackInfo);

    PndMCTrack *mcTrack = dynamic_cast<PndMCTrack *>(fMCTrack->At(mostProbableMCTrack.GetIndex()));

    if (mcTrack == nullptr) {
      LOG(error) << "No MC track for ideal track: ";
      LOG(error) << *idealTrack;
    }

    PndTrackingQAMCInfo *mcInfo = new PndTrackingQAMCInfo();

    mcInfo->SetMomentumFirst(idealTrack->GetParamFirst().GetMomentum());
    mcInfo->SetPositionFirst(idealTrack->GetParamFirst().GetPosition());
    mcInfo->SetMomentumLast(idealTrack->GetParamLast().GetMomentum());
    mcInfo->SetPositionLast(idealTrack->GetParamLast().GetPosition());
    mcInfo->SetIdealTrackLink(idealTrack->GetEntryNr());

    mcInfo->SetCharge(idealTrack->GetParamFirst().GetQ());
    mcInfo->SetIsPrimary(mcTrack->GetMotherID() < 0);
    mcInfo->SetMomentum(mcTrack->GetMomentum());
    mcInfo->SetVertex(mcTrack->GetStartVertex());
    mcInfo->SetPDGCode(mcTrack->GetPdgCode());
    mcInfo->SetdPCA2d(CalcPCA(idealTrack));
    mcInfo->SetMCTrackID(mostProbableMCTrack);

    PndTrackCand *idealtrkcand = idealTrack->GetTrackCandPtr(); // Ok time based

    mcInfo->SetNofPoints("MVDHitsPixel", idealtrkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId("MVDHitsPixel")));
    mcInfo->SetNofPoints("MVDHitsStrip", idealtrkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId("MVDHitsStrip")));
    mcInfo->SetNofPoints("GEMHit", idealtrkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId("GEMHit")));
    mcInfo->SetNofPoints("FTSHit", idealtrkcand->GetNHitsDet(FairRootManager::Instance()->GetBranchId("FTSHit")));

    int nofsttskewpoint = 0, nofsttparalpoint = 0;

    // This loop works event based
    // this loop counts skewed (--> parallel) STT/FTS hits

    for (size_t ihit = 0; ihit < idealtrkcand->GetNHits(); ihit++) {
      PndTrackCandHit idealcandhit = idealtrkcand->GetSortedHit(ihit);
      Int_t hitID = idealcandhit.GetHitId();
      Int_t detID = idealcandhit.GetDetId();

      if (detID != FairRootManager::Instance()->GetBranchId("STTHit"))
        continue;

      PndSttHit *stthit = (PndSttHit *)fSttHitArray->At(hitID);

      Int_t tubeID = stthit->GetTubeID();
      PndSttTube *tube = (PndSttTube *)fSttTubeArray->At(tubeID);
      if (tube->IsSkew())
        nofsttskewpoint++;
      else
        nofsttparalpoint++;
    }

    mcInfo->SetNofPoints("STTHitParal", nofsttparalpoint);
    mcInfo->SetNofPoints("STTHitSkew", nofsttskewpoint);

    int qualityNumber = static_cast<int>(GetMCInfoQualification(mcInfo, idealTrack));
    mcInfo->SetMCQuality(qualityNumber);
    mcInfo->SetQuality(qualityNumber);

    if (mcInfoMap.count(mostProbableMCTrack) > 0) { // it is possible that you have multiple MC track ids for one ideal track if e.g. a MVD pixel was hit by more than one track
//      LOG(error) << "Multiple ideal tracks for same MC track! " << mostProbableMCTrack;
//      LOG(error) << "Old mcInfo: " << *mcInfoMap[mostProbableMCTrack];
//      LOG(error) << "New mcInfo: " << *mcInfo;
      if (mcInfo->GetNofMCPoints() > mcInfoMap[mostProbableMCTrack]->GetNofMCPoints()) { // take the track with the largest number of hits
        mcInfoMap[mostProbableMCTrack] = mcInfo;
      }
    } else {
      mcInfoMap[mostProbableMCTrack] = mcInfo;
    }
  }

  //  std::cout << "FillMCInfo " << std::endl;
  //  for (auto val :mcInfoMap){
  //	  std::cout << *(val.second) << std::endl;
  //  }

  return mcInfoMap;
}

double PndTrackingQA::CalcPCA(PndTrack *track)
{
  PndHelixPropagator helixProp;
  FairTrackParP fTrackParamFirst = track->GetParamFirst();
  helixProp.Init(&fTrackParamFirst);
  helixProp.SetDestinationPoint(TVector3(0., 0., 0.));
  return helixProp.FindDistanceToPCAXYProjection(&fTrackParamFirst);
}

int PndTrackingQA::GetMCInfoQualification(PndTrackingQAMCInfo *mcInfo, PndTrack *idealTrack)
{

  bool primaryTrack = mcInfo->GetIsPrimary();
  //  if (fSecondaryDefinitionPCA) { // todo: this is not a clean solution. We should introduce a proper new classification for secondaries
  //    if (mcInfo->GetdPCA2d() > fdPCA) {
  //      primaryTrack = false;
  //    } else {
  //      primaryTrack = true;
  //    }
  //  }

  if ((*fPossibleTrack)((FairMultiLinkedData *)idealTrack->GetPointerToLinks(), primaryTrack)) {
    if (primaryTrack) {
      return (TrackingQA::qualityNumbers::kPossiblePrim);
    } else {
      return (TrackingQA::qualityNumbers::kPossibleSec);
    }
  }

  Bool_t atLeastThreeHits = kFALSE;

  if (mcInfo->GetNofMCPoints() > 2)
    atLeastThreeHits = kTRUE;

  if (atLeastThreeHits) {

    if (primaryTrack) {
      return (TrackingQA::qualityNumbers::kAtLeastThreePrim);
    } else {
      return TrackingQA::qualityNumbers::kAtLeastThreeSec;
    }
  }

  else if (primaryTrack) { // No hits for primary track in tracking detectors
    return (TrackingQA::qualityNumbers::kLessThanThreePrim);
  } else {
    return (TrackingQA::qualityNumbers::kLessThanThreeSec);
  }
}

std::map<TString, FairMultiLinkedData> PndTrackingQA::AnalyseTrackCand(PndTrackCand *trackCand)
{

  std::map<TString, FairMultiLinkedData> trackInfo;

  if (fVerbose > 2) {
    std::cout << "PndTrackingQualityData::AnalyseTrackCand: TrackInfo" << std::endl;
    std::cout << *trackCand << std::endl;
  }

  for (size_t branchIndex = 0; branchIndex < fBranchNames.size(); branchIndex++) {
    trackInfo[fBranchNames[branchIndex]] = GetMCInfoForBranch(fBranchNames[branchIndex], trackCand);
    trackInfo["AllHits"].AddLinks(trackInfo[fBranchNames[branchIndex]]);
  }
  if (fVerbose > 0)
    //    PrintTrackInfo(trackInfo);
    return trackInfo;
}

FairMultiLinkedData PndTrackingQA::GetMCInfoForBranch(TString branchName, PndTrackCand *trackCand)
{
  FairMultiLinkedData result;
  result.SetInsertHistory(kFALSE);
  FairMultiLinkedData linksOfType = trackCand->GetLinksWithType(ioman->GetBranchId(branchName));

  for (int j = 0; j < linksOfType.GetNLinks(); j++) {
    FairLink link = linksOfType.GetLink(j);

    if (link.GetEntry() < 0) {
      link.SetEntry(ioman->GetEntryNr());
    }

    FairMultiLinkedData_Interface *linkData = dynamic_cast<FairMultiLinkedData_Interface *>(ioman->GetCloneOfLinkData(link));

    if (linkData != 0) {
      FairMultiLinkedData linkDataType = linkData->GetLinksWithType(ioman->GetBranchId("MCTrack"));
      linkDataType.SetAllWeights(1.);
      result.AddLinks(linkDataType);
      linkData->Delete();
    }
  }
  return result;
}

void PndTrackingQA::PrintTrackInfo(std::map<TString, FairMultiLinkedData> info)
{
  std::cout << "TrackInfo: (MC-ID/NHits) : ";
  for (std::map<TString, FairMultiLinkedData>::iterator iter = info.begin(); iter != info.end(); iter++) {
    std::cout << iter->first;
    for (int i = 0; i < iter->second.GetNLinks(); i++) {
      std::cout << " : (" << iter->second.GetLink(i).GetIndex() << "/" << iter->second.GetLink(i).GetWeight() << ")";
    }
    std::cout << " || ";
  }
  std::cout << std::endl;
}

FairLink PndTrackingQA::GetMostProbableMCTrack(std::map<TString, FairMultiLinkedData> &trackInfo)
{
  FairLink mostProbableMCTrack;
  std::vector<FairLink> sortedMCTracks = trackInfo["AllHits"].GetSortedMCTracks();

  if (sortedMCTracks.size() == 0)
    return mostProbableMCTrack; // returns -1; no MCTracks
  else {
    mostProbableMCTrack = sortedMCTracks[0];
  }
  return mostProbableMCTrack;
}

PndTrackingQARecoInfo PndTrackingQA::GetRecoInfoFromRecoTrack(FairLink trackId, PndTrackingQAMCInfo *mcTrackInfo)
{

  PndTrackingQARecoInfo recoInfo(trackId);

  if (fVerbose > 2)
    std::cout << "PndTrackingQA::GetRecoInfoFromRecoTrack()" << std::endl;

  // get the reco track...
  PndTrack *track = dynamic_cast<PndTrack *>(fTrack->At(trackId.GetIndex())); // for timebased GetCloneOfLinkData has to be used

  recoInfo.SetPositionFirst(track->GetParamFirst().GetPosition());
  recoInfo.SetMomentumFirst(track->GetParamFirst().GetMomentum());

  recoInfo.SetPositionLast(track->GetParamLast().GetPosition());
  recoInfo.SetMomentumLast(track->GetParamLast().GetMomentum());

  recoInfo.SetCharge(track->GetParamFirst().GetQ());
  recoInfo.SetFitStatusFlag(track->GetFlag());

  recoInfo.SetNofMCTracks(track->GetSortedMCTracks().size());

  // get links associated to the reco track
  FairMultiLinkedData *ptrlink = track->GetTrackCandPtr()->GetPointerToLinks();

  for (size_t branchIndex = 0; branchIndex < fBranchNames.size(); branchIndex++) {
    int nOfTrueHits = 0;
    int nOfFakeHits = 0;

    int nOfSttSkewTrueHits = 0;
    int nOfSttParaTrueHits = 0;

    int nOfSttSkewFalseHits = 0;
    int nOfSttParaFalseHits = 0;

    // get links corresponding to the hits of the specific detector
    FairMultiLinkedData branchLinks = ptrlink->GetLinksWithType(ioman->GetBranchId(fBranchNames[branchIndex]));

    if (fVerbose > 2) {
      if (branchLinks.GetNLinks() > 0)
        std::cout << "----- reco track " << trackId << " (mc track " << mcTrackInfo->GetMCTrackID() << ") has " << branchLinks.GetNLinks() << " from " << fBranchNames[branchIndex]
                  << std::endl;
    }

    // get mc track info from each hit
    for (int ihit = 0; ihit < branchLinks.GetNLinks(); ihit++) {
      FairLink hitLink = branchLinks.GetLink(ihit);

      FairHit *hit = dynamic_cast<FairHit *>(branchLinks.GetData(hitLink)); // Need to use clone of link data

      if (hit == nullptr) { // this state should not be reached. Each hit with a FairLink should exist!
        LOG(error) << "Hit: " << ihit << " for FairLink " << hitLink << " does not EXIST";
        nOfFakeHits++;
        continue;
      }

      // get the links corresponding to the mc track associated to the hit
      FairMultiLinkedData mcLinks = hit->GetLinksWithType(ioman->GetBranchId("MCTrack"));
      Bool_t isGood = kFALSE;
      if (fBranchNames[branchIndex] == "MVDHitsStrip" || fBranchNames[branchIndex] == "GEMHit") {
        if (mcLinks.GetNLinks() == 1 && mcTrackInfo->GetMCTrackID() == mcLinks.GetLink(0)) // check for possible combination of wrong strips (hit has two MC Links)
          isGood = kTRUE;
      } else if (fBranchNames[branchIndex] == "STTHit") {
        PndSttHit *stthit = dynamic_cast<PndSttHit *>(hit);

        Int_t tubeID = stthit->GetTubeID();
        PndSttTube *tube = dynamic_cast<PndSttTube *>(fSttTubeArray->At(tubeID));
        for (int imctrk = 0; imctrk < mcLinks.GetNLinks(); imctrk++) {
          if (mcLinks.GetLink(imctrk) == mcTrackInfo->GetMCTrackID())
            isGood = kTRUE;
        }
        if (tube->IsSkew()) {
          if (isGood)
            nOfSttSkewTrueHits++;
          else
            nOfSttSkewFalseHits++;

        } else {
          if (isGood)
            nOfSttParaTrueHits++;
          else
            nOfSttParaFalseHits++;
        }

      } else {
        for (int imctrk = 0; imctrk < mcLinks.GetNLinks(); imctrk++) {
          if (mcLinks.GetLink(imctrk) == mcTrackInfo->GetMCTrackID())
            isGood = kTRUE;
        }
      }

      if (isGood == kTRUE) {
        nOfTrueHits++;
      } else {
        nOfFakeHits++;
      }
    }

    if (fBranchNames[branchIndex] == "STTHit") {
      recoInfo.SetTrueHits("STTHitParal", nOfSttParaTrueHits);
      recoInfo.SetFalseHits("STTHitParal", nOfSttParaFalseHits);
      recoInfo.SetTrueHits("STTHitSkew", nOfSttSkewTrueHits);
      recoInfo.SetFalseHits("STTHitSkew", nOfSttSkewFalseHits);
    } else {
      recoInfo.SetTrueHits(fBranchNames[branchIndex], nOfTrueHits);
      recoInfo.SetFalseHits(fBranchNames[branchIndex], nOfFakeHits);
    }
  }

  recoInfo.SetMCTrackID(mcTrackInfo->GetMCTrackID());
  recoInfo.SetMCTrackInfo(mcTrackInfo);
  recoInfo.SetIdealTrackId(mcTrackInfo->GetIdealTrackLink());

  return recoInfo;
}

void PndTrackingQA::AssociateRecoTracksToMCTracks(PndTrackingQARecoInfo &recoInfo)
{

  // loop over reco track infos
  FairLink mctrackid = recoInfo.GetMCTrackID(); // Get ID of associated MC track, this works only event based
  if (mctrackid.GetIndex() < 0)
    return;

  if (fMCInfoMap.count(mctrackid) == 0) {
    LOG(error) << "No MCInfo found for index: " << mctrackid;
    return;
  }
  PndTrackingQAMCInfo *mcInfo = fMCInfoMap[mctrackid];

  mcInfo->AddRecoTrackID(recoInfo.GetRecoTrackID()); // Sets the RecoTrackID of the mc info to the RecoTrackId of the recoInfo
}
