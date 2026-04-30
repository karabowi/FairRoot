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
 * PndTrackingQualityBarrelAnalysisNewLinks.cxx
 *
 *  Created on: Aug 23, 2013
 *      Author: stockman
 */

#include "PndTrackingQualityBarrelAnalysisNewLinks.h"

#include "FairLogger.h"
#include "PndTrack.h"
#include "PndMCTrack.h"
#include "FairHit.h"
#include "PndSdsHit.h"
#include "TGeoManager.h"
#include "PndGeoHandling.h"
#include "PndTrackFunctor.h"
using namespace std;

ClassImp(PndTrackingQualityBarrelAnalysisNewLinks);

PndTrackingQualityBarrelAnalysisNewLinks::PndTrackingQualityBarrelAnalysisNewLinks(TString trackBranchName, TString idealTrackName, Bool_t pndTrackData)
  : fVerbose(1), ioman(nullptr), fNGhosts(0), fTrackBranchName(trackBranchName), fIdealTrackName(idealTrackName), fPndTrackOrTrackCand(pndTrackData), fPossibleTrack(0),
    fUseCorrectedSkewedHits(kFALSE), fBranchNames(), fTrackIdMCId(), fMCIdIdealTrackId(), fMCTrackFound(), fMapTrackMCStatus(), fMapTrackQualification(), fMapEfficiencies(),
    fMapPResolution(), fMapP(), fMapPtResolution(), fMapPt(), fMapPResolutionRel(), fMapPtResolutionRel(), fTrack(nullptr), fMCTrack(nullptr), fIdealTrack(nullptr),
    fIdealTrackCand(nullptr)
{
  if (fPossibleTrack == 0) {
    LOG(info) << " PndTrackingQualityBarrelAnalysisNewLinks::PndTrackingQualityBarrelAnalysisNewLinks no PossibleTrackFunctor given. Taking Standard!";
    if (trackBranchName == "MVDTrack") {
      fPossibleTrack = new RiemannMvdSttGemFunctor();
    } else if (trackBranchName == "CombiTrackCand") {
      fPossibleTrack = new OnlySttFunctor();
    } else {
      fPossibleTrack = new StandardTrackFunctor();
    }
  }
}

PndTrackingQualityBarrelAnalysisNewLinks::PndTrackingQualityBarrelAnalysisNewLinks(TString trackBranchName, TString idealTrackName, PossibleTrackFunctor *posTrack,
                                                                                   Bool_t pndTrackData)
  : fVerbose(1), ioman(nullptr), fNGhosts(0), fTrackBranchName(trackBranchName), fIdealTrackName(idealTrackName), fPndTrackOrTrackCand(pndTrackData), fPossibleTrack(posTrack),
    fUseCorrectedSkewedHits(kFALSE), fBranchNames(), fTrackIdMCId(), fMCIdIdealTrackId(), fMCTrackFound(), fMapTrackMCStatus(), fMapTrackQualification(), fMapEfficiencies(),
    fMapPResolution(), fMapP(), fMapPtResolution(), fMapPt(), fMapPResolutionRel(), fMapPtResolutionRel(), fTrack(nullptr), fMCTrack(nullptr), fIdealTrack(nullptr),
    fIdealTrackCand(nullptr)

{
  if (fPossibleTrack == 0) {
    LOG(info) << " PndTrackingQualityBarrelAnalysisNewLinks::PndTrackingQualityBarrelAnalysisNewLinks no PossibleTrackFunctor given. Taking Standard!";
    if (trackBranchName == "MVDTrack") {
      fPossibleTrack = new RiemannMvdSttGemFunctor();
    } else if (trackBranchName == "CombiTrackCand") {
      fPossibleTrack = new OnlySttFunctor();
    } else {
      fPossibleTrack = new StandardTrackFunctor();
    }
  }
}

PndTrackingQualityBarrelAnalysisNewLinks::~PndTrackingQualityBarrelAnalysisNewLinks()
{
  delete (fPossibleTrack);
}

void PndTrackingQualityBarrelAnalysisNewLinks::Init()
{
  ioman = FairRootManager::Instance();
  if (!ioman) {
    std::cout << "-E- PndTrackingQualityTask::Init: "
              << "RootManager not instantiated!" << std::endl;
    return;
  }

  fTrack = (TClonesArray *)ioman->GetObject(fTrackBranchName);
  fMCTrack = (TClonesArray *)ioman->GetObject("MCTrack");
  fIdealTrack = (TClonesArray *)ioman->GetObject(fIdealTrackName);
  fIdealTrack = (TClonesArray *)ioman->GetObject(fIdealTrackName);

  if (fBranchNames.size() == 0) {
    AddHitsBranchName("MVDHitsPixel");
    AddHitsBranchName("MVDHitsStrip");
    AddHitsBranchName("STTHit");
    AddHitsBranchName("GEMHit");

    //	if (FairRootManager::Instance()->GetBranchId("CorrectedSkewedHits")  > 0){
    //		AddHitsBranchName("CorrectedSkewedHits");
    //	}
  }
  std::cout << "-I- PndTrackingQualityBarrelAnalysisNewLinks::Init: PossibleTrackFunctor: ";
  fPossibleTrack->Print();
}

void PndTrackingQualityBarrelAnalysisNewLinks::AnalyseEvent(TClonesArray *recoTrackInfo)
{
  FillMapTrackQualifikation();
  std::cout << "PndTrackingQualityBarrelAnalysisNewLinks::AnalyseEvent() Track quality map before analysis: " << std::endl << std::endl;
  PrintTrackQualityMap(kTRUE);

  for (Int_t i = 0; i < fTrack->GetEntriesFast(); i++) {

    std::cout << "----------------------------------" << std::endl;

    std::cout << "Analyse Track: " << i << std::endl;

    std::map<TString, FairMultiLinkedData> trackInfo;

    if (fPndTrackOrTrackCand) {
      PndTrack *myTrack = (PndTrack *)fTrack->At(i);
      trackInfo = AnalyseTrackCand(myTrack->GetTrackCandPtr());
    } else {
      PndTrackCand *myTrack = (PndTrackCand *)fTrack->At(i);
      trackInfo = AnalyseTrackCand(myTrack);
    }
    if (fVerbose > 1) {
      std::cout << "PndTrackingQualityBarrelAnalysisNewLinks::AnalyseEvent Analyse track: " << i << std::endl;
    }
    Int_t mostProbableTrack = AnalyseTrackInfo(trackInfo, i);
    std::cout << "mostProbableTrack " << mostProbableTrack << std::endl;
    if (mostProbableTrack == -1)
      continue;

    fTrackIdMCId[i] = mostProbableTrack;
    CalcEfficiencies(mostProbableTrack, trackInfo);

    fMCTrackFound[mostProbableTrack]++;

    PndTrackingQualityRecoInfo recoinfo = GetRecoInfoFromRecoTrack(i, mostProbableTrack);
    int nof_asso_mctracks = trackInfo["AllHits"].GetNLinks();
    recoinfo.SetNofMCTracks(nof_asso_mctracks);
    int size = recoTrackInfo->GetEntriesFast();
    new ((*recoTrackInfo)[size]) PndTrackingQualityRecoInfo(recoinfo);
  }

  for (std::map<Int_t, Int_t>::iterator iter = fMapTrackQualification.begin(); iter != fMapTrackQualification.end(); iter++) {

    if (iter->first > -1 && iter->second > 0) {
      PndMCTrack *mcTrack = (PndMCTrack *)fMCTrack->At(iter->first);

      if (fPndTrackOrTrackCand) {
        PndTrack *myTrack = (PndTrack *)fTrack->At(fMCIdTrackId[iter->first]);
        TVector3 mom(myTrack->GetParamFirst().GetPx(), myTrack->GetParamFirst().GetPy(), myTrack->GetParamFirst().GetPz());
        TVector3 McMom(mcTrack->GetMomentum());

        fMapPResolution[iter->first] = (mom.Mag() - McMom.Mag());
        fMapP[iter->first] = mom;
        fMapPtResolution[iter->first] = (mom.Pt() - McMom.Pt());
        fMapPt[iter->first] = mom.Pt();
        fMapPResolutionRel[iter->first] = (mom.Mag() - McMom.Mag()) / McMom.Mag();
        fMapPtResolutionRel[iter->first] = (mom.Pt() - McMom.Pt()) / McMom.Pt();
      }
    }
  }
}

FairMultiLinkedData PndTrackingQualityBarrelAnalysisNewLinks::GetMCInfoForBranch(TString branchName, PndTrackCand *trackCand)
{
  FairMultiLinkedData result;
  result.SetInsertHistory(kFALSE);
  // std::cout << "GetMCInforForBranch: " << branchName << std::endl;
  FairMultiLinkedData linksOfType = trackCand->GetLinksWithType(ioman->GetBranchId(branchName));
  // std::cout << "GetMCInforForBranch: LinksOfType " << branchName << " : " << linksOfType << std::endl;

  for (int j = 0; j < linksOfType.GetNLinks(); j++) {
    FairMultiLinkedData_Interface *linkData = (FairMultiLinkedData_Interface *)FairRootManager::Instance()->GetCloneOfLinkData(linksOfType.GetLink(j));
    // std::cout << "CloneOfLinkData: " << *linkData << std::endl;
    if (linkData != 0) {
      FairMultiLinkedData linkDataType = linkData->GetLinksWithType(FairRootManager::Instance()->GetBranchId("MCTrack"));
      linkDataType.SetAllWeights(1.);
      result.AddLinks(linkDataType);
    }
  }
  return result;
}

std::map<TString, FairMultiLinkedData> PndTrackingQualityBarrelAnalysisNewLinks::AnalyseTrackCand(PndTrackCand *trackCand)
{
  std::map<TString, FairMultiLinkedData> trackInfo;

  if (fVerbose > 0) {
    std::cout << "PndTrackingQualityData::AnalyseTrackCand: TrackInfo" << std::endl;
    // std::cout << *trackCand << std::endl;
  }
  for (unsigned int branchIndex = 0; branchIndex < fBranchNames.size(); branchIndex++) {
    trackInfo[fBranchNames[branchIndex]] = GetMCInfoForBranch(fBranchNames[branchIndex], trackCand);
    trackInfo["AllHits"].AddLinks(trackInfo[fBranchNames[branchIndex]]);
  }
  if (fVerbose > 0)
    PrintTrackInfo(trackInfo);
  return trackInfo;
}

Int_t PndTrackingQualityBarrelAnalysisNewLinks::AnalyseTrackInfo(std::map<TString, FairMultiLinkedData> &trackInfo, Int_t trackId)
{

  Int_t mostProbableTrack = -1;
  //	if (fVerbose > 0)
  //		std::cout << "PndTrackingQualityBarrelAnalysisNewLinks::AnalyseTrackInfo: TrackInfo: " << trackInfo["AllHits"].GetNLinks() << std::endl;
  // PrintTrackDataSummary(trackInfo["AllHits"]);

  if (trackInfo["AllHits"].GetNLinks() == 1) {
    mostProbableTrack = trackInfo["AllHits"].GetLink(0).GetIndex();
    PndTrackCand *myIdealTrack = ((PndTrack *)fIdealTrack->At(fMCIdIdealTrackId[mostProbableTrack]))->GetTrackCandPtr();
    Int_t nMCHits = GetSumOfAllValidMCHits(myIdealTrack->GetPointerToLinks());

    if (nMCHits == trackInfo["AllHits"].GetLink(0).GetWeight()) {
      fMapTrackQualification[trackInfo["AllHits"].GetLink(0).GetIndex()] = qualityNumbers::kFullyFound;
      fMCIdTrackId[mostProbableTrack] = trackId;
    } else {
      if (fMapTrackQualification[trackInfo["AllHits"].GetLink(0).GetIndex()] != qualityNumbers::kFullyFound) {
        fMapTrackQualification[trackInfo["AllHits"].GetLink(0).GetIndex()] = qualityNumbers::kPartiallyFound;
        fMCIdTrackId[mostProbableTrack] = trackId;
      }
    }
  } else {
    Int_t highestCount = 0;
    Int_t allCounts = 0;
    for (int i = 0; i < trackInfo["AllHits"].GetNLinks(); i++) {
      allCounts += trackInfo["AllHits"].GetLink(i).GetWeight();
      if (trackInfo["AllHits"].GetLink(i).GetWeight() > highestCount) {
        highestCount = trackInfo["AllHits"].GetLink(i).GetWeight();
        mostProbableTrack = trackInfo["AllHits"].GetLink(i).GetIndex();
      }
    }

    if ((Double_t)highestCount / (Double_t)allCounts > 0.7) {
      if (fMapTrackQualification[mostProbableTrack] != qualityNumbers::kFullyFound && fMapTrackQualification[mostProbableTrack] != qualityNumbers::kPartiallyFound) {
        fMapTrackQualification[mostProbableTrack] = qualityNumbers::kSpuriousFound;
        fMCIdTrackId[mostProbableTrack] = trackId;
      }
    } else {
      fNGhosts++;
    }
  }
  if (fVerbose > 0) {
    for (int j = 0; j < trackInfo["AllHits"].GetNLinks(); j++) {
      FairLink myLink = trackInfo["AllHits"].GetLink(j);
      if (fMCIdIdealTrackId.count(myLink.GetIndex()) > 0) {
        PndTrackCand *myIdealTrack = ((PndTrack *)fIdealTrack->At(fMCIdIdealTrackId[myLink.GetIndex()]))->GetTrackCandPtr();
        std::cout << "Ideal Tracking: Track " << myLink.GetIndex() << ": ";
        PrintTrackDataSummary(*myIdealTrack->GetPointerToLinks());
      } else {
        std::cout << "Ideal Tracking: Track " << myLink.GetIndex() << " not available" << std::endl;
      }
    }
    std::cout << "MostProbableTrack: " << mostProbableTrack << " Quality: " << fMapTrackQualification[mostProbableTrack] << std::endl;
    std::cout << std::endl;
  }

  return mostProbableTrack;
}

void PndTrackingQualityBarrelAnalysisNewLinks::FillMapTrackQualifikation()
{
  fMapTrackQualification.clear();
  fMapTrackMCStatus.clear();
  fMCIdIdealTrackId.clear();
  for (int i = 0; i < fIdealTrack->GetEntriesFast(); i++) {
    PndTrackCand *idealTrackCand = ((PndTrack *)fIdealTrack->At(i))->GetTrackCandPtr();
    PndMCTrack *mcTrack = (PndMCTrack *)fMCTrack->At(idealTrackCand->getMcTrackId());
    fMCIdIdealTrackId[idealTrackCand->getMcTrackId()] = i;
    Bool_t primaryTrack = (mcTrack->GetMotherID() < 0);
    Bool_t atLeastThreeHits = kFALSE;
    Int_t nHits = 0;
    //			for (int branchIndex = 0; branchIndex < fBranchNames.size(); branchIndex++){
    //				TString branchName = fBranchNames[branchIndex];
    //				nHits += GetNIdealHits(i, branchName);
    //			}
    Int_t nMVDPixel = GetNIdealHits(*(idealTrackCand->GetPointerToLinks()), "MVDHitsPixel");
    Int_t nMVDStrip = GetNIdealHits(*(idealTrackCand->GetPointerToLinks()), "MVDHitsStrip");
    Int_t nSTT = GetNIdealHits(*(idealTrackCand->GetPointerToLinks()), "STTHit");
    // SG!!! nHits += GetNIdealHits(*(idealTrackCand->GetPointerToLinks()), "GEMHit");

    nHits += nMVDPixel;
    nHits += nMVDStrip;
    nHits += nSTT;
    // std::cout<<"SG: n hits "<<nMVDPixel<<" "<<nMVDStrip<<" "<<nSTT<<std::endl;
    // std::cout << "FillMapTrackQualifikation: NHits: " << nHits << std::endl;

    // if (nHits > 2) atLeastThreeHits = kTRUE;
    if (nHits >= 5 && (nMVDPixel + nMVDStrip >= 0))
      atLeastThreeHits = kTRUE; // SG!!!
    fMapTrackQualification[idealTrackCand->getMcTrackId()] = 0;

    if (atLeastThreeHits) {
      if (primaryTrack) {
        fMapTrackQualification[idealTrackCand->getMcTrackId()] = qualityNumbers::kAtLeastThreePrim;
      } else {
        fMapTrackQualification[idealTrackCand->getMcTrackId()] = qualityNumbers::kAtLeastThreeSec;
      }
      //	std::cout << "AtLeastThreeHits: Track "<< i << " status: " << fMapTrackQualification[i] << std::endl;
    } else if (primaryTrack) { // No hits for primary track in central tracking detectors
      fMapTrackQualification[idealTrackCand->getMcTrackId()] = qualityNumbers::kLessThanThreePrim;
    }
    /*
    PndTrackCand* entry = ((PndTrack*)fIdealTrack->At(i))->GetTrackCandPtr();
    if ((*fPossibleTrack)((FairMultiLinkedData*)entry->GetPointerToLinks(), primaryTrack))
    {
      if (primaryTrack) {
        fMapTrackQualification[idealTrackCand->getMcTrackId()] = qualityNumbers::kPossiblePrim;
      } else {
        fMapTrackQualification[idealTrackCand->getMcTrackId()] = qualityNumbers::kPossibleSec;
      }
    }
    */
  }
  if (fVerbose > 0) {
    LOG(info) << " PndMCTestPatternRecoQuality::FillMapTrackQualifikation:";
    //		PrintTrackQualityMap();
  }
  fMapTrackMCStatus = fMapTrackQualification;
}

// Bool_t PndTrackingQualityBarrelAnalysisNewLinks::PossibleTrack(FairMultiLinkedData& mcForward)
//{
//	Bool_t possibleTrack = kFALSE;
//
//	for (int i = 0; i < fBranchNames.size(); i++){
//		if (fBranchNames[i] == "MVDHitsPixel"){
//			possibleTrack = possibleTrack | (mcForward.GetLinksWithType(ioman->GetBranchId("MVDHitsPixel")).GetNLinks() +
//										     mcForward.GetLinksWithType(ioman->GetBranchId("MVDHitsStrip")).GetNLinks() > 3);
//
//		}
//		if (fBranchNames[i] == "STTHit"){
//			possibleTrack = possibleTrack | mcForward.GetLinksWithType(ioman->GetBranchId("STTHit")).GetNLinks() > 5;
//		}
//	}
//
//	return possibleTrack;
//}

Int_t PndTrackingQualityBarrelAnalysisNewLinks::GetSumOfAllValidMCHits(FairMultiLinkedData *trackData)
{
  Int_t result = 0;
  for (unsigned int branchIndex = 0; branchIndex < fBranchNames.size(); branchIndex++) {
    if (fBranchNames[branchIndex] == "GEMHit") {
      FairMultiLinkedData gemHits = trackData->GetLinksWithType(ioman->GetBranchId("GEMPoint"));
      result += gemHits.GetNLinks();
    } else {
      result += trackData->GetLinksWithType(ioman->GetBranchId(fBranchNames[branchIndex])).GetNLinks();
    }
  }
  return result;
}

void PndTrackingQualityBarrelAnalysisNewLinks::CalcEfficiencies(Int_t mostProbableTrack, std::map<TString, FairMultiLinkedData> &trackInfo)
{
  if (mostProbableTrack < 0)
    return;
  for (unsigned int branchIndex = 0; branchIndex < fBranchNames.size(); branchIndex++) {
    if (fMCIdIdealTrackId.count(mostProbableTrack) > 0) {
      PndTrackCand *trackCand = ((PndTrack *)fIdealTrack->At(fMCIdIdealTrackId[mostProbableTrack]))->GetTrackCandPtr();
      Int_t nMcHits = GetNIdealHits(*trackCand->GetPointerToLinks(), fBranchNames[branchIndex]);
      FairMultiLinkedData foundHits = trackInfo[fBranchNames[branchIndex]];
      for (int i = 0; i < foundHits.GetNLinks(); i++) {
        if (foundHits.GetLink(i).GetIndex() == mostProbableTrack) {
          Double_t nFoundHits = foundHits.GetLink(i).GetWeight();
          if ((nFoundHits / nMcHits) > fMapEfficiencies[mostProbableTrack][fBranchNames[branchIndex]].first) {
            std::pair<Double_t, Int_t> result(nFoundHits / nMcHits, nMcHits);
            fMapEfficiencies[mostProbableTrack][fBranchNames[branchIndex]] = result;
          }
        }
      }
    }
  }
}

// Int_t PndTrackingQualityBarrelAnalysisNewLinks::GetNIdealHits(Int_t trackId, TString branchName)
//{
//	PndMCEntry idealTrack = fIdealTrackCand.GetEntry(trackId);
//	return GetNIdealHits(idealTrack, branchName);
//
//}

Int_t PndTrackingQualityBarrelAnalysisNewLinks::GetNIdealHits(FairMultiLinkedData &track, TString branchName)
{
  Int_t numberGemHits = 0;
  if (branchName == "GEMHit") {
    numberGemHits = track.GetLinksWithType(ioman->GetBranchId("GEMPoint")).GetNLinks();
    return numberGemHits;
  } else if (branchName == "MVDHitsPixel" || branchName == "MVDHitsStrip") {
    // cout<<"\n branch "<<branchName<<endl;
    FairMultiLinkedData links = track.GetLinksWithType(ioman->GetBranchId(branchName));
    int nBarrel = NBarrelMVD(links);
    // return links.GetNLinks();
    return nBarrel;
  }

  return track.GetLinksWithType(ioman->GetBranchId(branchName)).GetNLinks();
}

void PndTrackingQualityBarrelAnalysisNewLinks::PrintTrackDataSummary(FairMultiLinkedData &trackData, Bool_t detailedInfo)
{
  if (detailedInfo == kTRUE)
    std::cout << std::endl;
  for (unsigned int branchIndex = 0; branchIndex < fBranchNames.size(); branchIndex++) {
    TString branchName = fBranchNames[branchIndex];
    std::cout << branchName << " " << GetNIdealHits(trackData, branchName);
    if (detailedInfo == kTRUE) {
      std::cout << " : ";
      if (trackData.GetLinksWithType(ioman->GetBranchId(branchName)).GetNLinks() > 0)
        std::cout << trackData.GetLinksWithType(ioman->GetBranchId(branchName));
      std::cout << std::endl;
    } else {
      std::cout << " | ";
    }
  }
  std::cout << std::endl;
}

void PndTrackingQualityBarrelAnalysisNewLinks::PrintTrackQualityMap(Bool_t detailedInfo)
{
  for (std::map<Int_t, Int_t>::iterator iter = fMapTrackQualification.begin(); iter != fMapTrackQualification.end(); iter++) {
    std::cout << "TrackID: " << iter->first << " MCQuality: " << fMapTrackMCStatus[iter->first] << " Quality: " << iter->second << " Found: " << fMCTrackFound[iter->first]
              << " MCData: ";
    if (fMCIdIdealTrackId.count(iter->first) > 0) {
      PndTrackCand *trackCand = ((PndTrack *)fIdealTrack->At(fMCIdIdealTrackId[iter->first]))->GetTrackCandPtr();
      PrintTrackDataSummary(*trackCand->GetPointerToLinks(), detailedInfo);
    }
  }
  std::cout << std::endl;
}

void PndTrackingQualityBarrelAnalysisNewLinks::PrintTrackMCStatusMap()
{
  std::cout << "PrintTrackMCStatusMap: " << std::endl;
  for (std::map<Int_t, Int_t>::iterator iter = fMapTrackMCStatus.begin(); iter != fMapTrackMCStatus.end(); iter++) {
    std::cout << "TrackID: " << iter->first << " Quality: " << iter->second << " Found: " << fMCTrackFound[iter->first];
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

void PndTrackingQualityBarrelAnalysisNewLinks::PrintTrackInfo(std::map<TString, FairMultiLinkedData> info)
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

Bool_t PndTrackingQualityBarrelAnalysisNewLinks::IsBetterTrackExisting(Int_t &mcIndex, int quality)
{
  if (fMapTrackQualification.count(mcIndex) == 1) {
    if (fMapTrackQualification[mcIndex] > 0 && fMapTrackQualification[mcIndex] > quality)
      return true;
  }
  return false;
}

PndTrackingQualityRecoInfo PndTrackingQualityBarrelAnalysisNewLinks::GetRecoInfoFromRecoTrack(Int_t trackId, Int_t mctrackId)
{
  PndTrackingQualityRecoInfo recoinfo(trackId);

  // CHECK this can be modified in the future, for now it is ok
  // mvd pix / mvd str / stt paral / stt skew / gem
  std::map<int, int> noftruehits;
  std::map<int, int> noffakehits;
  std::map<int, int> nofmissinghits;

  for (unsigned int branchIndex = 0; branchIndex < fBranchNames.size(); branchIndex++) {
    noftruehits.clear();
    noffakehits.clear();
    nofmissinghits.clear();

    // get the reco track...
    PndTrack *track = (PndTrack *)fTrack->At(trackId);
    // .. and the track cand
    PndTrackCand *trackcand = track->GetTrackCandPtr();

    recoinfo.SetPositionFirst(track->GetParamFirst().GetPosition());
    recoinfo.SetMomentumFirst(track->GetParamFirst().GetMomentum());

    recoinfo.SetPositionLast(track->GetParamLast().GetPosition());
    recoinfo.SetMomentumLast(track->GetParamLast().GetMomentum());

    recoinfo.SetCharge(track->GetParamFirst().GetQ());
    recoinfo.SetFlag(track->GetFlag());

    // get links associated to the reco track
    FairMultiLinkedData ptrlink = *trackcand->GetPointerToLinks();
    // get links corresponding to the hits of the specific detector
    FairMultiLinkedData links = ptrlink.GetLinksWithType(ioman->GetBranchId(fBranchNames[branchIndex]));
    // get their number
    Int_t nHits = links.GetNLinks();
    std::cout << "----- reco track " << trackId << " (mc track " << mctrackId << ") has " << nHits << " from " << fBranchNames[branchIndex] << std::endl;

    // get mc track info from each hit
    for (int ihit = 0; ihit < nHits; ihit++) {
      FairLink link = links.GetLink(ihit);
      int assomctrack = -1;
      // std::cout << "ihit " << ihit << " " << link.GetIndex() << " " << link.GetType() << " " << link.GetWeight() << std::endl;
      FairHit *hit = (FairHit *)links.GetData(link);
      if (!hit) {
        std::cout << "ihit " << ihit << " " << link.GetIndex() << " is FAKE" << std::endl;
        //    std::cout << "No Obj Hit" << std::endl;

        //
        if (noffakehits.count(branchIndex) > 0)
          noffakehits[branchIndex]++;
        else
          noffakehits[branchIndex] = 1; // if not there
        continue;
      }

      // get links of the hit
      FairMultiLinkedData hitlink = *hit->GetPointerToLinks();
      // get the links corresponding to the mc track associated to the hit
      FairMultiLinkedData mclinks = hitlink.GetLinksWithType(ioman->GetBranchId("MCTrack"));
      //  	  std::cout << "hit " << ihit  << " belongs to " << mclinks.GetNLinks() << " mc tracks" << std::endl;
      Bool_t isgood = kFALSE;
      FairMultiLinkedData mvdstrhits = links.GetLinksWithType(FairRootManager::Instance()->GetBranchId("MVDHitsStrip"));
      FairMultiLinkedData gemhits = links.GetLinksWithType(FairRootManager::Instance()->GetBranchId("GEMHit"));
      if ((gemhits.GetNLinks() > 0 || mvdstrhits.GetNLinks() > 0) && mclinks.GetNLinks() > 1) {
        isgood = kFALSE;
      } else {
        for (int imctrk = 0; imctrk < mclinks.GetNLinks(); imctrk++) {
          FairLink mclink = mclinks.GetLink(imctrk);
          assomctrack = mclink.GetIndex();
          // 	     std::cout << "imctrk " << imctrk << " " << mclink.GetIndex() << " " << mclink.GetType() << " " << mclink.GetWeight() << std::endl;
          std::cout << "ihit " << ihit << " (hitid " << link.GetIndex() << ") belongs to MC track " << assomctrack << std::endl;
          if (assomctrack == mctrackId)
            isgood = kTRUE;
        }
      }

      // if true
      if (isgood == kTRUE) {
        // if true and already there
        if (noftruehits.count(branchIndex) > 0)
          noftruehits[branchIndex]++;
        else
          noftruehits[branchIndex] = 1; // if not there
      } else {                          // if not
        if (noffakehits.count(branchIndex) > 0)
          noffakehits[branchIndex]++;
        else
          noffakehits[branchIndex] = 1; // if not there
      }
    }

    // retrieve the ideal track cand associated
    if (fMCIdIdealTrackId.count(mctrackId) > 0) {
      int idealtrackid = fMCIdIdealTrackId[mctrackId];
      PndTrack *idealtrack = (PndTrack *)fIdealTrack->At(idealtrackid);
      PndTrackCand *idealtrackcand = idealtrack->GetTrackCandPtr();

      Int_t nMcHits = GetNIdealHits(*idealtrackcand->GetPointerToLinks(), fBranchNames[branchIndex]);

      nofmissinghits[branchIndex] = nMcHits - noftruehits[branchIndex];

      std::cout << "**** ideal track " << idealtrackid << " has " << nMcHits << " from " << fBranchNames[branchIndex] << std::endl;
    }

    std::cout << "===> BRANCH " << fBranchNames[branchIndex] << " of track " << trackId << " (mc track " << mctrackId << ") has " << noftruehits[branchIndex] << " true, "
              << noffakehits[branchIndex] << " fake and " << nofmissinghits[branchIndex] << " missing hits" << std::endl;

    if (fBranchNames[branchIndex] == "MVDHitsPixel") {
      recoinfo.SetNofMvdPixelTrueHits(noftruehits[branchIndex]);
      recoinfo.SetNofMvdPixelFakeHits(noffakehits[branchIndex]);
      recoinfo.SetNofMvdPixelMissingHits(nofmissinghits[branchIndex]);
    } else if (fBranchNames[branchIndex] == "MVDHitsStrip") {
      recoinfo.SetNofMvdStripTrueHits(noftruehits[branchIndex]);
      recoinfo.SetNofMvdStripFakeHits(noffakehits[branchIndex]);
      recoinfo.SetNofMvdStripMissingHits(nofmissinghits[branchIndex]);
    } else if (fBranchNames[branchIndex] == "STTHit") {
      recoinfo.SetNofSttTrueHits(noftruehits[branchIndex]);
      recoinfo.SetNofSttFakeHits(noffakehits[branchIndex]);
      recoinfo.SetNofSttMissingHits(nofmissinghits[branchIndex]);
    } else if (fBranchNames[branchIndex] == "GEMHit") {
      recoinfo.SetNofGemTrueHits(noftruehits[branchIndex]);
      recoinfo.SetNofGemFakeHits(noffakehits[branchIndex]);
      recoinfo.SetNofGemMissingHits(nofmissinghits[branchIndex]);
    }
  }

  recoinfo.SetMCTrackID(mctrackId);
  return recoinfo;
}

Bool_t PndTrackingQualityBarrelAnalysisNewLinks::IsBarrelMVD(FairMultiLinkedData &links, int iHit)
{
  if (iHit < 0 || iHit >= links.GetNLinks()) {
    std::cout << "IsBarrelMVD():: iHit " << iHit << " is out of range [0," << links.GetNLinks() << "]" << std::endl;
    return 0;
  }
  FairLink link = links.GetLink(iHit);
  const PndSdsHit *hit = dynamic_cast<const PndSdsHit *>(links.GetData(link));
  if (!hit) {
    std::cout << "IsBarrelMVD():: iHit " << iHit << " " << link.GetIndex() << " is FAKE" << std::endl;
    return 0;
  }
  Int_t sensorID = hit->GetSensorID();
  gGeoManager->cd(PndGeoHandling::Instance()->GetPath(sensorID));
  TGeoHMatrix *transMat = gGeoManager->GetCurrentMatrix();
  if (!transMat) {
    std::cout << "\n\nIsBarrelMVD():: NO transition Matrix for MVD sensor " << sensorID << "\n\n!!!" << std::endl;
    return 0;
  }
  Double_t *mmm = transMat->GetRotationMatrix();
  if (!mmm) {
    std::cout << "\n\nIsBarrelMVD():: Can not extract transition Matrix for MVD sensor " << sensorID << ", something is completely wrong\n\n!!!" << std::endl;
    return 0;
  }
  bool forward = (fabs(mmm[6]) < 0.999 && fabs(mmm[7]) < 0.999);
  return (!forward);
}

Int_t PndTrackingQualityBarrelAnalysisNewLinks::NBarrelMVD(FairMultiLinkedData &links)
{
  int nBarrel = 0;
  for (Int_t ihit = 0; ihit < links.GetNLinks(); ihit++) {
    if (IsBarrelMVD(links, ihit))
      nBarrel++;
  }
  return nBarrel;
}
