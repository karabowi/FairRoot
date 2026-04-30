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
 * PndSttCellTrackFinder.h
 *
 *  Created on: May 20, 2014
 *      Author: schumann
 */

#ifndef PndSttCellTrackFinder_H_
#define PndSttCellTrackFinder_H_

#include "PndTrackCand.h"
#include "PndRiemannTrack.h"
#include "PndTrack.h"
#include "FairLink.h"
#include "PndSttCellTrackFinderData.h"
#include "PndSttCellTrackletGenerator.h"
#include "PndSttHitCorrector.h"
#include "PndSttSkewedHit.h"

#include <vector>

class TClonesArray;
class PndStt2GeoHandler;

class PndSttCellTrackFinder {
 public:
  PndSttCellTrackFinder(PndStt2GeoHandler *geoH)
  //PndSttCellTrackFinder(TClonesArray *tubeArray)
    : fVerbose(0), fBz(2.), fClusterTime(250.0), fUseGPU(kFALSE), fRunTimeBased(kFALSE), fDev_tubeNeighborings(nullptr), fCalcWithCorrectedIsochrones(kFALSE),
      fCalcFirstTrackletInf(kFALSE), fTrackFinderData(nullptr), fTrackletGenerator(nullptr), fHitCorrector(nullptr)
  {

    // Generate TrackFinderData-Object
    fTrackFinderData = new PndSttCellTrackFinderData(geoH);
  };

  virtual ~PndSttCellTrackFinder()
  {
    delete fTrackFinderData;
    delete fHitCorrector;
    delete fTrackletGenerator;

    for (size_t i = 0; i < fHits.size(); ++i) {
      delete fHits.at(i);
    }
    for (std::multimap<int, PndSttSkewedHit *>::iterator it = fCombinedSkewedHits.begin(); it != fCombinedSkewedHits.end(); ++it) {
      delete (*it).second;
    }
  }

  void FindTracks();
  // so far only supports STTHits type of data. Uses the name of the Branch to distinguish between normal and Skewed.
  void AddHits(TClonesArray *hits, TString branchName);

  void SetUseGPU(Bool_t val) { fUseGPU = val; }
  void SetRunTimeBased(Bool_t val) { fRunTimeBased = val; }
  void SetClusterTime(double val) { fClusterTime = val; }; // J.R. 20/04-2018
  void SetDevTubeNeighboringsPointer(int *dev_pointer) { fDev_tubeNeighborings = dev_pointer; }

  void SetCalcWithCorrectedIsochrones(Bool_t val) { fCalcWithCorrectedIsochrones = val; }

  PndSttCellTrackFinderData *GetTrackFinderDataObject() { return fTrackFinderData; }

  std::map<Int_t, FairHit *> GetCorrectedIsochrones() { return fHitCorrector->GetCorrectedHits(); }

  int GetNumPrimaryTracklets() { return fTrackletGenerator->GetNumPrimaryTracklets(); }

  // Get TrackCands of start-tracklets before combination
  PndTrackCand GetFirstTrackCand(int i) { return fFirstTrackCand[i]; };

  // Get RiemannTracks before combination of tracklets
  PndRiemannTrack GetFirstRiemannTrack(int i) { return fFirstRiemannTrack[i]; };

  // Get TrackCands of combinated tracklets
  PndTrackCand GetCombiTrackCand(int i) { return fCombiTrackCand[i]; };

  // Get PndTrack of combinated tracklets
  PndTrack GetCombiTrack(int i) { return fCombiTrack[i]; };

  PndRiemannTrack GetCombiRiemannTrack(int i) { return fCombiRiemannTrack[i]; };

  std::vector<std::vector<Double_t>> GetTimeStampsTrackletGen() { return fTimeStampsTrackletGen; };

  std::vector<std::vector<Double_t>> GetTimeStampsGenerateNeighborhoodData() { return fTimeStampsGenerateNeighborhoodData; };

  int NumFirstTrackCands() { return fFirstTrackCand.size(); };

  int NumFirstRiemannTracks() { return fFirstRiemannTrack.size(); };

  int NumCombinedTracks() { return fCombiTrackCand.size(); };

  int NumCombinedRiemannTracks() { return fCombiRiemannTrack.size(); };

  int NumHits() { return fTrackFinderData->GetNumHits(); }

  int NumHitsWithoutDouble() { return fTrackFinderData->GetNumHitsWithoutDouble(); }

  int NumUnambiguousNeighbors() { return fTrackFinderData->GetSeparations()[1].size() + fTrackFinderData->GetSeparations()[2].size(); }

  void SetCalcFirstTrackletInf(Bool_t val) { fCalcFirstTrackletInf = val; };

  void SetVerbose(Int_t val) { fVerbose = val; };

  void SetBz(Double_t val) { fBz = val; };

  void StoreTrackData()
  {
    fFirstTrackCand = fTrackletGenerator->GetFirstTrackCands();

    if (fCalcFirstTrackletInf)
      fFirstRiemannTrack = fTrackletGenerator->GetFirstRiemannTracks();

    fCombiTrackCand = fTrackletGenerator->GetCombiTrackCands();
    fCombiRiemannTrack = fTrackletGenerator->GetCombiRiemannTracks();
    fCombiTrack = fTrackletGenerator->GetCombiTracks();
  };

  void Reset()
  {
    fHits.clear();
    fTrackFinderData->clear();

    fFirstTrackCand.clear();
    fFirstRiemannTrack.clear();

    fCombiTrackCand.clear();
    fCombiTrack.clear();
    fCombiRiemannTrack.clear();

    delete fHitCorrector;
    delete fTrackletGenerator;
  }

 private:
  std::vector<std::vector<Double_t>> fTimeStampsTrackletGen;
  std::vector<std::vector<Double_t>> fTimeStampsGenerateNeighborhoodData;

  Int_t fVerbose;
  Double_t fBz;

  double fClusterTime;

  Bool_t fUseGPU;
  Bool_t fRunTimeBased;
  int *fDev_tubeNeighborings;

  Bool_t fCalcWithCorrectedIsochrones;

  Bool_t fCalcFirstTrackletInf;                              // if true, calculate riemannTracks for start-tracklets
  std::vector<FairHit *> fHits;                              // vector with all hits of the current event
  std::multimap<int, PndSttSkewedHit *> fCombinedSkewedHits; //<(inner) Tube-ID of combined stt hits of skewed layers, corresponding hit>

  std::map<int, FairLink> fMapHitToFairLink; // map< index of hit in fHit, FairLink of SttHit>

  PndSttCellTrackFinderData *fTrackFinderData;
  PndSttCellTrackletGenerator *fTrackletGenerator;
  PndSttHitCorrector *fHitCorrector;

  // for first step of trackfinding
  std::vector<PndTrackCand> fFirstTrackCand;       // for saving trackCands after the use of cellular automaton
  std::vector<PndRiemannTrack> fFirstRiemannTrack; //	for saving + plotting the riemann-tracks after the first step

  std::vector<PndTrackCand> fCombiTrackCand; // resulting tracks of combined tracklets
  std::vector<PndTrack> fCombiTrack;         // resulting PndTrack
  std::vector<PndRiemannTrack> fCombiRiemannTrack;

  ClassDef(PndSttCellTrackFinder, 1);
};

#endif /* PndSttCellTrackFinder_H_ */
