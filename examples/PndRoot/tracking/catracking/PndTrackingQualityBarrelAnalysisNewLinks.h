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
 * PndTrackingQualityBarrelAnalysisNewLinks.h
 *
 *  Created on: Aug 23, 2013
 *      Author: stockman
 */

#ifndef PndTrackingQualityBarrelAnalysisNewLinks_H_
#define PndTrackingQualityBarrelAnalysisNewLinks_H_

#include "FairMultiLinkedData.h"
#include "FairRootManager.h"

#include "PndTrackCand.h"
#include "PndTrackingQualityRecoInfo.h"

#include <TObject.h>
#include <TString.h>
#include <TClonesArray.h>

#include <vector>
#include <map>

#include <functional>
#include "PndTrackingQualityAnalysis.h"

/*class PossibleTrackFunctor : public std::binary_function<FairMultiLinkedData* , Bool_t, Bool_t>
{
  public :
    virtual Bool_t operator() (FairMultiLinkedData* a, Bool_t primary) {return Call(a, primary);};
    virtual Bool_t Call(FairMultiLinkedData* a, Bool_t primary) = 0;
    virtual void Print() = 0;

    virtual ~PossibleTrackFunctor() {};

};

class StandardTrackFunctor : public PossibleTrackFunctor
{
  Bool_t Call(FairMultiLinkedData* a, Bool_t primary){
    FairRootManager* ioman = FairRootManager::Instance();
    Bool_t possibleTrack = kFALSE;
    possibleTrack = possibleTrack | (a->GetLinksWithType(ioman->GetBranchId("MVDHitsPixel")).GetNLinks() +
                     a->GetLinksWithType(ioman->GetBranchId("MVDHitsStrip")).GetNLinks() > 3);

    possibleTrack = possibleTrack | (a->GetLinksWithType(ioman->GetBranchId("MVDHitsPixel")).GetNLinks() +
                     a->GetLinksWithType(ioman->GetBranchId("MVDHitsStrip")).GetNLinks() +
                     a->GetLinksWithType(ioman->GetBranchId("STTHit")).GetNLinks() ) > 5;

    return possibleTrack;
  }

  void Print(){
    std::cout << "StandardTrackFunctor: > 3 Hits in MVD or > 5 Hits in (MVD+Stt)" << std::endl;
  }
};

class OnlySttFunctor : public PossibleTrackFunctor
{
  Bool_t Call(FairMultiLinkedData* a, Bool_t primary){
    FairRootManager* ioman = FairRootManager::Instance();
    Bool_t possibleTrack = kFALSE;

    possibleTrack = possibleTrack | a->GetLinksWithType(ioman->GetBranchId("STTHit")).GetNLinks() > 5;

    return possibleTrack;
  }
  void Print(){
    std::cout << "OnlySttFunctor: > 5 Hits in Stt" << std::endl;
  }
};

class RiemannMvdSttGemFunctor : public PossibleTrackFunctor
{
  Bool_t Call(FairMultiLinkedData* a, Bool_t primary){
    FairRootManager* ioman = FairRootManager::Instance();
    Bool_t possibleTrack = kFALSE;
    Bool_t mvdHits =  ((a->GetLinksWithType(ioman->GetBranchId("MVDHitsPixel")).GetNLinks() +
              a->GetLinksWithType(ioman->GetBranchId("MVDHitsStrip")).GetNLinks()) > 2);

    if (mvdHits){
      possibleTrack = a->GetLinksWithType(ioman->GetBranchId("STTHit")).GetNLinks() > 1 | a->GetLinksWithType(ioman->GetBranchId("GEMHit")).GetNLinks() > 1;
    }
    return possibleTrack;
  }
  void Print(){
    std::cout << "RiemannMvdSttGemFunctor: > 2 Hits in MVD and >0 Hits in (Stt+Gem)" << std::endl;
  }

};

class CircleHoughTrackFunctor : public PossibleTrackFunctor
{
  Bool_t Call(FairMultiLinkedData* a, Bool_t primary){
    if (primary == kFALSE) return kFALSE;
    FairRootManager* ioman = FairRootManager::Instance();
    Bool_t possibleTrack = kFALSE;

    Int_t nHitsMvdPixel = a->GetLinksWithType(ioman->GetBranchId("MVDHitsPixel")).GetNLinks();
    Int_t nHitsMvdStrip = a->GetLinksWithType(ioman->GetBranchId("MVDHitsStrip")).GetNLinks();
    if (nHitsMvdPixel + nHitsMvdStrip > 2) {  // First requirement: more than two MVD hits
      Int_t nHitsStt = a->GetLinksWithType(ioman->GetBranchId("STTHit")).GetNLinks();
      Int_t nHitsGem = a->GetLinksWithType(ioman->GetBranchId("GEMHit")).GetNLinks();
      possibleTrack = (nHitsMvdPixel + nHitsMvdStrip + nHitsStt + nHitsGem > 6);  // Second requirement: More than six hits total
    }
    return possibleTrack;
  }
  void Print(){
    std::cout << "CircleHoughTrackFunctor: > 3 Hits in MVD and primary track" << std::endl;
  }

};
*/

/**
 * @brief Holding statically callable quality numbers
 * @details Per event, a track can have a certain quality. On a MC level, it can be below a threshold to be even found. If found, it can be found fully, or partially.
 * The struct holds the identifiers needed to categorize tracks.
 * The rough idea is:
 * Positive numbers are referring to reconstructed tracks, negative numbers to track before reconstruction.
 * The five numbers from -5 to -1 are repeating from -11 to -7 and there referring to monte carlo data.
 *
 */

/*
struct qualityNumbers {
  static const int
    // Following: Default statuses.
    // Are the 'not found' tracks in the quality histogram of PndTrackingQualityTask.
    kPossibleSec = -1,  // possible: As defined through the possibleFunctor; secondary: a non-primary particle
    kPossiblePrim = -2,  // possible: As defined through the possibleFunctor; primary: coming directly from particle generator (e.g. EvtGen)
    kAtLeastThreeSec = -3,  // atLeastThree: min. 3 hit points in central tracking detectors (MVD, STT, GEM); secondary: a non-primary particle
    kAtLeastThreePrim = -4,  // atLeastThree: min. 3 hit points in central tracking detectors (MVD, STT, GEM); primary: coming directly from particle generator (e.g. EvtGen)
    kLessThanThreePrim = -5,  // LessThanThree: fewer than 3 hit points in central tracking detectors (MVD, STT, GEM); primar: coming directly from particle generator (e.g. EvtGen)

    // Following: MC statuses of all (found+notfound) tracks
    kMcPossibleSec = -7,  // see above
    kMcPossiblePrim = -8,
    kMcAtLeastThreeSec = -9,
    kMcAtLeastThreePrim = -10,
    kMcLessThanThreePrim = -11,

    // Following: Status of reconstructed tracks (= created PndTracks)
    kSpuriousFound = 1,  // spuriousFound: at least 70% of hits of reco'd track come from one MC track ('mostProbableTrack')
    kPartiallyFound = 2,  // partiallyFound: all hits of reco'd track come from one single MC track; at least 70% of hits of MC track have been found in reco'd track
    kFullyFound = 3,  // fullyFound: all hits of reco'd track come from one single MC track; all hits of MC track have been found in reco'd track

    kGhost = 5,  // ghost: less than 70% of hits of reco'd track come from one MC track ('mostProbableTrack')

    kNotFound = 7,  // notFound: total number of not reco'd tracks
    kFound = 8;  // found: total number of reco'd tracks; the sum of fullyFound, partiallyFound, spuriousFound
};

*/

class PndTrackingQualityBarrelAnalysisNewLinks : public TObject {
 public:
  PndTrackingQualityBarrelAnalysisNewLinks(TString trackBranchName, TString idealTrackName, Bool_t pndTrackData = kTRUE);
  PndTrackingQualityBarrelAnalysisNewLinks(TString trackBranchName, TString idealTrackName, PossibleTrackFunctor *posTrack, Bool_t pndTrackData = kTRUE);
  virtual ~PndTrackingQualityBarrelAnalysisNewLinks();

  virtual void Init();
  void SetVerbose(Int_t val) { fVerbose = val; }

  //! Adds branch names of detector data which should be taken into account in the analysis
  void AddHitsBranchName(TString name) { fBranchNames.push_back(name); }
  void SetHitsBranchNames(std::vector<TString> names) { fBranchNames = names; }

  void AnalyseEvent(TClonesArray *recoTrackInfo);

  //	Int_t GetNIdealHits(Int_t trackId, TString branchName);
  Int_t GetNIdealHits(FairMultiLinkedData &track, TString branchName);
  std::map<Int_t, Int_t> GetMCTrackFound() { return fMCTrackFound; }
  std::map<Int_t, Int_t> GetTrackQualification() { return fMapTrackQualification; }
  std::map<Int_t, Int_t> GetTrackMCStatus() { return fMapTrackMCStatus; }
  std::map<Int_t, std::map<TString, std::pair<Double_t, Int_t>>> GetEfficiencies() { return fMapEfficiencies; }
  std::map<Int_t, Double_t> GetPResolution() { return fMapPResolution; }
  std::map<Int_t, TVector3> GetP() { return fMapP; }
  std::map<Int_t, Double_t> GetPtResolution() { return fMapPtResolution; }
  std::map<Int_t, Double_t> GetPt() { return fMapPt; }
  std::map<Int_t, Double_t> GetPResolutionRel() { return fMapPResolutionRel; }
  std::map<Int_t, Double_t> GetPtResolutionRel() { return fMapPtResolutionRel; }
  std::map<Int_t, Int_t> GetTrackIdMCId() { return fTrackIdMCId; }
  Int_t GetNGhosts() { return fNGhosts; }

  void PrintTrackDataSummary(FairMultiLinkedData &trackData, Bool_t detailedInfo = kFALSE);

  /**
   * The description of the quality map IDs is now located a but further up, at struct qualityNumbers
   */
  void PrintTrackQualityMap(Bool_t detailedInfo = kFALSE);
  void PrintTrackMCStatusMap();
  void PrintTrackInfo(std::map<TString, FairMultiLinkedData> info);

  Int_t GetIdealTrackIdFromMCTrackId(int mctrackid) { return fMCIdIdealTrackId[mctrackid]; }
  Int_t GetIdealTrackIdFromRecoTrackId(int trackid)
  {
    int mctrackid = fTrackIdMCId[trackid];
    return fMCIdIdealTrackId[mctrackid];
  }

  PndTrackingQualityRecoInfo GetRecoInfoFromRecoTrack(Int_t trackId, Int_t mctrackId);

  static Bool_t IsBarrelMVD(FairMultiLinkedData &links, int iHit);
  static Int_t NBarrelMVD(FairMultiLinkedData &links);

 private:
  virtual void FillMapTrackQualifikation();
  Bool_t IsBetterTrackExisting(Int_t &mcIndex, int quality);
  //	virtual Bool_t PossibleTrack(FairMultiLinkedData& mcForward);
  Int_t GetSumOfAllValidMCHits(FairMultiLinkedData *trackData);
  virtual Int_t AnalyseTrackInfo(std::map<TString, FairMultiLinkedData> &trackInfo, Int_t trackId);
  virtual void CalcEfficiencies(Int_t mostProbableTrack, std::map<TString, FairMultiLinkedData> &trackInfo);
  FairMultiLinkedData GetMCInfoForBranch(TString branchName, PndTrackCand *trackCand);
  std::map<TString, FairMultiLinkedData> AnalyseTrackCand(PndTrackCand *trackCand);

  //	virtual Bool_t IsCorrectGemHit(FairLink& gemLink);

  Int_t fVerbose;
  FairRootManager *ioman;
  Int_t fNGhosts;

  TString fTrackBranchName;
  TString fIdealTrackName;
  Bool_t fPndTrackOrTrackCand; // kTRUE if track and kFALSE if track cand
  PossibleTrackFunctor *fPossibleTrack;

  Bool_t fUseCorrectedSkewedHits;

  std::vector<TString> fBranchNames;        //!
  std::map<Int_t, Int_t> fTrackIdMCId;      //!<! map between track id and most probable MC track id
  std::map<Int_t, Int_t> fMCIdTrackId;      //!<! map between MC id and track id
  std::map<Int_t, Int_t> fMCIdIdealTrackId; //!<! map between MC id and ideal track id

  std::map<Int_t, Int_t> fMCTrackFound; //!<! How often was a MC Track (key) found

  std::map<Int_t, Int_t> fMapTrackMCStatus;                                        //!<! TrackId vs TrackStatus from MC
  std::map<Int_t, Int_t> fMapTrackQualification;                                   //!<! TrackId vs TrackStatus after analysis of track finding
  std::map<Int_t, std::map<TString, std::pair<Double_t, Int_t>>> fMapEfficiencies; //!<! MostProbable TrackId, BranchName, Efficiency, #FoundHits / #MCHits, #MCHits
  std::map<Int_t, Double_t> fMapPResolution;                                       //!
  std::map<Int_t, TVector3> fMapP;                                                 //!
  std::map<Int_t, Double_t> fMapPtResolution;                                      //!
  std::map<Int_t, Double_t> fMapPt;                                                //!
  std::map<Int_t, Double_t> fMapPResolutionRel;                                    //!
  std::map<Int_t, Double_t> fMapPtResolutionRel;                                   //!

  TClonesArray *fTrack;
  TClonesArray *fMCTrack;
  TClonesArray *fIdealTrack;
  TClonesArray *fIdealTrackCand;

  ClassDef(PndTrackingQualityBarrelAnalysisNewLinks, 1)
};

#endif /* PNDTRACKINGQUALITY_H_ */
