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
 * PndSttCA.h
 *
 *  Created on: May 20, 2014
 *      Author: schumann
 */

#ifndef PndSttCA_H_
#define PndSttCA_H_

#include "PndTrackCand.h"
#include "PndTrack.h"
#include "FairLink.h"
#include "PndSttCAData.h"
#include "PndSttCATrackletGenerator.h"
#include "PndSttSkewedHit.h"
#include "PndStt2GeoHandler.h"

#include <vector>

class TClonesArray;

class PndSttCA {
 public:
  PndSttCA(PndStt2GeoHandler *geoH) : fBz(2.), fUseGPU(kFALSE), fDev_tubeNeighborings(nullptr), fTrackFinderData(nullptr), fTrackletGenerator(nullptr)
  //PndSttCA(TClonesArray *tubeArray) : fBz(2.), fUseGPU(kFALSE), fDev_tubeNeighborings(nullptr), fTrackFinderData(nullptr), fTrackletGenerator(nullptr)
  {

    // Generate TrackFinderData-Object
    fTrackFinderData = new PndSttCAData(geoH);
  };

  virtual ~PndSttCA()
  {
    delete fTrackFinderData;
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

  void SetDevTubeNeighboringsPointer(int *dev_pointer) { fDev_tubeNeighborings = dev_pointer; }
  void SetUseGPU(Bool_t val) { fUseGPU = val; }

  PndSttCAData *GetTrackFinderDataObject() { return fTrackFinderData; }

  int GetNumPrimaryTracklets() { return fTrackletGenerator->GetNumPrimaryTracklets(); }

  // Get TrackCands of start-tracklets before combination
  PndTrackCand GetFirstTrackCand(int i) { return fFirstTrackCand[i]; };

  std::vector<std::vector<Double_t>> GetTimeStampsTrackletGen() { return fTimeStampsTrackletGen; };

  std::vector<std::vector<Double_t>> GetTimeStampsGenerateNeighborhoodData() { return fTimeStampsGenerateNeighborhoodData; };

  int NumFirstTrackCands() { return fFirstTrackCand.size(); };

  int NumHits() { return fTrackFinderData->GetNumHits(); }

  int NumHitsWithoutDouble() { return fTrackFinderData->GetNumHitsWithoutDouble(); }

  int NumUnambiguousNeighbors() { return fTrackFinderData->GetSeparations()[1].size() + fTrackFinderData->GetSeparations()[2].size(); }

  void SetCalcFirstTrackletInf(Bool_t val) { fCalcFirstTrackletInf = val; };

  void SetBz(Double_t val) { fBz = val; };

  void StoreTrackData() { fFirstTrackCand = fTrackletGenerator->GetFirstTrackCands(); };

  void Reset()
  {
    fHits.clear();
    fTrackFinderData->clear();

    fFirstTrackCand.clear();

    delete fTrackletGenerator;
  }

 private:
  std::vector<std::vector<Double_t>> fTimeStampsTrackletGen;
  std::vector<std::vector<Double_t>> fTimeStampsGenerateNeighborhoodData;

  Double_t fBz;

  Bool_t fUseGPU;
  int *fDev_tubeNeighborings = nullptr;

  Bool_t fCalcFirstTrackletInf;                              // if true, calculate riemannTracks for start-tracklets
  std::vector<FairHit *> fHits;                              // vector with all hits of the current event
  std::multimap<int, PndSttSkewedHit *> fCombinedSkewedHits; //<(inner) Tube-ID of combined stt hits of skewed layers, corresponding hit>

  std::map<int, FairLink> fMapHitToFairLink; // map< index of hit in fHit, FairLink of SttHit>

  PndSttCAData *fTrackFinderData = nullptr;
  PndSttCATrackletGenerator *fTrackletGenerator = nullptr;

  // for first step of trackfinding
  std::vector<PndTrackCand> fFirstTrackCand; // for saving trackCands after the use of cellular automaton

  ClassDef(PndSttCA, 1);
};

#endif /* PndSttCA_H_ */
