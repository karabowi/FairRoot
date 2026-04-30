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
 * PndSttCATrackletGenerator.cxx
 *
 *  Created on: Jun 13, 2014
 *      Author: schumann
 */

#include "PndSttCATrackletGenerator.h"
#include "FairHit.h"
#include "PndSttHit.h"
//#include "PndSttStrawMap.h"
#include "PndSttSkewedHit.h"
#include "TTimeStamp.h"
#include "FairLogger.h"

ClassImp(PndSttCATrackletGenerator);

using namespace std;

void PndSttCATrackletGenerator::RefitTracks()
{

  LOG(debug1) << "PndSttCATrackletGenerator::RefitTracks()" << endl;

  fFirstTrackCand.clear();

  for (map<int, TrackletInf_t>::iterator trackIt = fStartTracklets.begin(); trackIt != fStartTracklets.end(); ++trackIt) {

    TrackletInf_t trackletInf = trackIt->second;

    PndTrackCand trackCand;
    for (size_t i = 0; i < trackletInf.hitIDs.size(); ++i) {
      // add hits to TrackCand

      trackCand.AddHit(fMapHitToFairLink[trackletInf.hitIDs.at(i)], i);
    }

    fFirstTrackCand.push_back(trackCand);
  }

  // update TrackletInf of combinations --> Hits of RiemannTracks changed
  for (size_t i = 0; i < fCombinedData.size(); i++) {
    fCombinedData[i].trackletInf = GetTrackletInf(fCombinedData[i].tracklets);
  }
}

void PndSttCATrackletGenerator::FindTracks()
{

  // Time stamps for runtime analysis

  // start time for FindTracks()
  fTimeStamps[10] = TTimeStamp();

  // cout << "PndSttCATrackletGenerator::FindTracks()" << endl;
  if (fUseGPU) {
    GenerateTrackletsGPU();
  } else {
    fTimeStamps[6] = TTimeStamp();
    GenerateTracklets();
    fTimeStamps[7] = TTimeStamp();
  }

  // Update trackletInf
  for (size_t i = 0; i < fCombinedData.size(); i++) {
    fCombinedData[i].trackletInf = GetTrackletInf(fCombinedData[i].tracklets);
  }

  fTimeStamps[8] = TTimeStamp();
  SplitData();
  fTimeStamps[9] = TTimeStamp();

  AddRemainingHits();
  fTimeStamps[11] = TTimeStamp();
}

void PndSttCATrackletGenerator::GenerateTrackletsGPU()
{
#ifdef RUNCUDA
  // initialize hits with 0, means no hit for that tube
  int *sttHits = (int *)calloc(NUM_STRAWS, sizeof(int));
  int *hitIndices = (int *)malloc((NUM_STRAWS + 1) * sizeof(int));
  // initialize hitIndices with -1 to signal there's no hit
  //
  for (int i = 0; i < NUM_STRAWS + 1; ++i) {
    hitIndices[i] = -1;
  }

  // form hits into necessary data structure
  set<int> skewedHits;
  set<int> unskewedHits;
  PndSttHit *sttHit;
  int tubeID;

  // fill sets with hits of skewed and unskewed tubes --> no more multiple hits per tube
  for (size_t i = 0; i < fHits.size(); ++i) {
    sttHit = (PndSttHit *)fHits[i];
    tubeID = sttHit->GetTubeID();

    if (tubeID >= START_TUBE_ID_SKEWED && tubeID <= END_TUBE_ID_SKEWED) {
      skewedHits.insert(tubeID);
    } else {
      unskewedHits.insert(tubeID);
    }
  }

  // fill sttHits at first with unskewed hits, set hitIndices
  int indexCounter = 0;
  for (set<int>::iterator it = unskewedHits.begin(); it != unskewedHits.end(); ++it) {
    sttHits[indexCounter] = *it;
    hitIndices[*it] = indexCounter;
    ++indexCounter;
  }

  for (set<int>::iterator it = skewedHits.begin(); it != skewedHits.end(); ++it) {
    sttHits[indexCounter] = *it;
    hitIndices[*it] = indexCounter;
    ++indexCounter;
  }

  int *states = EvaluateAllStates(fDev_tubeNeighborings, sttHits, unskewedHits.size(), skewedHits.size(), hitIndices);

  // extract result of gpu and store it in fStates and fMultiStates
  int numSttHits = unskewedHits.size() + skewedHits.size();

  for (int i = 0; i < numSttHits; ++i) {
    if (states[i] != NUM_STRAWS + 1) {
      fStates[sttHits[i]] = states[i];
    }
  }

  InitStartTracklets();

  LOG(debug1) << "#Start-Tracklets: " << fStartTracklets.size() << endl;

  LOG(info) << "Tracklet-Information: " << endl;
  for (map<int, TrackletInf_t>::iterator it = fStartTracklets.begin(); it != fStartTracklets.end(); ++it) {
    int state = it->first;
    TrackletInf_t inf = it->second;
  }

  LOG(info) << "#Short-Tracklets: " << fShortTracklets.size() << endl;

  LOG(info) << "Tracklet-Information: " << endl;
  for (map<int, TrackletInf_t>::iterator it = fShortTracklets.begin(); it != fShortTracklets.end(); ++it) {
    int state = it->first;
    TrackletInf_t inf = it->second;
  }

  int multiStateOffset = numSttHits;
  int multiStateTubeID;

  for (int i = 0; i < numSttHits; ++i) {

    if (states[i] == NUM_STRAWS + 1) {

      set<int> multiStates;
      for (int j = 0; j < MAX_MULTISTATE_NUM; ++j) {
        if (states[multiStateOffset + j * numSttHits + i] != 0) {
          // insert state not tubeID!
          multiStateTubeID = sttHits[states[multiStateOffset + j * numSttHits + i]];
          multiStates.insert(fStates[multiStateTubeID]);
        } else {
          break;
        }
      }
      if (multiStates.size() != 0)
        fMultiStates[sttHits[i]] = multiStates;
    }
  }

  LOG(debug2) << "MultiStates: " << endl;
  for (map<int, set<int>>::iterator iter = fMultiStates.begin(); iter != fMultiStates.end(); iter++) {
    LOG(debug2) << iter->first << " : ";
    for (set<int>::iterator iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++) {
      LOG(debug2) << *iter2 << " ";
    }
    LOG(debug2) << endl;
  }

  // free memory
  free(sttHits);
  free(hitIndices);
  free(states);
#endif
}

void PndSttCATrackletGenerator::GenerateTracklets()
{

  LOG(debug1) << "PndSttCATrackletGenerator::GenerateTracklets()" << endl;

  // initialize states of cells with the id of the tube
  PndSttHit *sttHit;
  for (size_t i = 0; i < fHits.size(); ++i) {
    sttHit = (PndSttHit *)fHits[i];
    fStates[sttHit->GetTubeID()] = sttHit->GetTubeID();
  }

  fTimeStamps[0] = TTimeStamp();
  EvaluateState();
  fTimeStamps[1] = TTimeStamp();

  LOG(debug1) << "States of start-tracklets: " << endl;
  for (map<int, int>::iterator iter = fStates.begin(); iter != fStates.end(); iter++) {
    LOG(debug1) << iter->first << " : " << iter->second << endl;
  }

  fTimeStamps[2] = TTimeStamp();
  InitStartTracklets();
  fTimeStamps[3] = TTimeStamp();

  LOG(info) << "#Start-Tracklets: " << fStartTracklets.size() << endl;

  LOG(info) << "Tracklet-Information: " << endl;
  for (map<int, TrackletInf_t>::iterator it = fStartTracklets.begin(); it != fStartTracklets.end(); ++it) {
    int state = it->first;
    TrackletInf_t inf = it->second;
  }

  LOG(info) << "#Short-Tracklets: " << fShortTracklets.size() << endl;

  LOG(info) << "Tracklet-Information: " << endl;
  for (map<int, TrackletInf_t>::iterator it = fShortTracklets.begin(); it != fShortTracklets.end(); ++it) {
    int state = it->first;
    TrackletInf_t inf = it->second;
  }

  fTimeStamps[4] = TTimeStamp();
  EvaluateMultiState();
  fTimeStamps[5] = TTimeStamp();

  LOG(debug1) << "MultiStates: " << endl;
  for (map<int, set<int>>::iterator iter = fMultiStates.begin(); iter != fMultiStates.end(); iter++) {
    LOG(debug1) << iter->first << " : ";
    for (set<int>::iterator iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++) {
      LOG(debug1) << *iter2 << " ";
    }
    LOG(debug1) << endl;
  }
}

void PndSttCATrackletGenerator::GenerateClusters()
{
  LOG(debug1) << "PndSttCATrackletGenerator::GenerateClusters()" << endl;

  // initialize states of cells with the id of the tube
  const int MAXIMUM_NUMBER_NEIGHBORS = 8;
  PndSttHit *sttHit;
  fStates.clear();
  for (size_t i = 0; i < fHits.size(); ++i) {
    sttHit = static_cast<PndSttHit *>(fHits[i]);
    fStates[sttHit->GetTubeID()] = sttHit->GetTubeID();
  }

  fTimeStamps[0] = TTimeStamp();
  EvaluateState(MAXIMUM_NUMBER_NEIGHBORS);
  fTimeStamps[1] = TTimeStamp();

  LOG(debug1) << "States of clusters: " << endl;
  for (map<int, int>::iterator iter = fStates.begin(); iter != fStates.end(); iter++) {
    LOG(debug1) << iter->first << " : " << iter->second << endl;
  }
}

void PndSttCATrackletGenerator::EvaluateState()
{

  LOG(debug1) << "PndSttCATrackletGenerator::EvaluateState()" << endl;

  /* Approach: The state has to be calculated for cells with 1 or 2 hit-neighbors (no ambiguities).
   * Each cell starts with an unique state-ID (tubeID). At each step the cells analyzes similar cells
   * (1/2 hit-neighbors) and calculate the minimum of all states. The states will be changed to
   * this minimum simultaneously after the calculation. If there are no changes at consecutive steps,
   * the evaluation of the states is finished. Cells with the same state-ID belongs to the same track
   * and form a tracklet.*/

  int currentSum = 0;
  int priorSum = -1;
  int newState;
  set<int> plainHitIds;
  vector<int>::iterator it;
  map<int, int>::iterator itStates;
  vector<int> neighbors;
  map<int, int> tmpStates;

  // set with tube-ids of hits without ambiguity
  plainHitIds.insert(fSeparations[1].begin(), fSeparations[1].end());
  // plainHitIds.insert(fSeparations[2].begin(), fSeparations[2].end());

  for (auto tube : fSeparations[2]) {
    if (fSttGeoH->GetRow(tube) != 0) {
    //if (fStrawMap->GetRow(tube) != 0) {
      plainHitIds.insert(tube);
    }
  }

  // calculate for tubes with only one or two hits in neighborings until the sum of states does not change anymore
  while (currentSum != priorSum) {

    priorSum = currentSum;
    currentSum = 0;

    // calculate new state of tubes with one hit in neighboring
    for (it = fSeparations[1].begin(); it < fSeparations[1].end(); ++it) {
      neighbors = fHitNeighbors[(*it)];

      // include only state of hits without ambiguity
      if (plainHitIds.find(neighbors[0]) != plainHitIds.end()) {
        newState = min(fStates[(*it)], fStates[neighbors[0]]);
        tmpStates[(*it)] = newState;
        currentSum += newState;
      } else {
        // no hit neighboring to consider
        tmpStates[(*it)] = fStates[(*it)];
        currentSum += newState;
      }
    }
    // calculate new state of tubes with two hits in neighboring
    for (it = fSeparations[2].begin(); it < fSeparations[2].end(); ++it) {

      if (fSttGeoH->GetRow(*it) == 0) {
      //if (fStrawMap->GetRow(*it) == 0) {
        // std::cout << std::endl;
        continue;
      }
      neighbors = fHitNeighbors[(*it)];

      // include only state of hits without ambiguity
      if (plainHitIds.find(neighbors[1]) == plainHitIds.end())
        neighbors.erase(neighbors.begin() + 1);
      if (plainHitIds.find(neighbors[0]) == plainHitIds.end())
        neighbors.erase(neighbors.begin());

      // calculate minimum
      switch (neighbors.size()) {
      case 2: newState = min(fStates[(*it)], min(fStates[neighbors[0]], fStates[neighbors[1]])); break;
      case 1: newState = min(fStates[(*it)], fStates[neighbors[0]]); break;
      case 0: newState = fStates[(*it)];
      }

      tmpStates[(*it)] = newState;
      currentSum += newState;
    }

    // update states simultaneously
    fStates.clear();
    fStates.insert(tmpStates.begin(), tmpStates.end());
  }
}

void PndSttCATrackletGenerator::EvaluateState(int nNeighbors)
{

  LOG(debug1) << "PndSttCATrackletGenerator::EvaluateState(" << nNeighbors << ")" << endl;

  /* Approach: The state has to be calculated for cells with 1 to n hit-neighbors (no ambiguities).
   * Each cell starts with an unique state-ID (tubeID). At each step the cells analyzes similar cells
   * (1/2/n hit-neighbors) and calculate the minimum of all states. The states will be changed to
   * this minimum simultaneously after the calculation. If there are no changes at consecutive steps,
   * the evaluation of the states is finished. Cells with the same state-ID belongs to the same track
   * and form a tracklet.*/

  int currentSum = 0;
  int priorSum = -1;
  int newState = 0;
  set<int> plainHitIds;
  vector<int>::iterator it;
  map<int, int>::iterator itStates;
  vector<int> neighbors;
  map<int, int> tmpStates;

  // set with tube-ids of hits without ambiguity
  for (int i = 0; i < nNeighbors; i++) {
    plainHitIds.insert(fSeparations[i].begin(), fSeparations[i].end());
  }

  // calculate for tubes with only one or two hits in neighborings until the sum of states does not change anymore
  while (currentSum != priorSum) {

    priorSum = currentSum;
    currentSum = 0;

    for (auto hitId : plainHitIds) {
      neighbors = fHitNeighbors[hitId];
      newState = fStates[hitId];
      for (auto neighbor : neighbors) {
        newState = min(fStates[hitId], newState);
        if (plainHitIds.find(neighbor) != plainHitIds.end()) {
          newState = min(newState, fStates[neighbor]);
        }
      }
      tmpStates[hitId] = newState;
      currentSum += newState;
    }

    // update states simultaneously
    fStates.clear();
    fStates.insert(tmpStates.begin(), tmpStates.end());
  }
}

void PndSttCATrackletGenerator::EvaluateMultiState()
{

  LOG(debug1) << "PndSttCATrackletGenerator::EvaluateMultiState()" << endl;

  /* Approach: For unambiguous hit-neighbors the cell value is a copy of all indices of the neighboring cells.
   * The copying is repeated until the values do not change anymore. In this way the cell content is a collection
   * of all unique tracklets touching the cluster of ambiguous cells.*/

  int currentSum = 0;
  int priorSum = -1;
  set<int> newState;
  set<int> ambiguousHitIds;
  set<int>::iterator it;
  map<int, int>::iterator itStates;
  vector<int> neighbors;
  map<int, set<int>> tmpStates;
  map<int, TrackletInf_t>::iterator trackletIt;

  // set with tube-ids of hits with ambiguities
  ambiguousHitIds.insert(fSeparations[3].begin(), fSeparations[3].end());
  ambiguousHitIds.insert(fSeparations[4].begin(), fSeparations[4].end());
  ambiguousHitIds.insert(fSeparations[5].begin(), fSeparations[5].end());
  ambiguousHitIds.insert(fSeparations[6].begin(), fSeparations[6].end());
  ambiguousHitIds.insert(fSeparations[7].begin(), fSeparations[7].end());

  // TODO  generate multistate for short tracklets too?
  //	for (trackletIt = fShortTracklets.begin();
  //			trackletIt != fShortTracklets.end(); ++trackletIt) {
  //
  //		for (int i = 0; i < trackletIt->second.hitIDs.size(); ++i) {
  //			PndSttHit* sttHit = (PndSttHit*) fHits[trackletIt->second.hitIDs[i]];
  //			int tubeID = sttHit->GetTubeID();
  //			ambiguousHitIds.insert(tubeID);
  //		}
  //	}

  LOG(debug1) << "EvaluateMultiState: AmbiguousHitIds: ";
  for (it = ambiguousHitIds.begin(); it != ambiguousHitIds.end(); it++) {
    LOG(debug1) << *it << " ";
  }
  LOG(debug1) << endl;

  int loopcount = 0;
  // while sum of all states changes
  while (currentSum != priorSum) {
    LOG(debug1) << endl;
    LOG(debug1) << "LoopCount: " << loopcount++ << " currentSum: " << currentSum << " priorSum: " << priorSum << endl;

    priorSum = currentSum;
    currentSum = 0;

    for (it = ambiguousHitIds.begin(); it != ambiguousHitIds.end(); ++it) {
      neighbors = fHitNeighbors[(*it)];
      newState.clear();
      LOG(debug1) << "AmbiguousHitId: " << *it << endl;
      LOG(debug1) << "Neighbors: " << endl;

      for (size_t i = 0; i < neighbors.size(); i++) {
        LOG(debug1) << neighbors[i] << " : ";

        if (fStates.count(neighbors[i]) > 0) {
          // if unambiguous neighbor copy one state
          newState.insert(fStates[neighbors[i]]);

          LOG(debug1) << fStates[neighbors[i]];
        } else if (fMultiStates.count(neighbors[i]) > 0) {
          // if multistate-neighbor copy all states
          newState.insert(fMultiStates[neighbors[i]].begin(), fMultiStates[neighbors[i]].end());

          for (set<int>::iterator iter = fMultiStates[neighbors[i]].begin(); iter != fMultiStates[neighbors[i]].end(); iter++) {
            LOG(debug1) << *iter << "/";
          }
        }
        LOG(debug1) << " | ";
      }
      LOG(debug1) << endl;

      // keep new one
      if (newState.size() > 0 && newState.size() > tmpStates[(*it)].size())
        tmpStates[(*it)] = newState;

      LOG(debug1) << "NewState for " << *it << " : ";

      // calculate sum of all states
      for (set<int>::iterator stateIter = newState.begin(); stateIter != newState.end(); stateIter++) {
        LOG(debug1) << *stateIter << " | ";
        currentSum += *stateIter;
      }
      LOG(debug1) << endl;
    }

    // update states simultaneously
    fMultiStates.clear();
    fMultiStates.insert(tmpStates.begin(), tmpStates.end());
  }
}

void PndSttCATrackletGenerator::InitStartTracklets()
{

  LOG(debug1) << "void PndSttCATrackletGenerator::InitStartTracklets()" << endl;

  vector<int>::iterator it;
  map<int, vector<int>> tmpStartTracklets;
  map<int, vector<int>>::iterator trackIt;

  // get states + hitIDs of tracklets with 1 or 2 hit-neighbors
  for (it = fSeparations[1].begin(); it < fSeparations[1].end(); ++it) {
    tmpStartTracklets[fStates[(*it)]].push_back(fMapTubeIdToHit[(*it)]);
  }

  for (it = fSeparations[2].begin(); it < fSeparations[2].end(); ++it) {
    tmpStartTracklets[fStates[(*it)]].push_back(fMapTubeIdToHit[(*it)]);
  }

  vector<int> hitIDs;
  int state;
  PndSttHit *sttHit;
  int tubeID;

  LOG(debug1) << "Calculation of StartTracklets: " << endl;
  for (trackIt = tmpStartTracklets.begin(); trackIt != tmpStartTracklets.end(); ++trackIt) {

    state = trackIt->first;
    TrackletInf_t trackletInf;
    trackletInf.hitIDs.insert(trackletInf.hitIDs.begin(), trackIt->second.begin(), trackIt->second.end());

    hitIDs = trackletInf.hitIDs;
    PndTrackCand trackCand;
    trackletInf.startID = state;
    trackletInf.maxID = 0;
    trackletInf.endID = state;
    trackletInf.straight = false;
    trackletInf.numSkewed = 0;
    bool IsEndID;

    for (size_t i = 0; i < hitIDs.size(); ++i) {

      sttHit = (PndSttHit *)fHits[hitIDs[i]];
      tubeID = sttHit->GetTubeID();

      if (fSttGeoH->IsSkewedStraw(tubeID)) {
      //if (fStrawMap->IsSkewedStraw(tubeID)) {
        ++trackletInf.numSkewed;
      }

      IsEndID = IsEndTubeOfTracklet(tubeID);

      // found tubeID that's bigger than maxID of tracklet?
      if (tubeID > trackletInf.maxID) {

        // if tube is end of tracklet --> straight tracklet
        if (fSttGeoH->GetRow(tubeID) == fSttGeoH->GetRow(trackletInf.maxID)) {
        //if (fStrawMap->GetRow(tubeID) == fStrawMap->GetRow(trackletInf.maxID)) {
          // Hits in same row --> not a straight track
          trackletInf.straight = false;
        }
        if (IsEndID) {
          // no hits in same row and hit in end-tube
          trackletInf.straight = true;
        } else {
          // hit was not in end-tube
          trackletInf.straight = false;
        }

        trackletInf.maxID = tubeID;
      }

      // safe ID of end-tube
      if (IsEndID && tubeID != state) {
        trackletInf.endID = tubeID;
      }

      // add hits to TrackCand
      trackCand.AddHit(fMapHitToFairLink[hitIDs[i]], 0);
    }

    fFirstTrackCand.push_back(trackCand);

    // tracklet with enough hits
    fStartTracklets[state] = trackletInf;
  }
}

void PndSttCATrackletGenerator::InsertCombination(set<int> combination)
{

  LOG(debug1) << "void PndSttCATrackletGenerator::InsertCombination(set<int> combination)" << endl;

  bool found = false;
  set<int>::iterator oldIt;
  set<int>::iterator newIt;

  for (size_t i = 0; i < fStateCombinations.size(); ++i) {

    if (fStateCombinations[i] == combination) {
      found = true;
      break;
    }
  }

  if (!found)
    fStateCombinations.push_back(combination);
}

// es wird davon ausgegangen, dass nur hintereinanderliegende tracklets kombiniert werden
TrackletInf_t PndSttCATrackletGenerator::GetTrackletInf(set<int> tracklets)
{

  LOG(debug1) << "TrackletInf_t PndSttCATrackletGenerator::GetTrackletInf(set<int> tracklets)" << endl;

  TrackletInf_t inf;

  if (tracklets.size() > 1) {
    // update trackletInf

    int state = 5000, maxID = 0; //, endID = 0; //[R.K. 01/2017] unused variable?
    bool straight = false;

    // state of combined tracklet equals min state of all
    for (set<int>::iterator iter = tracklets.begin(); iter != tracklets.end(); ++iter) {
      state = TMath::Min(state, *iter);
    }

    inf.startID = state;

    straight = true;
    // combined tracklet is straight if each particular tracklets are straight
    for (set<int>::iterator iter = tracklets.begin(); iter != tracklets.end(); ++iter) {
      if (!fStartTracklets[*iter].straight) {
        straight = false;
        break;
      }
    }

    inf.straight = straight;

    if (straight) {
      // maxID equals the data of the last tracklet
      maxID = fStartTracklets[*(--tracklets.end())].maxID;

    } else {
      // maxID is the maximum of all maxID
      maxID = 0;
      for (set<int>::iterator iter = tracklets.begin(); iter != tracklets.end(); ++iter) {
        maxID = TMath::Max(maxID, fStartTracklets[*iter].maxID);
      }
    }

    inf.maxID = maxID;

    // endID equals the endID of last tracklet
    inf.endID = fStartTracklets[*(--tracklets.end())].endID;

    // update hitIDs
    for (set<int>::iterator iter = tracklets.begin(); iter != tracklets.end(); ++iter) {
      inf.hitIDs.insert(inf.hitIDs.end(), fStartTracklets[*iter].hitIDs.begin(), fStartTracklets[*iter].hitIDs.end());
      inf.numSkewed += fStartTracklets[*iter].numSkewed;
    }
  }

  return inf;
}

void PndSttCATrackletGenerator::SplitData()
{

  LOG(debug1) << "void PndSttCATrackletGenerator::SplitData" << endl;

  set<int> combinedTracklets;

  // fill set with states of combined tracklets
  for (size_t i = 0; i < fCombinedData.size(); ++i) {
    for (set<int>::iterator iter = fCombinedData[i].tracklets.begin(); iter != fCombinedData[i].tracklets.end(); iter++) {
      combinedTracklets.insert(*iter);
    }
  }

  map<int, TrackletInf_t>::iterator it;
  for (it = fStartTracklets.begin(); it != fStartTracklets.end(); ++it) {

    if (combinedTracklets.find(it->first) == combinedTracklets.end()) {
      // tracklet was not combined

      if (it->second.hitIDs.size() - it->second.numSkewed > 2) {
        // enough hits for riemann fit
        fTrackletsWithoutCombi.push_back(it->first);
      } else {
        fShortTracklets[it->first] = fStartTracklets[it->first];
      }
    }
  }
}

void PndSttCATrackletGenerator::AddRemainingHits()
{

  LOG(debug1) << "void PndSttCATrackletGenerator::AddRemainingHits()" << endl;

  // add hits of short tracklets to the best combination
  int state;
  PndSttHit *sttHit;
  map<int, TrackletInf_t>::iterator it;
  for (it = fShortTracklets.begin(); it != fShortTracklets.end(); ++it) {
    state = it->first;

    for (size_t j = 0; j < fShortTracklets[state].hitIDs.size(); ++j) {
      sttHit = (PndSttHit *)fHits[fShortTracklets[state].hitIDs[j]];
      if (!fSttGeoH->IsSkewedStraw(sttHit->GetTubeID())) {
      //if (!fStrawMap->IsSkewedStraw(sttHit->GetTubeID())) {
        AddHitToBestCombi(fShortTracklets[state].hitIDs[j]);
      }
    }
  }
}

bool PndSttCATrackletGenerator::AddHitToBestCombi(int hitID)
{

  LOG(debug1) << "bool PndSttCATrackletGenerator::AddHitToBestCombi(int hitID)" << endl;

  double minDistance = 100, tmpDistance;
  PndSttHit *sttHit = (PndSttHit *)fHits[hitID];
  int tubeID = sttHit->GetTubeID();
  int sector = fSttGeoH->GetSector(tubeID);
  //int sector = fStrawMap->GetSector(tubeID);
  int sectorOfCombi, foundCombi;

  for (size_t i = 0; i < fCombinedData.size(); ++i) {

    sectorOfCombi = fSttGeoH->GetSector(*(fCombinedData[i].tracklets.begin()));
    //sectorOfCombi = fStrawMap->GetSector(*(fCombinedData[i].tracklets.begin()));
  }

  // min distance smaller than radius of a tube?
  if (minDistance < fTUBE_RADIUS) {
    // add hit to combi
    fCombinedData[foundCombi].trackletInf.hitIDs.push_back(hitID);
    LOG(debug1) << "AddHitToBestCombi(): add " << tubeID << " to Combi #" << foundCombi << ", distance: " << minDistance << endl;
    return true;
  } else {
    return false;
  }
}

/*################
 * Help functions
 * ##############*/

set<pair<int, int>> PndSttCATrackletGenerator::CreatePairCombis(int firstState, set<int> values)
{

  LOG(debug1) << "set<pair<int, int> > PndSttCATrackletGenerator::CreatePairCombis(...)" << endl;

  set<pair<int, int>> result;

  if (values.count(firstState) > 0) {
    for (set<int>::iterator iter = values.begin(); iter != values.end(); iter++) {
      pair<int, int> actualPair;
      actualPair.first = firstState;
      if (firstState != *iter) {
        actualPair.second = *iter;
        result.insert(actualPair);
      }
    }
  } else {
    LOG(debug1) << "-E- PndSttCATrackletGenerator::CreatePairCombis Value " << firstState << " is not in set: ";
    for (set<int>::iterator iter = values.begin(); iter != values.end(); iter++) {
      LOG(debug1) << *iter << " ";
    }
    LOG(debug1) << endl;
  }

  return result;
}

bool PndSttCATrackletGenerator::IsEndTubeOfTracklet(int tubeID)
{

  LOG(debug1) << "bool PndSttCATrackletGenerator::IsEndTubeOfTracklet(int tubeID)" << endl;

  // tube is end of tracklet if there's only one hit-neighbor or if it has neighbors with ambiguity hits
  if (fHitNeighbors[tubeID].size() == 1 || fHitNeighbors[fHitNeighbors[tubeID][0]].size() > 2 || fHitNeighbors[fHitNeighbors[tubeID][1]].size() > 2) {
    return true;
  }

  return false;
}
