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
 * PndTrackingQATimebased.h
 *
 *  Created on: Aug 23, 2013
 *      Author: stockman
 */

#ifndef PndTrackingQATimebased_H_
#define PndTrackingQATimebased_H_

#include "FairMultiLinkedData.h"
#include "FairRootManager.h"

#include "PndMCTrack.h"
#include "PndTrack.h"
#include "PndTrackCand.h"
#include "PndTrackingQualityRecoInfo.h"
#include "PndTrackingCloneInfo.h"

#include <TObject.h>
#include <TString.h>
#include <TClonesArray.h>

#include <vector>
#include <map>

#include <functional>
#include "PndTrackFunctor.h"

/**
 * @brief Holding statically callable quality numbers
 * @details Per event, a track can have a certain quality. On a MC level, it can be below a threshold to be even found. If found, it can be found fully, or partially.
 * The struct holds the identifiers needed to categorize tracks.
 * The rough idea is:
 * Positive numbers are referring to reconstructed tracks, negative numbers to track before reconstruction.
 * The five numbers from -5 to -1 are repeating from -11 to -7 and there referring to monte carlo data.
 *
 */

struct qualityNumbers {
  static const int
    // Following: Default statuses.
    // Are the 'not found' tracks in the quality histogram of PndTrackingQualityTask.
    kPossibleSec = -1,       // possible: As defined through the possibleFunctor; secondary: a non-primary particle
    kPossiblePrim = -2,      // possible: As defined through the possibleFunctor; primary: coming directly from particle generator (e.g. EvtGen)
    kAtLeastThreeSec = -3,   // atLeastThree: min. 3 hit points in central tracking detectors (MVD, STT, GEM); secondary: a non-primary particle
    kAtLeastThreePrim = -4,  // atLeastThree: min. 3 hit points in central tracking detectors (MVD, STT, GEM); primary: coming directly from particle generator (e.g. EvtGen)
    kLessThanThreePrim = -5, // LessThanThree: fewer than 3 hit points in central tracking detectors (MVD, STT, GEM); primar: coming directly from particle generator (e.g. EvtGen)

    // Following: MC statuses of all (found+notfound) tracks
    kMcPossibleSec = -7, // see above
    kMcPossiblePrim = -8, kMcAtLeastThreeSec = -9, kMcAtLeastThreePrim = -10, kMcLessThanThreePrim = -11,
    kMcAllTracksWithHits = -12, // sum of -7 - -10
    kMcAllTracks = -13,         // sum of -11 and -12

    // Following: Status of reconstructed tracks (= created PndTracks)
    kPartiallyImpure = 1, // PartiallyImpure: at least 70% of hits of reco'd track come from one MC track ('mostProbableTrack')
    kPartiallyPure = 2,   // PartiallyPure: all hits of reco'd track come from one single MC track; at least 70% of hits of MC track have been found in reco'd track
    kFullyImpure = 3,     // FullyImpure: all hits of MC track have been found in reco'd track but some impurities from other tracks are allowed
    kFullyPure = 4,       // FullyPure: all hits of reco'd track come from one single MC track; all hits of MC track have been found in reco'd track

    kGhost = 5, // ghost: less than 70% of hits of reco'd track come from one MC track ('mostProbableTrack')
    kClone = 6, // clone: sum of (number of times one mc track was found -1) over all mc tracks

    kNotFound = 7, // notFound: total number of not reco'd tracks
    kFound = 8;    // found: total number of reco'd tracks; the sum of FullyPure, FullyImpure, PartiallyPure, PartiallyImpure

  static std::string QualityNumberToString(int qNumber)
  {
    if (qNumber == kPossiblePrim)
      return "PossiblePrimary";
    if (qNumber == kPossibleSec)
      return "PossibleSec";
    if (qNumber == kAtLeastThreeSec)
      return "AtLeastThreeSec";
    if (qNumber == kAtLeastThreePrim)
      return "AtLeastThreePrim";
    if (qNumber == kLessThanThreePrim)
      return "LessThanThreePrim";
    if (qNumber == kMcPossibleSec)
      return "McPossibleSec";
    if (qNumber == kMcAtLeastThreeSec)
      return "McAtLeastThreeSec";
    if (qNumber == kMcAtLeastThreePrim)
      return "McAtLeastThreePrim";
    if (qNumber == kMcLessThanThreePrim)
      return "McLessThanThreePrim";
    if (qNumber == kMcAllTracksWithHits)
      return "McAllTracksWithHits";
    if (qNumber == kMcAllTracks)
      return "McAllTracks";
    if (qNumber == kPartiallyImpure)
      return "PartiallyImpure";
    if (qNumber == kPartiallyPure)
      return "PartiallyPure";
    if (qNumber == kFullyPure)
      return "FullyPure";
    if (qNumber == kFullyImpure)
      return "FullyImpure";
    if (qNumber == kGhost)
      return "Ghost";
    if (qNumber == kClone)
      return "Clone";
    if (qNumber == kNotFound)
      return "NotFound";
    if (qNumber == kFound)
      return "Found";
    return std::to_string(qNumber);
  };
};

class PndTrackingQATimebased : public TObject {
 public:
  PndTrackingQATimebased(TString trackBranchName, TString idealTrackName, Bool_t pndTrackData = kTRUE);
  PndTrackingQATimebased(TString trackBranchName, TString idealTrackName, PndTrackFunctor *posTrack, Bool_t pndTrackData = kTRUE);
  virtual ~PndTrackingQATimebased();

  virtual void Init();
  void SetVerbose(Int_t val) { fVerbose = val; }

  //! Adds branch names of detector data which should be taken into account in the analysis
  void AddHitsBranchName(TString name) { fBranchNames.push_back(name); }
  void SetHitsBranchNames(std::vector<TString> names) { fBranchNames = names; }
  void SetSecondaryDefinitionPCAXY(double dPCA)
  {
    fSecondaryDefinitionPCAXY = kTRUE;
    fdPCA = dPCA;
  }

  void AnalyseEvent(TClonesArray *recoTrackInfo);

  //	Int_t GetNIdealHits(Int_t trackId, TString branchName);
  Int_t GetNIdealHits(FairMultiLinkedData &track, TString branchName);
  std::map<Int_t, Int_t> GetMCTrackFound() { return fMCTrackFound; }
  std::map<FairLink, Int_t> GetMCTrackFoundTimeBased() { return fTimeBasedMCTrackFound; }
  std::map<Int_t, Int_t> GetTrackQualification() { return fMapTrackQualification; }
  std::map<FairLink, Int_t> GetTrackQualificationTimeBased() { return fTimeBasedMapTrackQualification; }
  std::map<Int_t, Int_t> GetTrackMCStatus() { return fMapTrackMCStatus; }
  std::map<FairLink, Int_t> GetTrackMCStatusTimeBased() { return fTimeBasedMapTrackMCStatus; }
  std::map<Int_t, std::map<TString, std::pair<Double_t, Int_t>>> GetEfficiencies() { return fMapEfficiencies; }
  std::map<FairLink, std::map<TString, std::pair<Double_t, Int_t>>> GetEfficienciesTimeBased() { return fMapEfficienciesTimeBased; }

  // Event based
  std::map<Int_t, Double_t> GetPResolution() { return fMapPResolution; }
  std::map<Int_t, TVector3> GetP() { return fMapP; }
  std::map<Int_t, Double_t> GetPtResolution() { return fMapPtResolution; }
  std::map<Int_t, Double_t> GetPt() { return fMapPt; }
  std::map<Int_t, Double_t> GetPlResolution() { return fMapPlResolution; }
  std::map<Int_t, Double_t> GetPl() { return fMapPl; }
  std::map<Int_t, Double_t> GetPResolutionRel() { return fMapPResolutionRel; }
  std::map<Int_t, Double_t> GetPtResolutionRel() { return fMapPtResolutionRel; }
  std::map<Int_t, Double_t> GetPlResolutionRel() { return fMapPlResolutionRel; }

  // Time based
  std::map<FairLink, Double_t> GetTimeBasedPResolution() { return fTimeBasedMapPResolution; }
  std::map<FairLink, TVector3> GetTimeBasedP() { return fTimeBasedMapP; }
  std::map<FairLink, Double_t> GetTimeBasedPtResolution() { return fTimeBasedMapPtResolution; }
  std::map<FairLink, Double_t> GetTimeBasedPt() { return fTimeBasedMapPt; }
  std::map<FairLink, Double_t> GetTimeBasedPlResolution() { return fTimeBasedMapPlResolution; }
  std::map<FairLink, Double_t> GetTimeBasedPl() { return fTimeBasedMapPl; }
  std::map<FairLink, Double_t> GetTimeBasedPResolutionRel() { return fTimeBasedMapPResolutionRel; }
  std::map<FairLink, Double_t> GetTimeBasedPtResolutionRel() { return fTimeBasedMapPtResolutionRel; }
  std::map<FairLink, Double_t> GetTimeBasedPlResolutionRel() { return fTimeBasedMapPlResolutionRel; }

  std::map<Int_t, Int_t> GetTrackIdMCId() { return fTrackIdMCId; }
  Int_t GetNGhosts() { return fNGhosts; }
  Int_t GetNClones() { return fNClones; }

  void PrintTrackDataSummary(FairMultiLinkedData &trackData, Bool_t detailedInfo = kFALSE);
  void PrintTrackDataSummaryCompare(FairMultiLinkedData &recoTrackData, FairMultiLinkedData &idealTrackData);

  /**
   * The description of the quality map IDs is now located a but further up, at struct qualityNumbers
   */
  void PrintTrackQualityMap(Bool_t detailedInfo = kFALSE);
  void PrintTrackMCStatusMap();
  void PrintTrackInfo(std::map<TString, FairMultiLinkedData> info);

  Int_t GetIdealTrackIdFromMCTrackId(int mctrackid)
  {
    if (fMCIdIdealTrackId.count(mctrackid) == 0)
      return -1;
    return fMCIdIdealTrackId[mctrackid];
  }
  // Time based verson
  FairLink GetIdealTrackFairLinkFromMCTrackFairLink(FairLink mctrackFairLink)
  {
    // if (fTimeBasedMCIdIdealTrackId.count(mctrackFairLink) == 0) return -1;
    return fTimeBasedMCIdIdealTrackId[mctrackFairLink];
  }
  Int_t GetIdealTrackIdFromRecoTrackId(int trackid)
  {
    int mctrackid = fTrackIdMCId[trackid];
    if (fMCIdIdealTrackId.count(mctrackid) == 0)
      return -1;
    return fMCIdIdealTrackId[mctrackid];
  }

  void SetRunTimeBased(bool runTimeBased) { fRunTimeBased = runTimeBased; }

  PndTrackingQualityRecoInfo GetRecoInfoFromRecoTrack(Int_t trackId, Int_t mctrackId, FairLink mctrackFairLink);
  PndTrackingQualityRecoInfo GetRecoInfoFromRecoTrackTimeBased(FairLink recoFairLink, Int_t mctrackId, FairLink mctrackFairLink);
  std::map<FairLink, Int_t> GetCloneInfoforMCTrack() { return fTimeBasedMapTrackMCStatus; };

  std::vector<double> GetEventPurityVector() { return fEventPurityVector; }

 private:
  virtual void FillMapTrackQualifikation();
  Bool_t IsBetterTrackExisting(Int_t &mcIndex, int quality);
  //	virtual Bool_t PossibleTrack(FairMultiLinkedData& mcForward);
  Int_t GetSumOfAllValidMCHits(FairMultiLinkedData *trackData);
  // AnalyseTrackInfo used event based, in this case the track id is set to an Int_t
  virtual Int_t AnalyseTrackInfo(std::map<TString, FairMultiLinkedData> &trackInfo, Int_t trackId);
  // Analyse track info used time based, in this case the track id is set to a FairLink
  FairLink AnalyseTrackInfoTimeBased(std::map<TString, FairMultiLinkedData> &trackInfo, FairLink trackId);
  virtual void CalcEfficiencies(Int_t mostProbableTrack, std::map<TString, FairMultiLinkedData> &trackInfo);
  virtual void CalcEfficienciesTimeBased(FairLink mostProbableTrackFairLink, std::map<TString, FairMultiLinkedData> &trackInfo);
  FairMultiLinkedData
  GetMCInfoForBranch(TString branchName, PndTrackCand *trackCand); ///< returns which MCTracks and how often (marked by a FairLink) they were seen by the hits of a PndTrackCand
  std::map<TString, FairMultiLinkedData>
  AnalyseTrackCand(PndTrackCand *trackCand); ///< returns a map<BranchNameOfHits, MCTrackLinks> which returns the FairLinks to MCTracks grouped by hit branches and all
  double CalcPCAXY(PndTrack *track);
  //	virtual Bool_t IsCorrectGemHit(FairLink& gemLink);

  FairRootManager *ioman;
  // Double_t fTotClonesTimeBased;
  TString fTrackBranchName;
  TString fIdealTrackName;
  Bool_t fPndTrackOrTrackCand; // kTRUE if track and kFALSE if track cand
  std::string fPossibleTrackName;
  PndTrackFunctor *fPossibleTrack;
  Bool_t fCleanFunctor;
  Int_t fNGhosts;
  Int_t fNClones;
  Bool_t fUseCorrectedSkewedHits;
  Bool_t fRunTimeBased;
  Int_t fVerbose;
  bool fSecondaryDefinitionPCAXY;
  double fdPCA;

  std::vector<TString> fBranchNames;        //!<! branch names of hits taken into account in the analysis (e.g. MVDHitsPixel, STTHit, ...)
  std::map<Int_t, Int_t> fTrackIdMCId;      //!<! map between track id and most probable MC track id
  std::map<Int_t, Int_t> fMCIdTrackId;      //!<! map between MC id and track id
  std::map<Int_t, Int_t> fMCIdIdealTrackId; //!<! map between MC id and ideal track id

  //////////////// Maps for time based case //////////////////////////////

  std::map<FairLink, FairLink> fTimeBasedTrackIdMCId;      //!<! map between track FairLink and most probable MC track FairLink
  std::map<FairLink, FairLink> fTimeBasedMCIdTrackId;      //!<! map between MC FairLink and track FairLink
  std::map<FairLink, FairLink> fTimeBasedMCIdIdealTrackId; //!<! map between MC FairLink and ideal track FairLink
  FairMultiLinkedData linksMCTrack;

  std::map<Int_t, Int_t> fMCTrackFound;             //!<! How often was a MC Track (key) found
  std::map<FairLink, Int_t> fTimeBasedMCTrackFound; //!<! How often was a MC Track (key) found

  std::map<Int_t, Int_t> fMapTrackMCStatus; //!<! TrackId vs TrackStatus from MC
  std::map<FairLink, Int_t> fTimeBasedMapTrackMCStatus;
  // Used event based
  std::map<Int_t, Int_t> fMapTrackQualification;             //!<! TrackId vs TrackStatus after analysis of track finding, the track id refers to the MC track
  std::map<FairLink, Int_t> fTimeBasedMapTrackQualification; // Used time based, the FairLink is that of the MC track

  std::map<Int_t, std::map<TString, std::pair<Double_t, Int_t>>> fMapEfficiencies;             //!<! MostProbable TrackId, BranchName, Efficiency (#FoundHits / #MCHits), #MCHits
  std::map<FairLink, std::map<TString, std::pair<Double_t, Int_t>>> fMapEfficienciesTimeBased; //!<! MostProbable TrackId, BranchName, Efficiency (#FoundHits / #MCHits), #MCHits

  // Event based maps
  std::map<Int_t, Double_t> fMapPResolution;     //!
  std::map<Int_t, TVector3> fMapP;               //!
  std::map<Int_t, Double_t> fMapPtResolution;    //!
  std::map<Int_t, Double_t> fMapPt;              //!
  std::map<Int_t, Double_t> fMapPResolutionRel;  //!
  std::map<Int_t, Double_t> fMapPtResolutionRel; //!
  std::map<Int_t, Double_t> fMapPlResolution;    //!
  std::map<Int_t, Double_t> fMapPl;              //!
  std::map<Int_t, Double_t> fMapPlResolutionRel; //!

  // Time based maps
  std::map<FairLink, Double_t> fTimeBasedMapPResolution;     //!
  std::map<FairLink, TVector3> fTimeBasedMapP;               //!
  std::map<FairLink, Double_t> fTimeBasedMapPtResolution;    //!
  std::map<FairLink, Double_t> fTimeBasedMapPt;              //!
  std::map<FairLink, Double_t> fTimeBasedMapPResolutionRel;  //!
  std::map<FairLink, Double_t> fTimeBasedMapPtResolutionRel; //!
  std::map<FairLink, Double_t> fTimeBasedMapPlResolution;    //!
  std::map<FairLink, Double_t> fTimeBasedMapPl;              //!
  std::map<FairLink, Double_t> fTimeBasedMapPlResolutionRel; //!

  TClonesArray *fTrack;
  TClonesArray *fMCTrack;
  TClonesArray *fIdealTrack;
  TClonesArray *fTrackCand;
  TClonesArray *fIdealTrackCand;

  double fEventPurity;
  std::vector<double> fEventPurityVector;

  std::map<int, int> fMapHitIdTimesUsedInEvent;
  std::map<int, int> fMapHitIdTimesUsedInTrack;

  ClassDef(PndTrackingQATimebased, 1)
};

#endif /* PNDTRACKINGQUALITY_H_ */
