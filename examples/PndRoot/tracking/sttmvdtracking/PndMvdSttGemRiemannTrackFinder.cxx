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

#include "PndMvdSttGemRiemannTrackFinder.h"
#include <iostream>
#include <math.h>
#include "PndSdsHit.h"
#include "PndGemHit.h"
#include "PndSdsHit.h"
#include "PndSttHit.h"
#include "TString.h"
#include "TMath.h"
#include "FairLogger.h"

#include <FairRootManager.h>

using namespace std;

ClassImp(PndMvdSttGemRiemannTrackFinder);

PndMvdSttGemRiemannTrackFinder::PndMvdSttGemRiemannTrackFinder() : PndRiemannTrackFinder(), fSttHits(0), fGemHits(0), fZClosePar(0.1), fLastLayerId(0)
{
  fLayers.resize(22);
  if (fUseZeroPos)
    fLayers[0].push_back(0);
  fGeoH = PndGeoHandling::Instance();
  fSttHitsInSectors.resize(6);
  fSttSkewedHitsInSectors.resize(6);
  InitLayerMap();
}

PndMvdSttGemRiemannTrackFinder::~PndMvdSttGemRiemannTrackFinder() {}

void PndMvdSttGemRiemannTrackFinder::InitLayerMap()
{
  InitLayerMapMvd();
  InitLayerMapGem();
}

void PndMvdSttGemRiemannTrackFinder::InitLayerMapMvd()
{
  fLayerMap["PixeloBlo1"] = fLastLayerId + 1;
  fLayerMap["PixeloSdko(Silicon)_1"] = fLastLayerId + 2; // naming after MVD2.2
  fLayerMap["PixeloSdkoco(Silicon)_1"] = fLastLayerId + 2;
  fLayerMap["PixeloSdko(Silicon)_2"] = fLastLayerId + 3; // naming after MVD2.2
  fLayerMap["PixeloSdkoco(Silicon)_2"] = fLastLayerId + 3;
  fLayerMap["PixeloSdko(Silicon)_3"] = fLastLayerId + 4; // naming after MVD2.2
  fLayerMap["PixeloSdkoco(Silicon)_3"] = fLastLayerId + 4;
  fLayerMap["PixeloSdko(Silicon)_4"] = fLastLayerId + 5; // naming after MVD2.2
  fLayerMap["PixeloSdkoco(Silicon)_4"] = fLastLayerId + 5;
  fLayerMap["PixeloBlo2"] = fLastLayerId + 6;
  fLayerMap["PixeloLdkoio(Silicon)_1"] = fLastLayerId + 7; // naming after MVD2.2
  fLayerMap["PixeloLdkoco(Silicon)_1"] = fLastLayerId + 7;
  fLayerMap["PixeloLdkoiio(Silicon)_1"] = fLastLayerId + 8; // naming after MVD2.2
  fLayerMap["PixeloLdkoco(Silicon)_2"] = fLastLayerId + 8;
  fLayerMap["PixeloLdkoiiio(Silicon)_1"] = fLastLayerId + 9; // naming after MVD2.2
  fLayerMap["PixeloLdkoco(Silicon)_3"] = fLastLayerId + 9;
  fLayerMap["PixeloLdkoiiio(Silicon)_2"] = fLastLayerId + 10; // naming after MVD2.2
  fLayerMap["PixeloLdkoco(Silicon)_4"] = fLastLayerId + 10;
  fLayerMap["PixeloLdkoiio(Silicon)_2"] = fLastLayerId + 11; // naming after MVD2.2
  fLayerMap["PixeloLdkoco(Silicon)_5"] = fLastLayerId + 11;
  fLayerMap["PixeloLdkoio(Silicon)_2"] = fLastLayerId + 12; // naming after MVD2.2
  fLayerMap["PixeloLdkoco(Silicon)_6"] = fLastLayerId + 12;
  fLayerMap["PixeloLdkoiiio(Silicon)_3"] = fLastLayerId + 13; // naming after MVD2.2
  fLayerMap["PixeloLdkoco(Silicon)_7"] = fLastLayerId + 13;
  fLayerMap["PixeloLdkoiiio(Silicon)_4"] = fLastLayerId + 14; // naming after MVD2.2
  fLayerMap["PixeloLdkoco(Silicon)_8"] = fLastLayerId + 14;

  fLayerMap["StripoBl3o(Silicon)"] = fLastLayerId + 15;
  fLayerMap["Fwdo(Silicon)_1"] = fLastLayerId + 16;
  fLayerMap["StripoBl4o(Silicon)"] = fLastLayerId + 17;
  // fLayerMap["Fwdo(Silicon)_2")){Layer=10;flag=false;}

  fLayerMap["StripoLdkoTrapSoRingAoSilicon_1"] = fLastLayerId + 18;
  fLayerMap["StripoLdkoTrapSoRingAoSilicon_2"] = fLastLayerId + 19;
  fLayerMap["StripoLdkoTrapSoRingBoSilicon_1"] = fLastLayerId + 18;
  fLayerMap["StripoLdkoTrapSoRingBoSilicon_2"] = fLastLayerId + 19;
  fLayerMap["StripoLdko5-6oTrapSo(Silicon)_1"] = fLastLayerId + 20;

  fLayerMap["LambdaDisk_1"] = fLastLayerId + 21;
  fLayerMap["LambdaDisk_2"] = fLastLayerId + 22;
  fLastLayerId += 22;
}

void PndMvdSttGemRiemannTrackFinder::InitLayerMapGem()
{
  fLayerMap["Gem_1_1"] = fLastLayerId + 1;
  fLayerMap["Gem_1_2"] = fLastLayerId + 2;

  fLayerMap["Gem_2_1"] = fLastLayerId + 3;
  fLayerMap["Gem_2_2"] = fLastLayerId + 4;

  fLayerMap["Gem_3_1"] = fLastLayerId + 5;
  fLayerMap["Gem_3_2"] = fLastLayerId + 6;

  fLastLayerId += 6;
}

void PndMvdSttGemRiemannTrackFinder::AddHits(TClonesArray *hits, Int_t branchId)
{
  //	SetVerbose(3);
  FairRootManager *man = FairRootManager::Instance();
  //	PndRiemannTrackFinder::AddHits(hits, branchId);
  if (branchId == man->GetBranchId("STTHit")) {
    fSttHits = hits;
    for (int i = 0; i < fSttHits->GetEntries(); i++) {
      PndSttHit *myHit = (PndSttHit *)(fSttHits->At(i));
      myHit->SetEntryNr(FairLink(man->GetBranchId("SttHit"), i));
      fHits.push_back(myHit);
      myHit->SetPositionError(TVector3(0.5 / TMath::Sqrt(12), 0.5 / TMath::Sqrt(12), 200));
      TVector3 sttPos;
      myHit->Position(sttPos);
      FairLink myID;

      if (myHit->GetEntryNr().GetIndex() < 0 || myHit->GetEntryNr().GetType() < 0) {
        myID = FairLink(branchId, i);
        myHit->SetEntryNr(myID);
      } else
        myID = myHit->GetEntryNr();
      double phi = 0;
      //			if  (sttPos.Phi() < 0)
      //				phi = sttPos.Phi() + TMath::Pi() * 2;
      //			else
      //				phi = sttPos.Phi();
      if (fVerbose > 1)
        std::cout << "PndMvdSttGemRiemannTrackFinder::AddHits StrawMap Sector: " << myHit->GetEntryNr() << " " << myHit->GetTubeID() << ": Phi: " << sttPos.x() << "/" << sttPos.y()
                  << " " << phi << " Sector: " << fStrawMap.GetSector(myHit->GetTubeID()) << std::endl;
      fSttHitsInSectors[fStrawMap.GetSector(myHit->GetTubeID())].push_back(myHit);

      fMapHitToID[fHits.size() - 1] = myID;
      fMapIDtoHit[myID] = fHits.size() - 1;
    }
    return;
  }

  if (branchId == man->GetBranchId("STTCombinedSkewedHits")) {
    if (hits == 0) {
      std::cout << "PndMvdSttGemRiemannTrackFinder::AddHits: no SkewedHits Branch" << std::endl;
      return;
    }
    std::cout << "PndMvdSttGemRiemannTrackFinder::AddHits: SkewedHits " << hits->GetEntriesFast() << std::endl;
    fSkewedSttHits = hits;
    for (int i = 0; i < fSkewedSttHits->GetEntries(); i++) {
      FairHit *myHit = (FairHit *)(fSkewedSttHits->At(i));
      myHit->SetEntryNr(FairLink(man->GetBranchId("STTCombinedSkewedHits"), i));
      fHits.push_back(myHit);
      myHit->SetPositionError(TVector3(0.5 / TMath::Sqrt(12), 0.5 / TMath::Sqrt(12), 200));
      TVector3 sttPos;
      myHit->Position(sttPos);
      FairLink myID;

      if (myHit->GetEntryNr().GetIndex() < 0 || myHit->GetEntryNr().GetType() < 0) {
        myID = FairLink(branchId, i);
        myHit->SetEntryNr(myID);
      } else
        myID = myHit->GetEntryNr();
      double phi = 0;
      if (sttPos.Phi() < 0)
        phi = sttPos.Phi() + TMath::Pi() * 2;
      else
        phi = sttPos.Phi();
      //			if (fVerbose > 1)
      std::cout << "PndMvdSttGemRiemannTrackFinder::AddHits StrawMap Sector: " << myHit->GetEntryNr() << ": Phi: " << sttPos.x() << "/" << sttPos.y() << " " << phi
                << " Sector: " << fStrawMap.FindPhiSector(phi) << std::endl;
      fSttSkewedHitsInSectors[fStrawMap.FindPhiSector(phi)].push_back(myHit);

      fMapHitToID[fHits.size() - 1] = myID;
      fMapIDtoHit[myID] = fHits.size() - 1;
    }
    return;
  }

  if (branchId == man->GetBranchId("GEMHit")) {
    fGemHits = hits;
    if (fVerbose > 1)
      std::cout << "PndMvdSttGemRiemannTrackFinder::AddHits GEMHitsAdded: " << hits->GetEntries() << std::endl;
    for (int i = 0; i < hits->GetEntries(); i++) {
      FairHit *myHit = (FairHit *)(hits->At(i));
      fHits.push_back(myHit);
      FairLink myID;
      if (myHit->GetEntryNr().GetIndex() < 0) {
        myID = FairLink(branchId, i);
        myHit->SetEntryNr(myID);
      } else
        myID = myHit->GetEntryNr();
      fMapHitToID[fHits.size() - 1] = myID;
      fMapIDtoHit[myID] = fHits.size() - 1;
    }
    return;
  }

  for (int i = 0; i < hits->GetEntries(); i++) {
    FairHit *myHit = (FairHit *)(hits->At(i));
    fHits.push_back(myHit);
    FairLink myID;
    if (myHit->GetEntryNr().GetIndex() < 0) {
      myID = FairLink(branchId, i);
      myHit->SetEntryNr(myID);
    } else
      myID = myHit->GetEntryNr();
    fMapHitToID[fHits.size() - 1] = myID;
    fMapIDtoHit[myID] = fHits.size() - 1;

    int Layer = 0;
    if (branchId == man->GetBranchId("MVDHitsStrip") || branchId == man->GetBranchId("MVDHitsPixel")) {
      Layer = GetLayerMvd(myHit);
    } else if (branchId == man->GetBranchId("GEMHit")) {
      Layer = GetLayerGem(myHit);
    }

    fNLayers = fLayers.size();
    while (fNLayers < Layer + 1) {
      std::vector<int> dummy;
      fLayers.push_back(dummy);
      fNLayers = fLayers.size();
    }

    if (fVerbose > 1)
      std::cout << "fMapHitToId: " << fHits.size() - 1 << " : " << myID << " " << myHit->GetX() << "/" << myHit->GetY() << "/" << myHit->GetZ() << " Layer: " << Layer << std::endl;

    if (Layer > 0)
      fLayers[Layer].push_back(fHits.size() - 1); // putting hit in layers array
  }
  fNLayers = fLayers.size();
}

int PndMvdSttGemRiemannTrackFinder::GetLayer(TString identifier)
{
  std::map<TString, int>::iterator layerIter;
  for (layerIter = fLayerMap.begin(); layerIter != fLayerMap.end(); layerIter++) {
    if (identifier.Contains(layerIter->first)) {
      return layerIter->second;
    }
  }
  return 0;
}

int PndMvdSttGemRiemannTrackFinder::GetLayerGem(FairHit *hit)
{
  PndGemHit *gemHit = (PndGemHit *)hit;

  TString prefix("Gem_");
  prefix += (gemHit->GetStationNr());
  prefix += ("_");
  prefix += (gemHit->GetSensorNr());

  return GetLayer(prefix);
}

int PndMvdSttGemRiemannTrackFinder::GetLayerMvd(FairHit *hit)
{
  PndSdsHit *tempHit = (PndSdsHit *)(hit);
  TString geoPath = fGeoH->GetPath(tempHit->GetSensorID());

  return GetLayer(geoPath);
}

void PndMvdSttGemRiemannTrackFinder::FindTracks()
{

  //	SetVerbose(3);

  std::vector<std::set<Int_t>> Tracks = GetStartTracks(); // Get the possible track seeds

  std::set<int> tooClose;
  std::set<int>::iterator iter;

  fTracks.clear();
  fTrackCand.clear();
  for (unsigned int trackId = 0; trackId < Tracks.size(); trackId++) { // Go through all track seeds and search for additional points

    if (Tracks[trackId].size() != 3)
      if (fVerbose > 1)
        LOG(error) << " PndMVDRiemannTrackFinder::FindTracks: Start points: " << Tracks[trackId].size() << " in Track: " << trackId;

    std::set<Int_t> StartTrack = Tracks[trackId];

    if (fVerbose > 1) {
      if (fVerbose > 1) {
        std::cout << "------------------------------------" << std::endl;

        iter = StartTrack.begin();
        std::cout << "Start Plane from Points: " << fMapHitToID[*iter] << " " << fMapHitToID[*(++iter)] << " " << fMapHitToID[*(++iter)] << std::endl;
        iter = StartTrack.begin();
        std::cout << "Start Plane from Points: " << *iter << " " << *(iter++) << " " << *(iter++) << std::endl;
      }
    }

    if (TrackExists(StartTrack) == true) {
      if (fVerbose > 1)
        std::cout << "Track exists already!" << std::endl;
      continue;
    }

    PndRiemannTrack actTrack = CreateRiemannTrack(StartTrack);
    int startLayer = 0;
    int startHit = *(StartTrack.begin());

    bool flag = false;
    for (int i = 1; i < fNLayers; i++) { ///< finding layer's number of start hit
      for (unsigned int j = 0; j < fLayers[i].size(); j++) {
        if (fLayers[i][j] == startHit) {
          startLayer = i;
          flag = true;
          break;
        }
      }
      if (flag)
        break;
    }

    int testHit;
    for (int Layer = startLayer; Layer < fNLayers; Layer++) {
      // if ((fHits[startHit]->GetZ()<(-fZClosePar)) && (Layer==2 or Layer==3 or Layer==5 or Layer==6 or Layer==8 or Layer==10)) continue;  /// < in case of backward tracks disk
      // layers can't contain hits
      for (unsigned int testHitInLayer = 0; testHitInLayer < fLayers[Layer].size(); testHitInLayer++) {
        testHit = fLayers[Layer][testHitInLayer];

        if (fVerbose > 2)
          if (fVerbose > 1)
            std::cout << "Layer: " << Layer << " hitInLayer: " << testHitInLayer << " hitID " << testHit << " ";
        if (fVerbose > 1)
          std::cout << "Point " << fMapHitToID[testHit] << " ";

        if (CheckHitInTrack(StartTrack, testHit))
          continue;
        iter = StartTrack.begin();
        if (CheckHitDistance(*(iter), testHit) != true)
          continue;
        if (CheckHitDistance(*(++iter), testHit) != true)
          continue;
        if (CheckHitDistance(*(++iter), testHit) != true)
          continue;
        if (CheckZeroPassing(StartTrack, testHit) == true)
          continue;

        // if ((fHits[startHit]->GetZ())*(fHits[testHit]->GetZ())<0 && fabs(fHits[startHit]->GetZ())>fZClosePar && fabs(fHits[testHit]->GetZ())>fZClosePar)  continue; //check the
        // same direction on z axis

        PndRiemannHit actHit(fHits[testHit], testHit);
        if (CheckRiemannHit(&actTrack, &actHit, fHits[testHit]) != true)
          continue;

        StartTrack.insert(testHit);

        actTrack.addHit(actHit);
        actTrack.refit(false);
        actTrack.szFit(false);

        TVectorD orig = actTrack.orig();
        if (fVerbose > 1)
          std::cout << "actHit added: " << testHit << " r: " << actTrack.r() << " orig: " << orig[0] << " " << orig[1] << std::endl;
        tooClose = GetTooCloseHitsInLayer(Layer, testHit);
        fHitsTooClose[trackId].insert(tooClose.begin(), tooClose.end());
        // break;
      }
    }

    if (fVerbose > 1) {
      std::cout << std::endl;
      std::cout << "PndMvdSttGemRiemannTrackFinder::FindTracks ActTrack for Assignment: ";
      for (iter = StartTrack.begin(); iter != StartTrack.end(); iter++) {
        int hitId = *iter;
        std::cout << fMapHitToID[hitId] << " ";
      }
      std::cout << std::endl;
    }

    if (fVerbose > 1)
      std::cout << "Track before STT: " << actTrack << std::endl;

    if (fSttHits != nullptr) { // assign STTHits to Mvd track
      if (fVerbose > 1)
        std::cout << "PndMvdSttGemRiemannTrackFinder::FindTracks AssignSttHits" << std::endl;
      AssignSttHits(actTrack, StartTrack);
      if (fVerbose > 1)
        std::cout << "ActTrack size with Stt: " << actTrack.getNumHits() << " " << StartTrack.size() << std::endl;
      actTrack.refit(false);
      actTrack.szFit(false);
      actTrack.correctSttHits();
      actTrack.refit(false);
      actTrack.szFit(false);
      actTrack.correctSttHits();
      actTrack.refit(false);
      actTrack.szFit(false);
    }

    if (fSkewedSttHits != nullptr) {
      if (fVerbose > 1)
        std::cout << "PndMvdSttGemRiemannTrackFinder::FindTracks AssignSkewedSttHits" << std::endl;
      AssignSkewedSttHits(actTrack, StartTrack);
      if (fVerbose > 1)
        std::cout << "ActTrack size with Stt: " << actTrack.getNumHits() << " " << StartTrack.size() << std::endl;
      actTrack.refit(false);
      actTrack.szFit(false);
    }

    if (fVerbose > 1)
      std::cout << "Track after STT: " << actTrack << std::endl;

    if (fGemHits != nullptr) {
      if (fVerbose > 1)
        if (fVerbose > 1)
          std::cout << "PndMvdSttGemRiemannTrackFinder::FindTracks AssignGemHits" << std::endl;
      AssignGemHits(actTrack, StartTrack);
      if (fVerbose > 1)
        if (fVerbose > 1)
          std::cout << "ActTrack size with Gem: " << actTrack.getNumHits() << " " << StartTrack.size() << std::endl;
      actTrack.refit(false);
      actTrack.szFit(false);
    }

    if (fVerbose > 1)
      std::cout << "Track after GEM: " << actTrack << std::endl;

    if ((int)actTrack.getNumHits() > (fMinNumberOfHits - 1)) // if you have a track match check hits which were to close
    {
      std::set<int> hits = fHitsTooClose[trackId];
      if (fVerbose > 1)
        std::cout << "HitsTooClose Test for Track: " << actTrack << std::endl;
      for (iter = hits.begin(); iter != hits.end(); iter++) {
        if (fVerbose > 1)
          std::cout << "Too Close Point " << *iter << ": " << fMapHitToID[*iter];
        if (CheckHitInTrack(StartTrack, *iter))
          continue;
        PndRiemannHit actHit(fHits[*iter]);
        if (CheckRiemannHit(&actTrack, &actHit, fHits[*iter]) != true)
          continue;
        StartTrack.insert(*iter);
        actTrack.addHit(actHit);
      }

      //			actTrack.refit(false);
      //			actTrack.szFit(false);

      if (fVerbose > 1)
        std::cout << "Hits in Track: " << StartTrack.size() << std::endl;

      fTracks.push_back(actTrack);
      fHitsInTracks.push_back(StartTrack);

      if (fVerbose > 1) {
        TVectorD orig = actTrack.orig();
        if (fVerbose > 1)
          iter = StartTrack.begin();
        std::cout << "Track added! " << *iter << " " << *(++iter) << " " << *(++iter) << " r: " << actTrack.r() << " orig: " << orig[0] << " " << orig[1]
                  << " sz-m: " << actTrack.getSZm() << " sz-t: " << actTrack.getSZt() << " dip: " << actTrack.dip() << std::endl;
      }
      if (fVerbose > 1) {
        for (iter = StartTrack.begin(); iter != StartTrack.end(); iter++) {
          std::cout << " " << fMapHitToID[*iter];
        }
      }
    }
  }

  //	if (fVerbose > 1) std::cout << "In PndMvdRiemannTrackFinder are: " << fTracks.size() << " tracks" << std::endl;
  for (unsigned int n = 0; n < fTracks.size(); n++) {
    PndTrackCand myTrackCand;
    bool trackFound = false;
    std::vector<PndRiemannHit> TrackHits = fTracks[n].getHits();

    for (unsigned int p = 0; p < TrackHits.size(); p++) {
      if (fVerbose > 1)
        std::cout << "HitID: " << TrackHits[p].hitID() << std::endl;
      if (TrackHits[p].hitID() > -1) {
        myTrackCand.AddHit(fMapHitToID[TrackHits[p].hitID()], TrackHits[p].s());
      }
    }
    if (fVerbose > 1)
      std::cout << "NPoints TrackCand: " << myTrackCand.GetNHits() << std::endl;
    if (fVerbose > 1)
      myTrackCand.Print();
    FairMultiLinkedData gemHits = myTrackCand.GetLinksWithType(FairRootManager::Instance()->GetBranchId("GEMHit"));
    FairMultiLinkedData sttHits = myTrackCand.GetLinksWithType(FairRootManager::Instance()->GetBranchId("STTHit"));
    if (gemHits.GetNLinks() > 3) {
      fTrackCand.push_back(myTrackCand);
      trackFound = true;
    } else if (sttHits.GetNLinks() > 3 && CheckBoarderHitsStt(&myTrackCand)) {
      fTrackCand.push_back(myTrackCand);
      trackFound = true;
      //		} else if (gemHits.GetNLinks() == 0 && sttHits.GetNLinks() == 0) {
      //			fTrackCand.push_back(myTrackCand);
      //			trackFound = true;
    } else {
      fTracks.erase(fTracks.begin() + n);
      n--;
    }
    if (trackFound) {
      //			fTracks[n].refit(false);
      //			fTracks[n].szFit(false);
      std::pair<double, double> CurvDip(1 / fTracks[n].r(), fTracks[n].dip());
      fCurvAndDipOfCand.push_back(CurvDip);

      if (fVerbose > 0) {
        std::cout << "PndMvdSttGemRiemannTrackFinder::FindTracks TrackFound: " << myTrackCand << std::endl;
      }
      if (fVerbose > 1)
        std::cout << " numHits: " << fTracks[n].getNumHits() << std::endl;
      //			if (fVerbose > 1)
      {
        TVectorD myOrig = fTracks[n].orig();
        std::cout << "Track: " << fTracks[n] << std::endl;
        std::cout << " curv: " << 1 / fTracks[n].r() << "+/-" << fTracks[n].dR() / (fTracks[n].r() * fTracks[n].r()) << " dip: " << fTracks[n].dip() << "+/-" << fTracks[n].dDip()
                  << " orig: " << myOrig[0] << "+/-" << fTracks[n].dX() << " " << myOrig[1] << "+/-" << fTracks[n].dY() << std::endl;
        std::cout << "PlaneChiSquare: " << fTracks[n].calcChi2Plane() << std::endl;
      }
    }
  }

  //	if (fVerbose > 1) std::cout << "Hits before merging: " << fTrackCand.size() << std::endl;
  //	MergeTracks();
  //	fTrackCand.clear();
  //	fTrackCand = fMergedTrackCand;
  //	if (fVerbose > 0) {
  //		if (fVerbose > 1) std::cout << "Tracks after merging:" << fMergedTrackCand.size() << std::endl;
  //		for (unsigned int p = 0; p < fTrackCand.size(); p++){
  //			fTrackCand[p].Print();
  //		}											///todo Create RiemannTracks out of PndTrackCands
  //	}
}

std::vector<std::set<Int_t>> PndMvdSttGemRiemannTrackFinder::GetStartTracks()
{
  std::set<Int_t> actCandidates;
  std::vector<std::set<Int_t>> Tracks;
  std::set<int> tooCloseFirst;
  std::set<int> tooCloseSecond;
  std::set<int> tooCloseThird;
  if (fHits.size() > 3) {
    int shift = 0;
    if (fUseZeroPos)
      shift = 1;
    for (int FirstLayer = 1 - shift; FirstLayer < fNLayers - 2; FirstLayer++) { /// going through layers : first, second and third
      for (int SecondLayer = FirstLayer + 1; SecondLayer < fNLayers - 1; SecondLayer++) {
        //				for (int ThirdLayer = SecondLayer + 1; ThirdLayer < fNLayers; ThirdLayer++) {
        for (int ThirdLayer = fNLayers - 1; ThirdLayer > SecondLayer; ThirdLayer--) {

          for (unsigned int firstInLayer = 0; firstInLayer < fLayers[FirstLayer].size(); firstInLayer++) {
            int first = fLayers[FirstLayer][firstInLayer];
            // if ((fHits[first]->GetZ()<(-fZClosePar)) && (SecondLayer==2 or SecondLayer==3 or SecondLayer==5 or SecondLayer==6 or SecondLayer==8 or SecondLayer==10)) continue;
            for (unsigned int secondInLayer = 0; secondInLayer < fLayers[SecondLayer].size(); secondInLayer++) {
              int second = fLayers[SecondLayer][secondInLayer];
              // if ((fHits[second]->GetZ()<(-fZClosePar)) && (ThirdLayer==2 or ThirdLayer==3 or ThirdLayer==5 or ThirdLayer==6 or ThirdLayer==8 or ThirdLayer==10)) continue;
              // if ((fHits[first]->GetZ())*(fHits[second]->GetZ())<0 && fabs(fHits[first]->GetZ())>fZClosePar && fabs(fHits[second]->GetZ())>fZClosePar) {/*printf("Diff Sign of
              // Points  z1=%e  z2=%e \n",fHits[first]->GetZ(),fHits[second]->GetZ()); */continue;} //my// check the same direction on z axis

              for (unsigned int thirdInLayer = 0; thirdInLayer < fLayers[ThirdLayer].size(); thirdInLayer++) {
                int third = fLayers[ThirdLayer][thirdInLayer];
                // if ((fHits[second]->GetZ())*(fHits[third]->GetZ())<0 && fabs(fHits[second]->GetZ())>fZClosePar && fabs(fHits[third]->GetZ())>fZClosePar) {/*printf("Diff Sign of
                // Points  z1=%e  z2=%e \n",fHits[first]->GetZ(),fHits[second]->GetZ());*/ continue;} //my// check the same direction on z axis
                if (fVerbose > 2)
                  std::cout << "Checking Points for Start Triplet: " << first << " " << second << " " << third << std::endl;
                if (fVerbose > 2)
                  std::cout << "Checking Points for Start Triplet: " << fMapHitToID[first] << " " << fMapHitToID[second] << " " << fMapHitToID[third] << std::endl;

                if (CheckHitDistance(first, third) != true) {
                  tooCloseFirst.insert(third);
                  continue;
                }
                if (CheckHitDistance(second, third) != true) {
                  tooCloseSecond.insert(third);
                  continue;
                } ///<---------
                if (CheckHitInSameSensor(first, third) == true)
                  continue;
                if (CheckHitInSameSensor(second, third) == true)
                  continue;

                actCandidates.clear();
                actCandidates.insert(first);
                actCandidates.insert(second);

                if (CheckZeroPassing(actCandidates, third) == true)
                  continue;

                actCandidates.insert(third);

                PndRiemannTrack actTrack; // = new PndRiemannTrack();
                PndRiemannHit hit1(fHits[first]);
                PndRiemannHit hit2(fHits[second]);
                PndRiemannHit hit3(fHits[third]);
                actTrack.addHit(hit1);
                actTrack.addHit(hit2);
                actTrack.addHit(hit3);
                actTrack.refit(false);
                if (CheckSZ(actTrack) != true)
                  continue;
                TVectorT<double> orig = actTrack.orig();
                //								if (fVerbose > 1)
                {
                  std::set<Int_t>::iterator iter = actCandidates.begin();
                  std::cout << "Base plane from Points: " << *(iter) << " " << *(iter++) << " " << *(iter++) << std::endl;
                  std::cout << "TrackParameter: " << actTrack << std::endl;
                }
                Tracks.push_back(actCandidates);
                tooCloseFirst = GetTooCloseHitsInLayer(FirstLayer, first);
                tooCloseSecond = GetTooCloseHitsInLayer(SecondLayer, second);
                tooCloseThird = GetTooCloseHitsInLayer(ThirdLayer, third);
                std::set<int> combHits = tooCloseFirst;
                combHits.insert(tooCloseSecond.begin(), tooCloseSecond.end());
                combHits.insert(tooCloseThird.begin(), tooCloseThird.end());
                fHitsTooClose.push_back(combHits);
              }
            }
          }
        }
      }
    }
  }

  if (fVerbose > 1) {
    std::cout << "Start Tracks are: " << std::endl;
    for (unsigned int i = 0; i < Tracks.size(); i++) {
      std::set<int> aTrack = Tracks[i];
      for (std::set<int>::iterator iter = aTrack.begin(); iter != aTrack.end(); iter++) {
        std::cout << *iter << " ";
      }
      std::cout << std::endl;
      for (std::set<int>::iterator iter = aTrack.begin(); iter != aTrack.end(); iter++) {
        std::cout << fMapHitToID[*iter] << " ";
      }
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }
  return Tracks;
}

void PndMvdSttGemRiemannTrackFinder::AssignSttHits(PndRiemannTrack &actTrack, std::set<Int_t> &startTrack)
{
  int startSector = GetStrawSector(actTrack);
  int searchSector = startSector;
  bool continueSearch = true;
  int nextSectorDirection = 0;
  int nHitsStt = -1;

  if (fVerbose > 2)
    std::cout << "StartSector: " << searchSector << std::endl;

  while (continueSearch) {
    if (nHitsStt ==
        0) { // check if you have found any Stt hits in the selected startSector. If this is not the case take the left one. If there are still no Stt hits take the right one
      if (startSector == searchSector) {
        searchSector = fStrawMap.GetRightSector(startSector);
      } else {
        searchSector = fStrawMap.GetLeftSector(startSector);
      }
    }

    std::vector<PndSttHit *> sttHits = fSttHitsInSectors[searchSector];
    continueSearch = false;
    if (fVerbose > 2)
      LOG(info) << " PndMvdSttGemRiemannTrackFinder SearchSector: Actual searchSector: " << searchSector;

    for (size_t i = 0; i < sttHits.size(); i++) {

      if (nHitsStt < 0)
        nHitsStt = 0;

      PndSttHit *fairSttHit = sttHits[i];

      if (fStrawMap.IsSkewedStraw(fairSttHit->GetTubeID())) {
        //				actTrack.correctSttSkewedHit(fairSttHit, fStrawMap.GetTube(fairSttHit->GetTubeID()));
        continue;
      }

      //			PndSttHit* correctedSttHit = new PndSttHit(CalculateCorrectedSTTHit(*fairSttHit, actTrack));
      //			correctedSttHit->SetDxyz(fairSttHit->GetDx(), fairSttHit->GetDy(), fairSttHit->GetDz());
      //			correctedSttHit->SetEntryNr(FairLink(FairRootManager::Instance()->GetBranchId("STTHit"), i));
      PndRiemannHit actHit(fairSttHit, fMapIDtoHit[fairSttHit->GetEntryNr()]);

      if (fVerbose > 2) {
        std::cout << std::endl;
        LOG(info) << " PndMvdSttGemRiemannTrackFinder SearchSector: Act Hit: " << fairSttHit->GetEntryNr();
      }

      //			if (CheckRiemannHitStt(&actTrack, &actHit,(FairHit*)fSttHits->At(i)) != true) continue;
      if (CheckRiemannHitStt(&actTrack, &actHit, fairSttHit) != true)
        continue;

      if (fVerbose > 2)
        LOG(info) << " PndMvdSttGemRiemannTrackFinder SearchSector: STTHit add to Track!";

      actTrack.addHit(actHit);
      startTrack.insert(fMapIDtoHit[actHit.hit()->GetEntryNr()]);
      actTrack.refit(kFALSE);
      //			actTrack.szFit(kFALSE);
      nHitsStt++;
      if (fStrawMap.IsSectorBorderStraw(fairSttHit->GetTubeID()) != 0) {
        if (fVerbose > 2)
          std::cout << " TubeID: " << fairSttHit->GetTubeID() << " Sector: " << fStrawMap.GetSector(fairSttHit->GetTubeID()) << std::endl;
        if (nextSectorDirection == 0) {
          nextSectorDirection = fStrawMap.IsSectorBorderStraw(fairSttHit->GetTubeID());
        } else if (nextSectorDirection != fStrawMap.IsSectorBorderStraw(fairSttHit->GetTubeID())) {
          continue;
        }
        searchSector = fStrawMap.GetSector(fairSttHit->GetTubeID()) + fStrawMap.IsSectorBorderStraw(fairSttHit->GetTubeID());
        searchSector %= 6;
        if (searchSector < 0)
          searchSector = 5;

        if (fVerbose > 2)
          std::cout << "-I- PndMvdSttGemRiemannTrackFinder SearchSector: Next Sector: " << searchSector << " " << fStrawMap.IsSectorBorderStraw(fairSttHit->GetTubeID())
                    << std::endl;

        continueSearch = true;
      }
    }
    if (nHitsStt == 0) { // if you have not found any sttHits and you have not looked yet into the left sector from the start sector continue otherwise stop.
      if (searchSector != fStrawMap.GetLeftSector(startSector))
        continueSearch = true;
      else
        continueSearch = false;
    }
  }
}

void PndMvdSttGemRiemannTrackFinder::AssignSkewedSttHits(PndRiemannTrack &actTrack, std::set<Int_t> &startTrack)
{
  int startSector = GetStrawSector(actTrack);
  int searchSector = startSector;
  int nHitsStt = -1;

  //	SetVerbose(3);

  if (fVerbose > 2)
    std::cout << "StartSector: " << searchSector << std::endl;

  std::vector<FairHit *> sttHits = fSttSkewedHitsInSectors[searchSector];
  int leftSector = fStrawMap.GetLeftSector(startSector);
  int rightSector = fStrawMap.GetRightSector(startSector);
  sttHits.insert(sttHits.end(), fSttSkewedHitsInSectors[leftSector].begin(), fSttSkewedHitsInSectors[leftSector].end());
  sttHits.insert(sttHits.end(), fSttSkewedHitsInSectors[rightSector].begin(), fSttSkewedHitsInSectors[rightSector].end());

  if (fVerbose > 2)
    LOG(info) << " PndMvdSttGemRiemannTrackFinder SearchSector: Actual searchSector: " << searchSector;

  for (size_t i = 0; i < sttHits.size(); i++) {

    if (nHitsStt < 0)
      nHitsStt = 0;

    FairHit *fairSttHit = sttHits[i];

    PndRiemannHit actHit(fairSttHit, fMapIDtoHit[fairSttHit->GetEntryNr()]);

    if (fVerbose > 2) {
      std::cout << std::endl;
      LOG(info) << " PndMvdSttGemRiemannTrackFinder SearchSector: Act Hit: " << fairSttHit->GetEntryNr();
    }

    if (CheckRiemannHitSkewedStt(&actTrack, &actHit, fairSttHit) != true)
      continue;

    if (fVerbose > 2)
      LOG(info) << " PndMvdSttGemRiemannTrackFinder SearchSector: STTHit add to Track!";

    actTrack.addHit(actHit);
    startTrack.insert(fMapIDtoHit[actHit.hit()->GetEntryNr()]);
    actTrack.refit(kFALSE);
    //			actTrack.szFit(kFALSE);
    nHitsStt++;
  }
  //	SetVerbose(0);
}

void PndMvdSttGemRiemannTrackFinder::AssignGemHits(PndRiemannTrack &actTrack, set<Int_t> &startTrack)
{

  if (fVerbose > 1)
    std::cout << "NGemHits: " << fGemHits->GetEntriesFast() << std::endl;
  for (int i = 0; i < fGemHits->GetEntriesFast(); i++) {
    FairHit *fairGemHit = (FairHit *)fGemHits->At(i);
    PndRiemannHit actHit(fairGemHit, fMapIDtoHit[fairGemHit->GetEntryNr()]);

    if (CheckRiemannHitGem(&actTrack, &actHit, fairGemHit) != true)
      continue;

    if (fVerbose > 1)
      LOG(info) << " PndMvdSttGemRiemannTrackFinder SearchSector: GemHit add to Track!";

    actTrack.addHit(actHit);
    startTrack.insert(fMapIDtoHit[actHit.hit()->GetEntryNr()]);
  }
}

bool PndMvdSttGemRiemannTrackFinder::CheckSZ(PndRiemannTrack aTrack)
{
  aTrack.szFit(false);
  double r = aTrack.r();
  double dip = aTrack.dip();
  bool sign;
  if ((aTrack.getHit(0)->z()) > 0)
    sign = true;
  else
    sign = false;
  if (aTrack.szChi2() > GetMaxSZChi2(r, dip, sign) || aTrack.szChi2() < 0) {
    if (fVerbose > 1)
      std::cout << "sz-Fit does not match, Chi2: " << aTrack.szChi2() << " max: " << GetMaxSZChi2(r, dip, sign) << std::endl;
    return false;
  }
  return true;
}

bool PndMvdSttGemRiemannTrackFinder::CheckRiemannHit(PndRiemannTrack *track, PndRiemannHit *hit, FairHit *fairHit)
{
  if (fairHit->GetEntryNr().GetType() == FairRootManager::Instance()->GetBranchId("MVDHitsStrip") ||
      fairHit->GetEntryNr().GetType() == FairRootManager::Instance()->GetBranchId("MVDHitsPixel")) {
    return CheckRiemannHitMvd(track, hit, fairHit);
  } else if (fairHit->GetEntryNr().GetType() == FairRootManager::Instance()->GetBranchId("GEMHit")) {
    return CheckRiemannHitGem(track, hit, fairHit);
  } else if (fairHit->GetEntryNr().GetType() == FairRootManager::Instance()->GetBranchId("STTHit")) {
    return CheckRiemannHitStt(track, hit, fairHit);
  } else {
    std::cout << "-E- PndMvdRIemannTrackFinder::CheckRiemannHit: Detector " << FairRootManager::Instance()->GetBranchName(fairHit->GetEntryNr().GetType()) << " not supported!"
              << std::endl;
  }
  return false;
}

bool PndMvdSttGemRiemannTrackFinder::CheckRiemannHitStt(PndRiemannTrack *track, PndRiemannHit *hit, FairHit *fairHit)
{
  PndRiemannHit tempHit = track->correctSttHit((PndSttHit *)fairHit);

  //	std::cout << "CheckRiemannHit PndRiemannHit: " << fairHit->GetEntryNr() << " : " << hit->x().X() << "/" << hit->x().Y() << "/" << hit->x().Z() << " " << hit->z() <<
  // std::endl; 	std::cout << "CheckRiemannHit PndRiemannHit: " << tempHit.x().X() << "/" << tempHit.x().Y() << "/" << tempHit.x().Z() << " " << tempHit.z() << std::endl;

  hit->setXYZ(tempHit.x().X(), tempHit.x().Y(), tempHit.x().Z());
  hit->setDXYZ(tempHit.sigmaX(), tempHit.sigmaY(), 200);

  //	std::cout << "CheckRiemannHit PndRiemannHit: " << hit->x().X() << "/" << hit->x().Y() << "/" << hit->x().Z() << " " << hit->z() << std::endl;

  double dist = track->dist(hit);
  //	std::cout << "CheckRiemannHit PndRiemannHit:Dist " << dist << std::endl;

  double maxDist = 1;
  //	double szDist = track->szDist(hit);
  //	double maxSZDist = 10;
  //	double szChi2 = track->calcSZChi2(hit);
  //	double maxSZChi2 = 25;
  // double r = track->r(); //[R.K. 01/2017] unused variable?
  // bool sign; //[R.K.02/2017] Unused variable?
  // if ((track->getHit(1)->z())>0 ) //[R.K.02/2017] Unused variable?
  // sign=true; //[R.K.02/2017] Unused variable?
  // else sign=false; //[R.K.02/2017] Unused variable?
  if (fVerbose > 1)
    std::cout << "STTHit: dist " << dist << std::endl;

  if (fabs(dist) > maxDist) {
    if (fVerbose > 1)
      std::cout << "dist larger than " << maxDist << std::endl;
    return false;
  } else {

    //		std::cout << "PndMvdSttGemRiemannTrackFinder::CheckRiemannHitStt Hit taken! Dist: " << dist << " < " << maxDist << std::endl << std::endl;
  }
  //
  //	if (szChi2 > maxSZChi2){
  //		if (fVerbose > 1) std::cout << " SZ Chi2 too big! Cut at: " << maxSZChi2 << std::endl;
  //		return false;
  //	}
  //	if (fabs(szDist) > maxSZDist){
  //		if (fVerbose > 1) std::cout << "SZ Dist too big! Cut at: " << maxSZDist << std::endl;
  //		return false;
  //	}
  return true;
}

bool PndMvdSttGemRiemannTrackFinder::CheckRiemannHitSkewedStt(PndRiemannTrack *track, PndRiemannHit *hit, FairHit *) // fairHit //[R.K.03/2017] unused variable(s)
{

  //	std::cout << "CheckRiemannHit PndRiemannHit: " << hit->x().X() << "/" << hit->x().Y() << "/" << hit->x().Z() << " " << hit->z() << std::endl;

  double dist = track->dist(hit);
  //	std::cout << "CheckRiemannHit PndRiemannHit:Dist " << dist << std::endl;

  double maxDist = 1;
  //	double szDist = track->szDist(hit);
  //	double maxSZDist = 10;
  //	double szChi2 = track->calcSZChi2(hit);
  //	double maxSZChi2 = 25;
  // double r = track->r(); //[R.K. 01/2017] unused variable?
  // bool sign; //[R.K.02/2017] Unused variable?
  // if ((track->getHit(1)->z())>0 ) //[R.K.02/2017] Unused variable?
  // sign=true; //[R.K.02/2017] Unused variable?
  // else sign=false; //[R.K.02/2017] Unused variable?
  if (fVerbose > 1)
    std::cout << "STTHit: dist " << dist << std::endl;

  if (fabs(dist) > maxDist) {
    if (fVerbose > 1)
      std::cout << "dist larger than " << maxDist << std::endl;
    return false;
  } else {

    //		std::cout << "PndMvdSttGemRiemannTrackFinder::CheckRiemannHitStt Hit taken! Dist: " << dist << " < " << maxDist << std::endl << std::endl;
  }
  //
  //	if (szChi2 > maxSZChi2){
  //		if (fVerbose > 1) std::cout << " SZ Chi2 too big! Cut at: " << maxSZChi2 << std::endl;
  //		return false;
  //	}
  //	if (fabs(szDist) > maxSZDist){
  //		if (fVerbose > 1) std::cout << "SZ Dist too big! Cut at: " << maxSZDist << std::endl;
  //		return false;
  //	}
  return true;
}

bool PndMvdSttGemRiemannTrackFinder::CheckRiemannHitGem(PndRiemannTrack *track, PndRiemannHit *hit, FairHit *fairHit)
{
  double dist = track->dist(hit);
  double maxDist = 2;
  double szDist = track->szDist(hit);
  double maxSZDist = 10;
  double szChi2 = track->calcSZChi2(hit);
  double maxSZChi2 = 25;
  // double r = track->r(); //[R.K. 01/2017] unused variable?
  // double dip=track->dip(); //[R.K. 01/2017] unused variable?
  // bool sign; //[R.K.02/2017] Unused variable?
  // if ((track->getHit(1)->z())>0 ) //[R.K.02/2017] Unused variable?
  // sign=true; //[R.K.02/2017] Unused variable?
  // else sign=false; //[R.K.02/2017] Unused variable?
  if (fVerbose > 1)
    std::cout << "Checking GemHit: " << fairHit->GetEntryNr() << std::endl;
  if (fVerbose > 1)
    std::cout << ": dist " << dist << " szDist " << szDist << " szChi2 " << szChi2 << std::endl;

  if (fabs(dist) > maxDist) {
    if (fVerbose > 1)
      std::cout << "dist larger than " << maxDist << std::endl;
    return false;
  }

  if (szChi2 > maxSZChi2) {
    if (fVerbose > 1)
      std::cout << " SZ Chi2 too big! Cut at: " << maxSZChi2 << std::endl;
    return false;
  }
  if (fabs(szDist) > maxSZDist) {
    if (fVerbose > 1)
      std::cout << "SZ Dist too big! Cut at: " << maxSZDist << std::endl;
    return false;
  }
  return true;
}

bool PndMvdSttGemRiemannTrackFinder::CheckRiemannHitMvd(PndRiemannTrack *track, PndRiemannHit *hit, FairHit *) // fairHit //[R.K.03/2017] unused variable(s)
{
  // double oldSZChi2 = track->szChi2(); //[R.K. 01/2017] unused variable?
  double dist = track->dist(hit);
  double szDist = track->szDist(hit);
  double szChi2 = track->calcSZChi2(hit);
  double r = track->r();
  double dip = track->dip();
  bool sign;
  if ((track->getHit(1)->z()) > 0)
    sign = true;
  else
    sign = false;
  if (fVerbose > 1)
    std::cout << ": dist " << dist << " szDist " << szDist << " szChi2 " << szChi2 << std::endl;

  if (fabs(dist) > GetMaxPlaneDist(r, dip, sign)) {
    if (fVerbose > 1)
      std::cout << "dist larger than " << GetMaxPlaneDist(r, dip, sign) << std::endl;
    return false;
  }

  //	if (fVerbose > 1) std::cout << "CheckRiemannHitMvd oldSZChi2 " << oldSZChi2 << " new SZChi2 " << szChi2 << std::endl;

  //	if (oldSZChi2 < szChi2){
  //		if (fVerbose > 1) std::cout << " SZ Chi2 " << szChi2 << " bigger than before!  " << oldSZChi2 << std::endl;
  //		return false;
  //	}

  //	if (szChi2 > GetMaxSZChi2(r,dip,sign)){
  //		if (fVerbose > 1) std::cout << " SZ Chi2 too big! Cut at: " << GetMaxSZChi2(r,dip,sign) << std::endl;
  //		return false;
  //	}
  if (fabs(szDist) > fMaxSZDist) {
    if (fVerbose > 1)
      std::cout << "SZ Dist too big! Cut at: " << fMaxSZDist << std::endl;
    return false;
  }
  return true;
}

bool PndMvdSttGemRiemannTrackFinder::CheckBoarderHitsStt(PndTrackCand *track)
{
  FairMultiLinkedData sttHitsInTrack = track->GetLinksWithType(FairRootManager::Instance()->GetBranchId("STTHit"));

  for (int i = 0; i < sttHitsInTrack.GetNLinks(); i++) {
    PndSttHit *mySttHit = (PndSttHit *)fHits[fMapIDtoHit[sttHitsInTrack.GetLink(i)]];
    if (fVerbose > 1)
      LOG(info) << " PndMvdSttGemRiemannTrackFinder::CheckBoarderHitsStt Stt Index: " << mySttHit->GetTubeID();
    if (fStrawMap.GetRow(mySttHit->GetTubeID()) == 0)
      return true;
  }
  return false;
}

int PndMvdSttGemRiemannTrackFinder::GetStrawSector(PndRiemannTrack &track)
{
  double radiusSq(.0), oldRadiusSq(.0);
  int indexHitMaxRadius(-1);

  std::vector<PndRiemannHit> riemannHits = track.getHits();

  for (size_t i = 0; i < riemannHits.size(); i++) {
    FairLink hitLink = fMapHitToID[riemannHits[i].hitID()];
    if (fVerbose > 1)
      std::cout << "HitID Track : " << i << " : " << riemannHits[i].hitID() << " " << hitLink << std::endl;
    int branchId = hitLink.GetType();
    FairRootManager *man = FairRootManager::Instance();
    if (branchId == man->GetBranchId("MVDHitsStrip") || branchId == man->GetBranchId("MVDHitsPixel")) {
      FairHit *FairHit = (PndSdsHit *)fHits[fMapIDtoHit[hitLink]];
      radiusSq = FairHit->GetX() * FairHit->GetX() + FairHit->GetY() * FairHit->GetY();
      if (fVerbose > 1)
        LOG(info) << " PndMvdSttGemRiemannTrackFinder::GetStrawSector FairHit: " << i << " : " << radiusSq << " " << *FairHit;

      if (radiusSq > oldRadiusSq) {
        oldRadiusSq = radiusSq;
        indexHitMaxRadius = fMapIDtoHit[hitLink];
      }
    }
  }
  if (indexHitMaxRadius > -1) {
    if (fVerbose > 1)
      LOG(info) << " PndMvdSttGemRiemannTrackFinder::GetStrawSector IndexHitMaxRadius: " << indexHitMaxRadius << " Link: " << fMapHitToID[indexHitMaxRadius];
    FairHit *FairHit = (PndSdsHit *)fHits[indexHitMaxRadius];
    TVector3 hitPos(FairHit->GetX(), FairHit->GetY(), FairHit->GetZ());
    if (fVerbose > 1)
      LOG(info) << " PndMvdSttGemRiemannTrackFinder::GetStrawSector Phi: " << hitPos.Phi() << " PhiSector: " << fStrawMap.FindPhiSector(hitPos.Phi());
    return fStrawMap.FindPhiSector(hitPos.Phi());
  }
  return -1;
}

std::set<int> PndMvdSttGemRiemannTrackFinder::GetTooCloseHitsInLayer(int LayerNumber, int HitNumber)
{
  std::set<int> result;
  int testN;
  for (unsigned int i = 0; i < fLayers[LayerNumber].size(); i++) {
    testN = fLayers[LayerNumber][i];
    if (CheckHitDistance(HitNumber, testN) != true) {
      if (testN != HitNumber) {
        result.insert(testN);
      }
    }
  }
  return result;
}
////////////////////////////////////
double PndMvdSttGemRiemannTrackFinder::GetMaxPlaneDist(double radius, double dip, bool sign)
{
  double Pt = ((radius / 100) * 2 * 3 * 1E8) / 1E9;
  double Theta;
  if (fabs(TMath::ACos(dip)) < 1E-100) {
    Theta = 0;
  } else {
    if (sign)
      Theta = (TMath::ATan(TMath::Power(TMath::Tan(TMath::ACos(dip)), -1))) * 180 / TMath::Pi(); /// calc Theta from dip
    else
      Theta = (TMath::Pi() - TMath::ATan(TMath::Power(TMath::Tan(TMath::ACos(dip)), -1))) * 180 / TMath::Pi();
  }

  if (fCutDistH != nullptr) {
    double minPt = fCutDistH->GetXaxis()->GetXmin();
    double maxPt = fCutDistH->GetXaxis()->GetXmax();
    double minTh = fCutDistH->GetYaxis()->GetXmin();
    double maxTh = fCutDistH->GetYaxis()->GetXmax();

    int binPt = int(floor((Pt - minPt) * fCutDistH->GetXaxis()->GetNbins() / (maxPt - minPt))) + 1;
    int binTh = int(floor((Theta - minTh) * fCutDistH->GetYaxis()->GetNbins() / (maxTh - minTh))) + 1;

    if (binPt < 1)
      binPt = 1;
    if (binPt > fCutDistH->GetXaxis()->GetNbins())
      binPt = fCutDistH->GetXaxis()->GetNbins();
    if (binTh < 1)
      binTh = 1;
    if (binTh > fCutDistH->GetYaxis()->GetNbins())
      binTh = fCutDistH->GetYaxis()->GetNbins();
    return fCutDistH->GetBinContent(binPt, binTh);

  } else
    return fMaxPlaneDist;
}

double PndMvdSttGemRiemannTrackFinder::GetMaxSZChi2(double radius, double dip, bool sign)
{
  double Pt = ((radius / 100) * 2 * 3 * 1E8) / 1E9;
  double Theta;
  if (fabs(TMath::ACos(dip)) < 1E-100) {
    Theta = 0;
  } else {
    if (sign)
      Theta = (TMath::ATan(TMath::Power(TMath::Tan(TMath::ACos(dip)), -1))) * 180 / TMath::Pi(); /// calc Theta from dip
    else
      Theta = (TMath::Pi() - TMath::ATan(TMath::Power(TMath::Tan(TMath::ACos(dip)), -1))) * 180 / TMath::Pi();
  }
  if (fCutChi2H != nullptr) {
    double minPt = fCutChi2H->GetXaxis()->GetXmin();
    double maxPt = fCutChi2H->GetXaxis()->GetXmax();
    double minTh = fCutChi2H->GetYaxis()->GetXmin();
    double maxTh = fCutChi2H->GetYaxis()->GetXmax();

    int binPt = int(floor((Pt - minPt) * fCutChi2H->GetXaxis()->GetNbins() / (maxPt - minPt))) + 1;
    int binTh = int(floor((Theta - minTh) * fCutChi2H->GetYaxis()->GetNbins() / (maxTh - minTh))) + 1;

    if (binPt < 1)
      binPt = 1;
    if (binPt > fCutChi2H->GetXaxis()->GetNbins())
      binPt = fCutChi2H->GetXaxis()->GetNbins();
    if (binTh < 1)
      binTh = 1;
    if (binTh > fCutChi2H->GetYaxis()->GetNbins())
      binTh = fCutChi2H->GetYaxis()->GetNbins();
    return fCutChi2H->GetBinContent(binPt, binTh);

  } else
    return fMaxSZChi2;
}
//////////////////////////////
