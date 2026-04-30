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
 * PndSttCellTrackletGenerator.h
 *
 *  Created on: Jun 13, 2014
 *      Author: schumann
 */

#ifndef PNDSTTCELLTRACKLETGENERATOR_H_
#define PNDSTTCELLTRACKLETGENERATOR_H_

#include "PndSttCellTrackFinderData.h"
#include "PndTrackCand.h"
#include "PndRiemannTrack.h"
#include "PndTrack.h"
#include "FairLink.h"
#include "MacrosForGPUComputing.h"

//class PndSttStrawMap;
class FairHit;
class PndSttSkewedHit;

using std::map;
using std::vector;


#ifdef RUNCUDA
extern "C" int *EvaluateAllStates(int *, int *, int, int, int *);
#endif

struct TrackletInf_t {

  TrackletInf_t() : numSkewed(0), startID(0), endID(0), maxID(0), straight(false), error(0.0), numErrHits(0){};
  std::vector<int> hitIDs;      // vector<hit-indices of STTHits of the tracklet>
  int numSkewed;                // number of skewed tubes
  int startID;                  // tube-ID of the first tube of the tracklet
  int endID;                    // tube-ID of the final tube of the tracklet
  int maxID;                    // max tube-ID of all hits of the tracklet
  bool straight;                // indicates whether the tracklet runs straight from the center to border of the STT
  PndRiemannTrack riemannTrack; // riemannTrack of the tracklet
  double error;                 // sum of squared error
  int numErrHits;               // number of hits that deviate from circle by more than radius of a straw tube

  void Print()
  {
    std::cout << "startId: " << startID << ", endId: " << endID << ", maxId: " << maxID << ", straight: " << straight << ", #hits: " << hitIDs.size()
              << ", numSkewed: " << numSkewed << std::endl;

    if (riemannTrack.getNumHits() != 0) {
      std::cout << ", RiemannTrack created error: " << error << ", #wrong hits: " << numErrHits << " ";
      // std::cout << riemannTrack;
      std::cout << std::endl;
    }
  };
};

struct Combination_t {
  std::set<int> tracklets;   // status of combined tracklets
  TrackletInf_t trackletInf; // information about the resulting tracklet

  void Print()
  {
    std::cout << "combined tracklets: ";
    for (std::set<int>::iterator it = tracklets.begin(); it != tracklets.end(); ++it) {
      std::cout << *it << ", ";
    }
    std::cout << std::endl;
    trackletInf.Print();
  }
};

class PndSttCellTrackletGenerator {
 public:
  PndSttCellTrackletGenerator(const PndSttCellTrackFinderData *data)
    : fTimeStamps(20), fVerbose(0), fBz(2.), fCalcFirstTrackletInf(false), fCalcWithCorrectedHits(false), fTUBE_RADIUS(0.5005), fUseGPU(false), fDev_tubeNeighborings(nullptr),
      fHits(data->GetHits()), fCombinedSkewedHits(data->GetCombinedSkewedHits()), fSttGeoH(data->GetSttGeoH()), fMapTubeIdToHit(data->GetMapTubeIdToHit()),
      //fHits(data->GetHits()), fCombinedSkewedHits(data->GetCombinedSkewedHits()), fStrawMap(data->GetStrawMap()), fMapTubeIdToHit(data->GetMapTubeIdToHit()),
      fMapTubeIdToPos(data->GetMapTubeIdToPos()), fMapHitToFairLink(data->GetMapHitToFairLink()), fHitNeighbors(data->GetHitNeighbors()), fSeparations(data->GetSeparations())
  {
  }

  virtual ~PndSttCellTrackletGenerator() {}

  void SetUseGPU(Bool_t val) { fUseGPU = val; }

  void SetDevTubeNeighboringsPointer(int *dev_pointer) { fDev_tubeNeighborings = dev_pointer; }

  void FindTracks();

  void SetCorrectedHits(std::map<int, FairHit *> correctedHits);

  /* For refitting all RiemanTracks with correctedHits (if available)*/
  void RefitTracks();

  void PrintInfo();

  void SetCalcWithCorrectedHits(bool calcWithCorrectedHits = true) { fCalcWithCorrectedHits = calcWithCorrectedHits; }

  int GetNumPrimaryTracklets() { return fStartTracklets.size(); }

  /* Get TrackCands of start-tracklets (before combination)*/
  std::vector<PndTrackCand> GetFirstTrackCands() { return fFirstTrackCand; };

  /* Get RiemannTracks before combination of tracklets*/
  std::vector<PndRiemannTrack> GetFirstRiemannTracks() { return fFirstRiemannTrack; };

  /* Get TrackCands of combinated start-tracklets*/
  std::vector<PndTrackCand> GetCombiTrackCands() { return fCombiTrackCand; };

  std::vector<PndTrack> GetCombiTracks() { return fCombiTrack; };

  bool CalcWithCorrectedHits() { return fCalcWithCorrectedHits; }

  /* Get RiemannTracks of combinated tracklets*/
  std::vector<PndRiemannTrack> GetCombiRiemannTracks() { return fCombiRiemannTrack; };

  void SetCalcFirstTrackletInf(Bool_t val) { fCalcFirstTrackletInf = val; };

  void SetVerbose(Int_t val) { fVerbose = val; };

  void SetBz(Double_t val) { fBz = val; };

  std::vector<Double_t> GetTimeStamps() { return fTimeStamps; };

 private:
  std::vector<Double_t> fTimeStamps;

  Int_t fVerbose;
  Double_t fBz;
  bool fCalcFirstTrackletInf;
  bool fCalcWithCorrectedHits;
  double fTUBE_RADIUS;

  bool fUseGPU;
  int *fDev_tubeNeighborings;

  std::vector<FairHit *> fHits;
  std::multimap<int, PndSttSkewedHit *> fCombinedSkewedHits; //<(inner) Tube-ID of combined stt hits of skewed layers, corresponding hit>

  //const PndSttStrawMap *fStrawMap;
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
  std::vector<PndTrackCand> fFirstTrackCand;       // for saving trackCands after the use of cellular automaton
  std::vector<PndRiemannTrack> fFirstRiemannTrack; //	for saving + plotting the riemann-tracks after the first step

  // for second step of trackfinding
  std::vector<std::set<int>> fStateCombinations; // vector< set<state of start-tracklets that should be combined> >
  std::vector<Combination_t> fCombinedData;      // for storing combination of start-tracklets
  std::vector<int> fTrackletsWithoutCombi;       // state of tracklets that were not combined

  std::vector<PndTrackCand> fCombiTrackCand;       // resulting tracks after combination of tracklets (uncombined too)
  std::vector<PndRiemannTrack> fCombiRiemannTrack; // resulting riemanntracks
  std::vector<PndTrack> fCombiTrack;               // resulting PndTrack

  std::map<int, FairHit *> fCorrectedHits;

  /* This Method creates PndTrackCands out of the entries in fCombinedData and
   *  the not combined tracklets with more than 2 hits.*/
  void CreatePndTrackCands();

  /* Method creates the tracklets by the means of a cellular automaton.*/
  void GenerateTracklets();

  /* Method creates the tracklets by the means of a cellular automaton on the GPU.*/
  void GenerateTrackletsGPU();

  /* Method update the states of each until no state change anymore.*/
  void EvaluateState();

  /* Method update the states of tubes with more than two neighbors until no state changes anymore. */
  void EvaluateMultiState();

  /* Method initialzises fStartTracklets with the states and trackletInf
   * of the generated tracklets.*/
  void InitStartTracklets();

  /* Method searches for reasonable combination of the start-tracklets with a recursive algorithm.
   * They are stored in fCombinedData. */
  void CombineTrackletsMultiStages();

  /* Implementation of the recursive algorithm for finding combination of the start-tracklets.*/
  void CombineTrackletsMultiStagesRecursive(int stateToCombine, std::set<int> currentCombi);

  /* Method inserts a combination of states if not existing yet.*/
  void InsertCombination(std::set<int> combination);

  /* Method for calculating the trackletInf for a combination of tracklets.*/
  TrackletInf_t GetTrackletInf(std::set<int> tracklets);

  /* Method splits the start tracklets in uncombined and short tracklets*/
  void SplitData();

  void AssignAmbiguousHits();

  /* Method adds the unassigned hits and trackCands with 1 and 2 hits to an
   * appropriate combination (if possible).*/
  void AddRemainingHits();

  /* Methods adds the uncared hits with 3 and 4 hit-neighbors to the best combination
   * of tracklets. If the nearest riemann-circle is found and the distance is
   * smaller than the radius of a tube, the hit is added.*/
  bool AddHitToBestCombi(int hitID);

  /* Method for creating a riemannTrack out of hits.
   * Hits of skewed tubes were ignored.*/
  PndRiemannTrack CreateRiemannTrack(std::vector<int> hitIDs);

  std::set<std::pair<int, int>> CreatePairCombis(int firstState, std::set<int> values);

  /* Method calculates the mean squared deviation of the hits from the riemann-circle.*/
  double CalcDeviationOfRiemannTrack(PndRiemannTrack &track);

  /* Method calculates the deviation of the hit from the riemann-circle.*/
  double CalcDeviation(PndRiemannTrack &track, int hitID);

  /* Method counts the hits of the riemannTrack, that had a distance of more
   * than r (radius of a straw tube) to the riemann-circle.*/
  int GetDeviationCount(PndRiemannTrack &track);

  /* Method checks if a tubeID belongs to the end-tube of a tracklet.*/
  bool IsEndTubeOfTracklet(int tubeID);

  ClassDef(PndSttCellTrackletGenerator, 1);
};

#endif /* PNDSTTCELLTRACKLETGENERATOR_H_ */
