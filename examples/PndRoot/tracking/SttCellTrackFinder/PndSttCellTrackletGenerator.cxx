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
 * PndSttCellTrackletGenerator.cxx
 *
 *  Created on: Jun 13, 2014
 *      Author: schumann
 */

#include "PndSttCellTrackletGenerator.h"
#include "FairHit.h"
//#include "PndSttStrawMap.h"
#include "PndStt2GeoHandler.h"
#include "PndRiemannHit.h"
#include "PndSttSkewedHit.h"
#include "TTimeStamp.h"

ClassImp(PndSttCellTrackletGenerator);

using namespace std;

void PndSttCellTrackletGenerator::SetCorrectedHits(map<int, FairHit *> correctedHits)
{

  if (fVerbose > 3)
    cout << "PndSttCellTrackletGenerator::SetCorrectedHits" << endl;
  fCorrectedHits.clear();

  if (correctedHits.size() != 0) {

    fCalcWithCorrectedHits = true;
    fCorrectedHits = correctedHits;

  } else {
    fCalcWithCorrectedHits = false;
  }
}

void PndSttCellTrackletGenerator::RefitTracks()
{

  if (fVerbose > 2)
    cout << "PndSttCellTrackletGenerator::RefitTracks()" << endl;

  fCombiRiemannTrack.clear();
  fCombiTrack.clear();
  fCombiTrackCand.clear();

  if (fCalcFirstTrackletInf) {
    fFirstTrackCand.clear();
    fFirstRiemannTrack.clear();

    for (map<int, TrackletInf_t>::iterator trackIt = fStartTracklets.begin(); trackIt != fStartTracklets.end(); ++trackIt) {

      TrackletInf_t trackletInf = trackIt->second;
      // calc riemannTrack
      PndRiemannTrack riemannTrack = CreateRiemannTrack(trackletInf.hitIDs);

      // add riemannTrack to trackletInf if possible
      if (riemannTrack.getNumHits() > 2) {
        // riemannTrack.refit(false);
        trackletInf.riemannTrack = riemannTrack;
        trackletInf.error = CalcDeviationOfRiemannTrack(riemannTrack);
        trackletInf.numErrHits = GetDeviationCount(riemannTrack);
        fFirstRiemannTrack.push_back(riemannTrack);
      }

      PndTrackCand trackCand;
      for (size_t i = 0; i < trackletInf.hitIDs.size(); ++i) {
        // add hits to TrackCand
        if (fCalcWithCorrectedHits && (fCorrectedHits.find(trackletInf.hitIDs.at(i)) != fCorrectedHits.end())) {

          trackCand.AddHit(fCorrectedHits[trackletInf.hitIDs.at(i)]->GetEntryNr(), i);
        } else {
          trackCand.AddHit(fMapHitToFairLink[trackletInf.hitIDs.at(i)], i);
        }
      }

      fFirstTrackCand.push_back(trackCand);
    }
  }

  // update TrackletInf of combinations --> Hits of RiemannTracks changed
  for (size_t i = 0; i < fCombinedData.size(); i++) {
    fCombinedData[i].trackletInf = GetTrackletInf(fCombinedData[i].tracklets);
  }

  // Track refit
  CreatePndTrackCands();
}

void PndSttCellTrackletGenerator::PrintInfo()
{

  cout << "PndSttCellTrackletGenerator::PrintInfo()" << endl;

  cout << "#Start Tracklets: " << fStartTracklets.size() << endl;

  for (map<int, TrackletInf_t>::iterator i = fStartTracklets.begin(); i != fStartTracklets.end(); ++i) {
    cout << "state: " << i->first << ", tubeIDs: ";
    for (size_t j = 0; j < i->second.hitIDs.size(); ++j)
      cout << ((PndSttHit *)fHits[i->second.hitIDs.at(j)])->GetTubeID() << ", ";
    cout << endl;
  }

  int maxNumMultiState = 0, averageNumMultiState = 0;

  cout << "#Tubes with multistate: " << fMultiStates.size() << endl;
  for (map<int, set<int>>::iterator it = fMultiStates.begin(); it != fMultiStates.end(); ++it) {
    cout << it->first << ": ";
    for (set<int>::iterator setIt = it->second.begin(); setIt != it->second.end(); ++setIt) {
      cout << *setIt << ", ";
    }
    if (maxNumMultiState < (int)it->second.size())
      maxNumMultiState = it->second.size();
    averageNumMultiState += it->second.size();
    cout << endl;
  }

  if (fMultiStates.size() != 0) {
    averageNumMultiState = averageNumMultiState / fMultiStates.size();
    cout << "Max #MultiState: " << maxNumMultiState << ", Avergage #MultiState: " << averageNumMultiState << endl;
  }

  cout << "#Found Combinations (recursive): " << fStateCombinations.size() << endl;

  cout << "#Accepted Combination of Tracklets: " << fCombinedData.size() << endl;
  for (size_t i = 0; i < fCombinedData.size(); ++i) {
    cout << "Combination #" << i << endl;
    cout << "state of tracklets: ";
    for (set<int>::iterator iter = fCombinedData[i].tracklets.begin(); iter != fCombinedData[i].tracklets.end(); iter++) {
      cout << *iter << ", ";
    }
    cout << endl;
  }
}

void PndSttCellTrackletGenerator::FindTracks()
{

  // Time stamps for runtime analysis

  // start time for FindTracks()
  fTimeStamps[18] = TTimeStamp();

  if (fVerbose > 0) {
    cout << "PndSttCellTrackletGenerator::FindTracks()" << endl;
  }

  if (fUseGPU) {
    GenerateTrackletsGPU();
  } else {
    fTimeStamps[6] = TTimeStamp();
    GenerateTracklets();
    fTimeStamps[7] = TTimeStamp();
  }

  fTimeStamps[8] = fTimeStamps[7];
  CombineTrackletsMultiStages();
  fTimeStamps[9] = TTimeStamp();

  fTimeStamps[10] = fTimeStamps[9];
  AssignAmbiguousHits();
  fTimeStamps[11] = TTimeStamp();

  // Update trackletInf
  for (size_t i = 0; i < fCombinedData.size(); i++) {
    fCombinedData[i].trackletInf = GetTrackletInf(fCombinedData[i].tracklets);
  }

  fTimeStamps[12] = TTimeStamp();
  SplitData();
  fTimeStamps[13] = TTimeStamp();

  fTimeStamps[14] = fTimeStamps[13];
  AddRemainingHits();
  fTimeStamps[15] = TTimeStamp();

  fTimeStamps[16] = fTimeStamps[15];
  CreatePndTrackCands();
  fTimeStamps[17] = TTimeStamp();

  // ending time of FindTracks()
  fTimeStamps[19] = fTimeStamps[17];
}

void PndSttCellTrackletGenerator::CreatePndTrackCands()
{
  if (fVerbose > 1) {
    cout << "PndSttCellTrackletGenerator::CreatePndTrackCands()" << endl;
  }

  int numHits; // state,  //[R.K. 01/2017] unused variable?

  // create TrackCands for each combination of tracklets
  for (size_t i = 0; i < fCombinedData.size(); ++i) {
    PndTrackCand trackCand;

    // add all hits of combined tracklets to trackCand
    for (size_t j = 0; j < fCombinedData[i].trackletInf.hitIDs.size(); ++j) {

      int hitIndex = fCombinedData[i].trackletInf.hitIDs.at(j);

      if (fCalcWithCorrectedHits && (fCorrectedHits.find(hitIndex) != fCorrectedHits.end())) {

        trackCand.AddHit(fCorrectedHits[hitIndex]->GetEntryNr(), j);
      } else {
        trackCand.AddHit(fMapHitToFairLink[hitIndex], j);
      }
    }

    fCombiTrackCand.push_back(trackCand);
    fCombiRiemannTrack.push_back(fCombinedData[i].trackletInf.riemannTrack);
    fCombiTrack.push_back(fCombinedData[i].trackletInf.riemannTrack.getPndTrack(fBz));
    fCombiTrack.back().SetLinks(*(trackCand.GetPointerToLinks()));
    // fCombinedData[i].trackletInf.riemannTrack.getPndTrack(2.0)); //TODO replace fixed magnetic field with info from simulation

    if (fVerbose > 2) {
      cout << "TrackletInf of tracklet-combination: ";
      fCombinedData[i].trackletInf.Print();
      cout << endl;

      cout << "Created PndTrack " << i << " :" << fCombinedData[i].trackletInf.riemannTrack.getPndTrack(fBz) << endl;
    }
  }

  // create TrackCands for tracklets without a combi and more than 2 hits
  for (size_t i = 0; i < fTrackletsWithoutCombi.size(); ++i) {
    PndTrackCand trackCand;
    numHits = fStartTracklets[fTrackletsWithoutCombi[i]].hitIDs.size();

    if (numHits > 2) {

      for (int j = 0; j < numHits; ++j) {

        int hitIndex = fStartTracklets[fTrackletsWithoutCombi[i]].hitIDs[j];

        if (fCalcWithCorrectedHits && (fCorrectedHits.find(hitIndex) != fCorrectedHits.end())) {
          trackCand.AddHit(fCorrectedHits[hitIndex]->GetEntryNr(), j);
        } else {
          trackCand.AddHit(fMapHitToFairLink[hitIndex], j);
        }
      }

      PndRiemannTrack trackRefit = CreateRiemannTrack(fStartTracklets[fTrackletsWithoutCombi[i]].hitIDs);

      fCombiTrackCand.push_back(trackCand);
      fCombiRiemannTrack.push_back(trackRefit);
      fCombiTrack.push_back(trackRefit.getPndTrack(fBz));
      fCombiTrack.back().SetLinks(*(trackCand.GetPointerToLinks()));

      if (fVerbose > 2) {
        cout << "TrackletInf of tracklet without combination: ";
        fStartTracklets[fTrackletsWithoutCombi[i]].Print();
        cout << endl;

        cout << "Created PndTrack " << fCombiTrack.size() - 1 << " :" << trackRefit.getPndTrack(fBz) << endl;
      }
    }
  }
}
void PndSttCellTrackletGenerator::GenerateTrackletsGPU()
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

  if (fVerbose > 1) {

    cout << "#Start-Tracklets: " << fStartTracklets.size() << endl;

    cout << "Tracklet-Information: " << endl;
    for (map<int, TrackletInf_t>::iterator it = fStartTracklets.begin(); it != fStartTracklets.end(); ++it) {
      int state = it->first;
      TrackletInf_t inf = it->second;

      cout << "State: " << state << " ";
      inf.Print();
      cout << endl;
    }

    cout << "#Short-Tracklets: " << fShortTracklets.size() << endl;

    cout << "Tracklet-Information: " << endl;
    for (map<int, TrackletInf_t>::iterator it = fShortTracklets.begin(); it != fShortTracklets.end(); ++it) {
      int state = it->first;
      TrackletInf_t inf = it->second;

      cout << "State: " << state << " ";
      inf.Print();
      cout << endl;
    }
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

  if (fVerbose > 2) {
    cout << "MultiStates: " << endl;
    for (map<int, set<int>>::iterator iter = fMultiStates.begin(); iter != fMultiStates.end(); iter++) {
      cout << iter->first << " : ";
      for (set<int>::iterator iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++) {
        cout << *iter2 << " ";
      }
      cout << endl;
    }
  }

  // free memory
  free(sttHits);
  free(hitIndices);
  free(states);
#endif
}

void PndSttCellTrackletGenerator::GenerateTracklets()
{

  if (fVerbose > 1) {
    cout << "PndSttCellTrackletGenerator::GenerateTracklets()" << endl;
  }

  // initialize states of cells with the id of the tube
  PndSttHit *sttHit;
  for (size_t i = 0; i < fHits.size(); ++i) {
    sttHit = (PndSttHit *)fHits[i];
    fStates[sttHit->GetTubeID()] = sttHit->GetTubeID();
  }

  fTimeStamps[0] = TTimeStamp();
  EvaluateState();
  fTimeStamps[1] = TTimeStamp();

  if (fVerbose > 2) {
    cout << "States of start-tracklets: " << endl;
    for (map<int, int>::iterator iter = fStates.begin(); iter != fStates.end(); iter++) {
      cout << iter->first << " : " << iter->second << endl;
    }
  }

  fTimeStamps[2] = TTimeStamp();
  InitStartTracklets();
  fTimeStamps[3] = TTimeStamp();

  if (fVerbose > 1) {

    cout << "#Start-Tracklets: " << fStartTracklets.size() << endl;

    cout << "Tracklet-Information: " << endl;
    for (map<int, TrackletInf_t>::iterator it = fStartTracklets.begin(); it != fStartTracklets.end(); ++it) {
      int state = it->first;
      TrackletInf_t inf = it->second;

      cout << "State: " << state << " ";
      inf.Print();
      cout << endl;
    }

    cout << "#Short-Tracklets: " << fShortTracklets.size() << endl;

    cout << "Tracklet-Information: " << endl;
    for (map<int, TrackletInf_t>::iterator it = fShortTracklets.begin(); it != fShortTracklets.end(); ++it) {
      int state = it->first;
      TrackletInf_t inf = it->second;

      cout << "State: " << state << " ";
      inf.Print();
      cout << endl;
    }
  }

  fTimeStamps[4] = TTimeStamp();
  EvaluateMultiState();
  fTimeStamps[5] = TTimeStamp();

  if (fVerbose > 2) {
    cout << "MultiStates: " << endl;
    for (map<int, set<int>>::iterator iter = fMultiStates.begin(); iter != fMultiStates.end(); iter++) {
      cout << iter->first << " : ";
      for (set<int>::iterator iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++) {
        cout << *iter2 << " ";
      }
      cout << endl;
    }
  }
}

void PndSttCellTrackletGenerator::EvaluateState()
{

  if (fVerbose > 2) {
    cout << "PndSttCellTrackletGenerator::EvaluateState()" << endl;
  }

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
  plainHitIds.insert(fSeparations[2].begin(), fSeparations[2].end());

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

void PndSttCellTrackletGenerator::EvaluateMultiState()
{

  if (fVerbose > 2) {
    cout << "PndSttCellTrackletGenerator::EvaluateMultiState()" << endl;
  }

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

  if (fVerbose > 3) {
    cout << "EvaluateMultiState: AmbiguousHitIds: ";
    for (it = ambiguousHitIds.begin(); it != ambiguousHitIds.end(); it++) {
      cout << *it << " ";
    }
    cout << endl;
  }

  int loopcount = 0;
  // while sum of all states changes
  while (currentSum != priorSum) {
    if (fVerbose > 3) {
      cout << endl;
      cout << "LoopCount: " << loopcount++ << " currentSum: " << currentSum << " priorSum: " << priorSum << endl;
    }

    priorSum = currentSum;
    currentSum = 0;

    for (it = ambiguousHitIds.begin(); it != ambiguousHitIds.end(); ++it) {
      neighbors = fHitNeighbors[(*it)];
      newState.clear();
      if (fVerbose > 3) {
        cout << "AmbiguousHitId: " << *it << endl;
        cout << "Neighbors: " << endl;
      }

      for (size_t i = 0; i < neighbors.size(); i++) {
        if (fVerbose > 3)
          cout << neighbors[i] << " : ";

        if (fStates.count(neighbors[i]) > 0) {
          // if unambiguous neighbor copy one state
          newState.insert(fStates[neighbors[i]]);

          if (fVerbose > 3)
            cout << fStates[neighbors[i]];
        } else if (fMultiStates.count(neighbors[i]) > 0) {
          // if multistate-neighbor copy all states
          newState.insert(fMultiStates[neighbors[i]].begin(), fMultiStates[neighbors[i]].end());
          if (fVerbose > 3) {
            for (set<int>::iterator iter = fMultiStates[neighbors[i]].begin(); iter != fMultiStates[neighbors[i]].end(); iter++) {
              cout << *iter << "/";
            }
          }
        }
        if (fVerbose > 3)
          cout << " | ";
      }
      if (fVerbose > 3)
        cout << endl;

      // keep new one
      if (newState.size() > 0 && newState.size() > tmpStates[(*it)].size())
        tmpStates[(*it)] = newState;

      if (fVerbose > 3)
        cout << "NewState for " << *it << " : ";

      // calculate sum of all states
      for (set<int>::iterator stateIter = newState.begin(); stateIter != newState.end(); stateIter++) {
        if (fVerbose > 3)
          cout << *stateIter << " | ";
        currentSum += *stateIter;
      }
      if (fVerbose > 3)
        cout << endl;
    }

    // update states simultaneously
    fMultiStates.clear();
    fMultiStates.insert(tmpStates.begin(), tmpStates.end());
  }
}

void PndSttCellTrackletGenerator::InitStartTracklets()
{

  if (fVerbose > 3)
    cout << "void PndSttCellTrackletGenerator::InitStartTracklets()" << endl;

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

  if (fVerbose > 3)
    cout << "Calculation of StartTracklets: " << endl;
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

      if (fCalcFirstTrackletInf) {
        // add hits to TrackCand
        trackCand.AddHit(fMapHitToFairLink[hitIDs[i]], 0);
      }
    }

    if (fCalcFirstTrackletInf) {
      // calc riemannTrack
      PndRiemannTrack riemannTrack = CreateRiemannTrack(hitIDs);

      // add riemannTrack to trackletInf if possible
      if (riemannTrack.getNumHits() > 2) {
        // riemannTrack.refit(false);
        trackletInf.riemannTrack = riemannTrack;
        trackletInf.error = CalcDeviationOfRiemannTrack(riemannTrack);
        trackletInf.numErrHits = GetDeviationCount(riemannTrack);
        fFirstRiemannTrack.push_back(riemannTrack);
      }
      fFirstTrackCand.push_back(trackCand);
    }

    if (fVerbose > 3)
      cout << "RiemannTrack for startTracklet: " << state << " - " << trackletInf.riemannTrack << endl;

    // tracklet with enough hits
    fStartTracklets[state] = trackletInf;
  }
}

void PndSttCellTrackletGenerator::CombineTrackletsMultiStages()
{

  if (fVerbose > 1)
    cout << "void PndSttCellTrackletGenerator::CombineTrackletsMultiStages()" << endl;

  map<int, TrackletInf_t>::iterator trackletIt;

  // try to combine start- and short-tracklets that begin in the first row
  for (trackletIt = fStartTracklets.begin(); trackletIt != fStartTracklets.end(); ++trackletIt) {

    // only for tracklets that starts in the first row
    if (trackletIt->first <= 104) {
      set<int> combi;
      CombineTrackletsMultiStagesRecursive(trackletIt->first, combi);
    }
  }

  vector<set<int>>::iterator combiIt;
  set<int>::iterator setIt;

  if (fVerbose > 0)
    cout << "Found combinations: " << endl;

  // fit combinations
  for (combiIt = fStateCombinations.begin(); combiIt != fStateCombinations.end(); ++combiIt) {

    TrackletInf_t trackletInf;

    trackletInf = GetTrackletInf(*combiIt);

    if (fVerbose > 0) {
      for (setIt = (*combiIt).begin(); setIt != (*combiIt).end(); ++setIt) {
        cout << (*setIt) << " ";
      }
    }

    // store combinations with more than 3 hits only
    if (trackletInf.riemannTrack.getNumHits() > 2 && trackletInf.numErrHits == 0) {
      Combination_t combi;
      combi.tracklets = *combiIt;
      combi.trackletInf = trackletInf;

      fCombinedData.push_back(combi);

      if (fVerbose > 0)
        cout << "--> keep";
    }

    if (fVerbose > 0)
      cout << endl;
  }
}

void PndSttCellTrackletGenerator::CombineTrackletsMultiStagesRecursive(int stateToCombine, set<int> currentCombi)
{

  if (fVerbose > 2)
    cout << "CombineTrackletsMultiStagesRecursive, tracklet: " << stateToCombine << endl;

  set<int> newCombi;
  newCombi.insert(currentCombi.begin(), currentCombi.end());
  newCombi.insert(stateToCombine);

  int ambiguousID = 0;
  int endID;

  endID = fStartTracklets[stateToCombine].endID;

  set<int>::iterator it;

  if (fVerbose > 2) {
    cout << "neighbors of end-tube " << endID << ": ";
    for (size_t i = 0; i < fHitNeighbors[endID].size(); ++i) {
      cout << fHitNeighbors[endID].at(i) << " ";
    }
    cout << endl;
  }

  // tubes that are already included in a tracklet can only have one or two neighbors.
  // check if the tracklet could be continued

  if (fHitNeighbors[endID].size() == 2) {
    // ending tube of tracklet has an ambiguous neighbor --> could be combined

    // find neighbor with multistate to get information for possible combination
    if (fMultiStates.count(fHitNeighbors[endID].at(0))) {
      ambiguousID = fHitNeighbors[endID].at(0);

    } else if (fMultiStates.count(fHitNeighbors[endID].at(1))) {
      ambiguousID = fHitNeighbors[endID].at(1);
    }
    // multistate must be found because neighbor of end-tube lies next to the tracklet
    //--> should have been adapted

  } else if (fMultiStates.count(fHitNeighbors[endID].at(0))) {
    ambiguousID = fHitNeighbors[endID].at(0);
  }

  if (ambiguousID != 0) {
    // way was already chosen? only possible for strong bended tracks.
    // to avoid that combination goes "backwards" (definition of end-tube)

    for (it = currentCombi.begin(); it != currentCombi.end(); ++it) {
      if (fMultiStates[ambiguousID].count(*it)) {
        ambiguousID = 0;
        break;
      }
    }
  }

  if (ambiguousID == 0) {
    // no ambiguous neighbor found --> end of track
    if (fVerbose > 2) {
      cout << "ambiguous id was not found. finish combi: ";
      for (it = newCombi.begin(); it != newCombi.end(); ++it) {
        cout << *it << ", ";
      }
      cout << endl;
    }

    if (newCombi.size() > 1)
      InsertCombination(newCombi);
    return;
  }

  if (fVerbose > 2)
    cout << "found ambiguous tube: " << ambiguousID << endl;

  set<pair<int, int>> pairCombinations;
  set<pair<int, int>>::iterator pairIt;
  int combiPartner;

  // get possible combination
  pairCombinations = CreatePairCombis(stateToCombine, fMultiStates[ambiguousID]);

  if (fVerbose > 2) {
    cout << "StateToCombine " << stateToCombine << " with: ";
    for (set<int>::iterator iter = fMultiStates[ambiguousID].begin(); iter != fMultiStates[ambiguousID].end(); iter++) {
      cout << *iter << " ";
    }
    cout << endl;
    cout << "results in PairCombinations: ";
    for (set<pair<int, int>>::iterator iter = pairCombinations.begin(); iter != pairCombinations.end(); iter++) {
      cout << iter->first << "/" << iter->second << " ";
    }
    cout << endl;
  }

  // find pair with curState
  for (pairIt = pairCombinations.begin(); pairIt != pairCombinations.end(); ++pairIt) {
    combiPartner = pairIt->second;

    if (fVerbose > 2)
      cout << "found combipartner: " << combiPartner << endl;

    if (!newCombi.count(combiPartner))
      CombineTrackletsMultiStagesRecursive(combiPartner, newCombi);
  }
}

void PndSttCellTrackletGenerator::InsertCombination(set<int> combination)
{

  if (fVerbose > 4) {
    cout << "void PndSttCellTrackletGenerator::InsertCombination(set<int> combination)" << endl;
  }

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
TrackletInf_t PndSttCellTrackletGenerator::GetTrackletInf(set<int> tracklets)
{

  if (fVerbose > 4) {
    cout << "TrackletInf_t PndSttCellTrackletGenerator::GetTrackletInf(set<int> tracklets)" << endl;
  }

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

  inf.riemannTrack = CreateRiemannTrack(inf.hitIDs);

  if (inf.riemannTrack.getNumHits() > 2) {
    inf.error = CalcDeviationOfRiemannTrack(inf.riemannTrack);
    inf.numErrHits = GetDeviationCount(inf.riemannTrack);
  }

  return inf;
}

void PndSttCellTrackletGenerator::SplitData()
{

  if (fVerbose > 1) {
    cout << "void PndSttCellTrackletGenerator::SplitData" << endl;
  }

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

void PndSttCellTrackletGenerator::AssignAmbiguousHits()
{

  if (fVerbose > 1) {
    cout << "void PndSttCellTrackletGenerator::AssignAmbiguousHits()" << endl;
  }

  for (map<int, set<int>>::iterator multistateIt = fMultiStates.begin(); multistateIt != fMultiStates.end(); multistateIt++) {
    if (multistateIt->second.size() == 1) {
      // hit can be assigned to only one possible tracklet

      fStartTracklets[*(multistateIt->second.begin())].hitIDs.push_back(fMapTubeIdToHit[multistateIt->first]);

      if (fVerbose > 1)
        cout << "add " << multistateIt->first << " to (only possible) tracklet " << *(multistateIt->second.begin()) << endl;

    } else if (multistateIt->second.size() == 2) {

      PndRiemannHit hit(fHits[fMapTubeIdToHit[multistateIt->first]]);

      // check if RiemannTrack was calculated
      if (fStartTracklets[*(multistateIt->second.begin())].riemannTrack.getNumHits() == 0) {
        fStartTracklets[*(multistateIt->second.begin())].riemannTrack = CreateRiemannTrack(fStartTracklets[*(multistateIt->second.begin())].hitIDs);
      }
      // check if RiemannTrack has at least 3 hits and distance to the hit
      if (fStartTracklets[*(multistateIt->second.begin())].riemannTrack.getNumHits() > 2 &&
          abs(fStartTracklets[*(multistateIt->second.begin())].riemannTrack.dist(&hit)) < fTUBE_RADIUS) {
        fStartTracklets[*(multistateIt->second.begin())].hitIDs.push_back(fMapTubeIdToHit[multistateIt->first]);

        if (fVerbose > 1)
          cout << "add " << multistateIt->first << " to tracklet " << *(multistateIt->second.begin()) << endl;
      }
    }
  }
}

void PndSttCellTrackletGenerator::AddRemainingHits()
{

  if (fVerbose > 1) {
    cout << "void PndSttCellTrackletGenerator::AddRemainingHits()" << endl;
  }

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

bool PndSttCellTrackletGenerator::AddHitToBestCombi(int hitID)
{

  if (fVerbose > 3) {
    cout << "bool PndSttCellTrackletGenerator::AddHitToBestCombi(int hitID)" << endl;
  }

  double minDistance = 100, tmpDistance;
  PndSttHit *sttHit = (PndSttHit *)fHits[hitID];
  int tubeID = sttHit->GetTubeID();
  int sector = fSttGeoH->GetSector(tubeID);
  //int sector = fStrawMap->GetSector(tubeID);
  int sectorOfCombi, foundCombi;

  for (size_t i = 0; i < fCombinedData.size(); ++i) {

    sectorOfCombi = fSttGeoH->GetSector(*(fCombinedData[i].tracklets.begin()));
    //sectorOfCombi = fStrawMap->GetSector(*(fCombinedData[i].tracklets.begin()));

    if (sector / 3 == sectorOfCombi / 3) {
      // combi and hit are in the same half of the STT (integer division)
      tmpDistance = CalcDeviation(fCombinedData[i].trackletInf.riemannTrack, hitID);
      if (tmpDistance < minDistance) {
        minDistance = tmpDistance;
        foundCombi = i;
      }
    }
  }

  // min distance smaller than radius of a tube?
  if (minDistance < fTUBE_RADIUS) {
    // add hit to combi
    fCombinedData[foundCombi].trackletInf.hitIDs.push_back(hitID);
    if (fVerbose > 2) {
      cout << "AddHitToBestCombi(): add " << tubeID << " to Combi #" << foundCombi << ", distance: " << minDistance << endl;
    }
    return true;
  } else {
    return false;
  }
}

PndRiemannTrack PndSttCellTrackletGenerator::CreateRiemannTrack(vector<int> hitIDs)
{

  if (fVerbose > 3)
    cout << "PndSttCellTrackletGenerator::CreateRiemannTrack with correction " << fCalcWithCorrectedHits << endl;

  PndRiemannTrack riemannTrack;
  PndSttHit *sttHit;
  set<int> skewedTubeIdsInTrack;

  // add hits to riemannTrack (if unskewed)
  for (size_t i = 0; i < hitIDs.size(); ++i) {

    sttHit = (PndSttHit *)fHits[hitIDs[i]];
    if (!fSttGeoH->IsSkewedStraw(sttHit->GetTubeID())) {
    //if (!fStrawMap->IsSkewedStraw(sttHit->GetTubeID())) {

      if (fCalcWithCorrectedHits && fCorrectedHits.find(hitIDs[i]) != fCorrectedHits.end()) {

        PndRiemannHit riemannHit(fCorrectedHits[hitIDs[i]], hitIDs[i]);
        riemannTrack.addHit(riemannHit);

      } else {
        PndRiemannHit riemannHit(fHits[hitIDs[i]], hitIDs[i]);
        riemannHit.setDXYZ(1, 1, 100);
        riemannTrack.addHit(riemannHit);
      }
    } else {
      skewedTubeIdsInTrack.insert(sttHit->GetTubeID());
    }
  }

  if (fVerbose > 3)
    cout << "#hits: " << riemannTrack.getNumHits() << endl;

  // fit riemannTrack if it contains more than 2 hits
  if (riemannTrack.getNumHits() > 2) {

    if (fVerbose > 3) {
      vector<PndRiemannHit> hits = riemannTrack.getHits();
      cout << endl << "RiemannTrack, Input: " << endl;
      for (size_t i = 0; i < hits.size(); ++i) {
        const FairHit *hit = hits[i].hit();
        cout << "(" << hit->GetX() << "|" << hit->GetY() << "), ";
      }
      cout << endl;
    }

    riemannTrack.refit(false);

    if (fVerbose > 3) {
      cout << "Calculated RiemannTrack: " << riemannTrack << endl << "End of Calculation" << endl;
    }

    for (set<int>::iterator tubeIter = skewedTubeIdsInTrack.begin(); tubeIter != skewedTubeIdsInTrack.end(); tubeIter++) {
      multimap<int, PndSttSkewedHit *>::iterator it, itlow, itup, itbest;
      itlow = fCombinedSkewedHits.lower_bound(*tubeIter);
      itup = fCombinedSkewedHits.upper_bound(*tubeIter);
      itbest = fCombinedSkewedHits.end();
      Double_t minDist = 1000;
      for (it = itlow; it != itup; ++it) {
        PndRiemannHit skewedRiemann(it->second);
        Double_t actDist = riemannTrack.dist(&skewedRiemann);
        if (actDist < minDist) {
          itbest = it;
          minDist = actDist;
        }
      }
      if (itbest != fCombinedSkewedHits.end()) {
        PndRiemannHit skewedRiemann(itbest->second);
        riemannTrack.addHit(skewedRiemann);
      }
    }
    riemannTrack.refit(false);
  }

  return riemannTrack;
}

/*################
 * Help functions
 * ##############*/

set<pair<int, int>> PndSttCellTrackletGenerator::CreatePairCombis(int firstState, set<int> values)
{

  if (fVerbose > 3) {
    cout << "set<pair<int, int> > PndSttCellTrackletGenerator::CreatePairCombis(...)" << endl;
  }

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
  } else if (fVerbose > 3) {
    cout << "-E- PndSttCellTrackletGenerator::CreatePairCombis Value " << firstState << " is not in set: ";
    for (set<int>::iterator iter = values.begin(); iter != values.end(); iter++) {
      cout << *iter << " ";
    }
    cout << endl;
  }

  return result;
}

double PndSttCellTrackletGenerator::CalcDeviationOfRiemannTrack(PndRiemannTrack &track)
{

  if (fVerbose > 3) {
    cout << "double PndSttCellTrackletGenerator::CalcDeviationOfRiemannTrack(...)" << endl;
  }

  vector<PndRiemannHit> hits = track.getHits();
  TVector2 pos;
  TVectorD orig(2);
  orig = track.orig();
  TVector2 diff;
  double r = track.r();
  double sum = 0;

  for (size_t i = 0; i < track.getNumHits(); ++i) {

    // get x- and y-coordinate of the hit
    pos.Set(hits[i].x()[0], hits[i].x()[1]);
    // calc diff-vector to origin
    diff.Set(pos.X() - orig[0], pos.Y() - orig[1]);
    // calc distance to circle
    sum += (diff.Mod() - r) * (diff.Mod() - r);
  }

  return sum / track.getNumHits();
}

double PndSttCellTrackletGenerator::CalcDeviation(PndRiemannTrack &track, int hitID)
{

  if (fVerbose > 3) {
    cout << "double PndSttCellTrackletGenerator::CalcDeviation(...)" << endl;
  }

  PndSttHit *sttHit = (PndSttHit *)fHits[hitID];
  TVector3 pos = fMapTubeIdToPos[sttHit->GetTubeID()];
  TVector2 diff;
  TVectorD orig(2);
  double r = track.r();
  orig = track.orig();

  diff.Set(pos.X() - orig[0], pos.Y() - orig[1]);

  return TMath::Abs(diff.Mod() - r);
}

int PndSttCellTrackletGenerator::GetDeviationCount(PndRiemannTrack &track)
{

  if (fVerbose > 3) {
    cout << "int PndSttCellTrackletGenerator::GetDeviationCount(PndRiemannTrack& track)" << endl;
  }

  int counter = 0;

  vector<PndRiemannHit> hits = track.getHits();
  TVector2 pos;
  TVectorD orig(2);
  orig = track.orig();
  TVector2 diff;
  double r = track.r();

  for (size_t i = 0; i < track.getNumHits(); ++i) {

    // get x- and y-coordinate of the hit
    pos.Set(hits[i].x()[0], hits[i].x()[1]);
    diff.Set(pos.X() - orig[0], pos.Y() - orig[1]);

    if (TMath::Abs(diff.Mod() - r) > fTUBE_RADIUS) {
      ++counter;
    }
  }
  return counter;
}

bool PndSttCellTrackletGenerator::IsEndTubeOfTracklet(int tubeID)
{

  if (fVerbose > 4) {
    cout << "bool PndSttCellTrackletGenerator::IsEndTubeOfTracklet(int tubeID)" << endl;
  }

  // tube is end of tracklet if there's only one hit-neighbor or if it has neighbors with ambiguity hits
  if (fHitNeighbors[tubeID].size() == 1 || fHitNeighbors[fHitNeighbors[tubeID][0]].size() > 2 || fHitNeighbors[fHitNeighbors[tubeID][1]].size() > 2) {
    return true;
  }

  return false;
}
