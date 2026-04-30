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
 * PndSttCATrackletGenerator.h
 *
 *  Created on: Jun 13, 2014
 *      Author: schumann
 */

#ifndef PNDSTTCATRACKLETGENERATOR_H_
#define PNDSTTCATRACKLETGENERATOR_H_

#include "PndSttCAData.h"
#include "PndTrackCand.h"
#include "PndTrack.h"
#include "FairLink.h"
#include "MacrosForGPUComputing.h"
#include "PndSttCellTrackletGenerator.h"
#include "PndStt2GeoHandler.h"

//class PndSttStrawMap;
class FairHit;
class PndSttSkewedHit;

extern "C" int *EvaluateAllStates(int *, int *, int, int, int *);

/*
struct TrackletInf_t {

  TrackletInf_t() :
      numSkewed(0), startID(0), endID(0), maxID(0), straight(false), error(
          0.0), numErrHits(0) {
  }
  ;
  std::vector<int> hitIDs; // vector<hit-indices of STTHits of the tracklet>
  int numSkewed;					// number of skewed tubes
  int startID;					// tube-ID of the first tube of the tracklet
  int endID;						// tube-ID of the final tube of the tracklet
  int maxID;						// max tube-ID of all hits of the tracklet
  bool straight;// indicates whether the tracklet runs straight from the center to border of the STT
  double error;					// sum of squared error
  int numErrHits;	// number of hits that deviate from circle by more than radius of a straw tube

  void Print() {
    std::cout << "startId: " << startID << ", endId: " << endID
        << ", maxId: " << maxID << ", straight: " << straight
        << ", #hits: " << hitIDs.size() << ", numSkewed: " << numSkewed
        << std::endl;
  }
  ;
};

struct Combination_t {
  std::set<int> tracklets; 			// status of combined tracklets
  TrackletInf_t trackletInf;		// information about the resulting tracklet

  void Print() {
    std::cout << "combined tracklets: ";
    for (std::set<int>::iterator it = tracklets.begin();
        it != tracklets.end(); ++it) {
      std::cout << *it << ", ";
    }
    std::cout << std::endl;
    trackletInf.Print();
  }
};
*/
class PndSttCATrackletGenerator {
 public:
  PndSttCATrackletGenerator(const PndSttCAData *data)
    : fTimeStamps(20), fBz(2.), fTUBE_RADIUS(0.5005), fDev_tubeNeighborings(nullptr), fHits(data->GetHits()), fCombinedSkewedHits(data->GetCombinedSkewedHits()),
      fSttGeoH(data->GetSttGeoH()), fMapTubeIdToHit(data->GetMapTubeIdToHit()), fMapTubeIdToPos(data->GetMapTubeIdToPos()), fMapHitToFairLink(data->GetMapHitToFairLink()),
      //fStrawMap(data->GetStrawMap()), fMapTubeIdToHit(data->GetMapTubeIdToHit()), fMapTubeIdToPos(data->GetMapTubeIdToPos()), fMapHitToFairLink(data->GetMapHitToFairLink()),
      fHitNeighbors(data->GetHitNeighbors()), fSeparations(data->GetSeparations()), fUseGPU(false)
  {
  }

  virtual ~PndSttCATrackletGenerator() {}

  void SetDevTubeNeighboringsPointer(int *dev_pointer) { fDev_tubeNeighborings = dev_pointer; }

  void SetUseGPU(Bool_t val) { fUseGPU = val; }

  void FindTracks();

  /* For refitting all RiemanTracks with correctedHits (if available)*/
  void RefitTracks();

  void PrintInfo();

  int GetNumPrimaryTracklets() { return fStartTracklets.size(); }

  /* Get TrackCands of start-tracklets (before combination)*/
  std::vector<PndTrackCand> GetFirstTrackCands() { return fFirstTrackCand; };

  void SetBz(Double_t val) { fBz = val; };

  std::vector<Double_t> GetTimeStamps() { return fTimeStamps; };

  map<int, int> GetStates() const { return fStates; }
  map<int, std::set<int>> GetMultiStates() const { return fMultiStates; }

  /* Method creates the tracklets by the means of a cellular automaton.*/
  void GenerateTracklets();

  void GenerateClusters();

 private:
  std::vector<Double_t> fTimeStamps;

  Double_t fBz;
  double fTUBE_RADIUS;

  int *fDev_tubeNeighborings;
  bool fUseGPU;

  std::vector<FairHit *> fHits;
  std::multimap<int, PndSttSkewedHit *> fCombinedSkewedHits; //<(inner) Tube-ID of combined stt hits of skewed layers, corresponding hit>

  //const PndSttStrawMap *fStrawMap = nullptr;
  PndStt2GeoHandler *fSttGeoH;
  std::map<int, int> fMapTubeIdToHit;
  std::map<int, TVector3> fMapTubeIdToPos;
  std::map<int, FairLink> fMapHitToFairLink;

  map<int, vector<int>> fHitNeighbors;
  map<int, vector<int>> fSeparations;

  map<int, int> fStates;                // map<straw id, state id>
  map<int, std::set<int>> fMultiStates; // map<straw id, set of neighboring state ids for straws with more than two neighbors

  map<int, TrackletInf_t> fStartTracklets; // map<state of start-tracklets (with more than 2 hits) generated by cellular automaton, TrackletInf>
  map<int, TrackletInf_t> fShortTracklets; // set<state of tracklets with less than 3 hits>

  // for first step of trackfinding
  std::vector<PndTrackCand> fFirstTrackCand; // for saving trackCands after the use of cellular automaton

  // for second step of trackfinding
  std::vector<std::set<int>> fStateCombinations; // vector< set<state of start-tracklets that should be combined> >
  std::vector<Combination_t> fCombinedData;      // for storing combination of start-tracklets
  std::vector<int> fTrackletsWithoutCombi;       // state of tracklets that were not combined

  /* Method creates the tracklets by the means of a cellular automaton on the GPU.*/
  void GenerateTrackletsGPU();

  /* Method update the states of each until no state change anymore.*/
  void EvaluateState();

  /* Method update the states of each until no state change anymore. Generalization of EvaluateState()*/
  void EvaluateState(int nNeighbors);

  /* Method update the states of tubes with more than two neighbors until no state changes anymore. */
  void EvaluateMultiState();

  /* Method initialzises fStartTracklets with the states and trackletInf
   * of the generated tracklets.*/
  void InitStartTracklets();

  /* Method inserts a combination of states if not existing yet.*/
  void InsertCombination(std::set<int> combination);

  /* Method for calculating the trackletInf for a combination of tracklets.*/
  TrackletInf_t GetTrackletInf(std::set<int> tracklets);

  /* Method splits the start tracklets in uncombined and short tracklets*/
  void SplitData();

  /* Method adds the unassigned hits and trackCands with 1 and 2 hits to an
   * appropriate combination (if possible).*/
  void AddRemainingHits();

  /* Methods adds the uncared hits with 3 and 4 hit-neighbors to the best combination
   * of tracklets. If the nearest riemann-circle is found and the distance is
   * smaller than the radius of a tube, the hit is added.*/
  bool AddHitToBestCombi(int hitID);

  std::set<std::pair<int, int>> CreatePairCombis(int firstState, std::set<int> values);

  /* Method checks if a tubeID belongs to the end-tube of a tracklet.*/
  bool IsEndTubeOfTracklet(int tubeID);

  ClassDef(PndSttCATrackletGenerator, 1);
};

#endif /* PNDSTTCATRACKLETGENERATOR_H_ */
