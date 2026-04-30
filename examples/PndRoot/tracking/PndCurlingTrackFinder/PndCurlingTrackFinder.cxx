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

/////////////////////////////////////////////////////////////
//  PndCurlingTrackFinder
//  Track Finder for primaries and secondaries in Panda's barrel part
/////////////////////////////////////////////////////////////////

/** PndHoughApollonius
 *@author Anna Alicke <a.alicke@fz-juelich.de>
 *@since 14.09.2021
 *@version 1.0
 **
 ** Track Finder for primaries and Secondaries in Panda's barrel part
 ** Task level RECO
 **/

#include "PndCurlingTrackFinder.h"
#include "PndHoughApollonius.h"
#include "PndTrackEvaluatorDetStt.h"
#include "PndApolloniusTriplet.h"

//#include "PndSttStrawMap.h"
//#include "PndSttGeometryMap.h"


//PndCurlingTrackFinder::PndCurlingTrackFinder(TClonesArray *tubeArray)
//{
  //fGeometryMap = new PndSttGeometryMap(tubeArray, 1);
  //fStrawMap = new PndSttStrawMap(tubeArray);
//}

PndCurlingTrackFinder::~PndCurlingTrackFinder() {}

void PndCurlingTrackFinder::Reset()
{
  fTripletTracksAfterAdding.clear();
  fContinuousTripletTracks.clear();
  fTripletTracks.clear();
  fSolutions.clear();
  fMapHitstoCATracklet.clear();
}

/**
 * @brief      Main method to find the tracks of one event
 *             The algorithm consist of different steps:
 *             2. Find rows in STT which are used for the triplet generation (GenerateTriplets)
 *             3. Reduce the number of combinations for the triplets (ReduceCombinatorics)
 *             4. Calculates the Apollonius circles for each triplet (GenerateTripletTracks)
 *             5. Takes only the Apollonius circles which are continuous in the STT (CheckContinuitySolutions)
 *             6. Hits from other detectors are added to the found apollonius circles (AddOtherDetectors)
 *             7. Returns only the best solution of the 8 possible apollonoius circles (FindBestSolutions)
 *             8. Combines identical solutions and reduces the number of tracks to the number of expected tracks in the event (CombineIdenticalSolutions, CheckCombinedSolutions)
 *             9. Finds straight tracks within a curcling track (GenerateCurlingGroups, FindStraightInCurled)
 */
void PndCurlingTrackFinder::FindTracks()
{

  LOG(debug) << "FindTracks";

  if (fBranchMap.size() == 0) {
    LOG(error) << "No data branches given!";
    return;
  }

  // the steps of the algorithms are numbered like in the description
  fMapHitstoCATracklet = fFunc.GetHitsToCAMap(fCATrackFinder);

  if (fUnassignedSttHits.size() > 3) {
    std::vector<TripletSolution> groupSolutions;
    double fMinDistance = 5.;
    //std::map<int, std::vector<std::vector<PndSttHit *>>> tubeStructure = fFunc.GetTubeStructure(fUnassignedSttHits, fGeometryMap, fAllHitsCounter, fIsStrongCurling);
    std::map<int, std::vector<std::vector<PndSttHit *>>> tubeStructure = fFunc.GetTubeStructure(fUnassignedSttHits, fSttGeoH, fAllHitsCounter, fIsStrongCurling);
    TripletValues triplets = GetTripletsCurledTracks(tubeStructure);
    // TripletValues triplets = GetTripletsCurledTracks2();
    // std::cout << "Generated Triplets: " << triplets << std::endl;
    std::vector<Triplet> tripletIndices =
      fFunc.ReduceCombinatorics(triplets, fMapHitstoCATracklet, fBranchMap["STTHit"], fCATrackFinder, fSttGeoH, fMinDistance, fWithCombiReduction);
      //fFunc.ReduceCombinatorics(triplets, fMapHitstoCATracklet, fBranchMap["STTHit"], fCATrackFinder, fGeometryMap, fMinDistance, fWithCombiReduction);
    // std::cout << "number f triplets: " << tripletIndices.size() << std::endl;
    // for (auto t : tripletIndices)
    //  std::cout << t;

    for (auto triplet : tripletIndices) {
      // Is triplet used is missing here isn't ist?
      LOG(debug) << "Solution for triplet: " << triplet.fTripletHits[0]->GetTubeID() << "/" << triplet.fTripletHits[1]->GetTubeID() << "/" << triplet.fTripletHits[2]->GetTubeID();

      if (fFunc.IsTripletUsed(groupSolutions, triplet)) {
        LOG(debug) << "Triplet used";
        continue;
      }
      // 4. Calculates the Apollonius circles for each triplet (GenerateTripletTracks)
      std::vector<TripletSolution> tripletSolutions;

      tripletSolutions = fFunc.GenerateTripletTracks(triplet, fUnassignedSttHits, fSttGeoH);
      //tripletSolutions = fFunc.GenerateTripletTracks(triplet, fUnassignedSttHits, fGeometryMap);
#ifdef DEBUGApolloniusTripletFinder
      for (auto solution : tripletSolutions) {
        fTripletTracks.push_back(solution);
      }
#endif
      // 5. Takes only the Apollonius circles which are continuous in the STT (CheckContinuitySolutions)
      fFunc.CheckContinuitySolutions(tripletSolutions, fSttGeoH);
      //fFunc.CheckContinuitySolutions(tripletSolutions, fGeometryMap);
#ifdef DEBUGApolloniusTripletFinder
      for (auto solution : tripletSolutions) {
        fContinuousTripletTracks.push_back(solution);
      }
#endif
      // 6. Hits from other detectors are added to the found apollonius circles (AddOtherDetectors)
      fFunc.AddOtherDetectors(tripletSolutions, fSttGeoH, fBranchMap, "STTHit");
      //fFunc.AddOtherDetectors(tripletSolutions, fStrawMap, fBranchMap, "STTHit");
#ifdef DEBUGApolloniusTripletFinder
      for (auto solution : tripletSolutions) {
        fTripletTracksAfterAdding.push_back(solution);
      }
#endif
      // 7. Returns only the best solution of the 8 possible apollonoius circles (FindBestSolutions)
      std::vector<TripletSolution> bestSolutions = fFunc.FindBestSolutions(tripletSolutions);
      groupSolutions.insert(groupSolutions.end(), bestSolutions.begin(), bestSolutions.end());
    }
#ifdef DEBUGApolloniusTripletFinder
    LOG(debug) << "Grouped Solutions: ";
    for (auto solution : groupSolutions) {
      LOG(debug) << solution;
    }
#endif
    // 8. Combines identical solutions and reduces the number of tracks to the number of expected tracks in the event (CombineIdenticalSolutions, CheckCombinedSolutions)
    std::vector<TripletSolution> differentSolutions = fFunc.CombineIdenticalSolutions(groupSolutions);
#ifdef DEBUGApolloniusTripletFinder
    LOG(debug) << "Different Solutions: ";
    for (auto solution : differentSolutions) {
      LOG(debug) << solution;
    }
#endif
    int fNExpectedTracks = 1;
    std::vector<TripletSolution> combinedSolutions = fFunc.CheckCombinedSolutions(differentSolutions, fNExpectedTracks);

    LOG(debug) << "Combined Solutions: ";
    for (auto solution : combinedSolutions) {
      solution.SortAllHits(fSttGeoH);
      //solution.SortAllHits(fGeometryMap);
      LOG(debug) << solution;
      fSolutions.push_back(solution);
      // std::cout << "solution: " << solution << std::endl;
      ;
    }
  }
}

/**
 * @brief      Method to find streight tracks within a curling track
 *
 * @param[in] combinedSolutions         A vector containing the already found tracks
 * @param[in] CombinedTubeStructure     A map containing the tube structure of the curling track and possible streight tracks that can't be distinguished from the curling track
 * @param[out] CombinedTubeStructure    A map containing the tube structure of the curling track. The hits belonging to the streight tracks are deleted.
 */
/*
std::map<int, std::vector<std::vector<PndSttHit *>>>
PndCurlingTrackFinder::DeleteFromCurlingPool(std::vector<TripletSolution> &combinedSolutions, std::map<int, std::vector<std::vector<PndSttHit *>>> &CombinedTubeStructure)
{

  for (auto solution : combinedSolutions) {
    for (auto hit : solution.fHits[TripletSolution::detID::STT]) {
      PndSttHit *sttHit = (PndSttHit *)hit;
      fCurlingGroup.erase(std::remove(fCurlingGroup.begin(), fCurlingGroup.end(), sttHit), fCurlingGroup.end());

      for (auto it = CombinedTubeStructure.begin(); it != CombinedTubeStructure.end(); it++) {
        for (auto group : it->second) {
          group.erase(std::remove(group.begin(), group.end(), sttHit), group.end());
        }
      }
    }
  }
  return CombinedTubeStructure;
}

*/
TripletValues PndCurlingTrackFinder::GetTripletsCurledTracks(std::map<int, std::vector<std::vector<PndSttHit *>>> &tubeStructure)
{
  TripletValues result;

  auto mapIterFirst = tubeStructure.begin();
  // auto mapIterLast = tubeStructure.end();
  // mapIterLast--;

  std::vector<std::pair<int, int>> rowFirst;
  std::vector<std::pair<int, int>> rowLast;
  std::vector<std::pair<int, int>> rowMidPair;
  rowFirst.clear();
  rowLast.clear();
  rowMidPair.clear();
  rowFirst.push_back(std::make_pair(mapIterFirst->first, mapIterFirst->second.size()));
  // rowLast.push_back(std::make_pair(mapIterFirst->first, mapIterFirst->second.size()));
  /*
  for (auto lastIter = tubeStructure.rbegin(); lastIter != tubeStructure.rend(); lastIter++) {
    if (lastIter->first >= fAfterSkewedRowNumber) {
      rowMidPair.push_back(std::make_pair(lastIter->first, lastIter->second.size()));
      break;
    }
  }
  for (auto lastIter = tubeStructure.rbegin(); lastIter != tubeStructure.rend(); lastIter++) {
    if (lastIter->first <= fBeforeSkewedRowNumber) {
      rowMidPair.push_back(std::make_pair(lastIter->first, lastIter->second.size()));
      break;
    }
  }
  */

  int last = 0;
  for (auto lastIter = tubeStructure.rbegin(); lastIter != tubeStructure.rend(); lastIter++) {
    if (fSttGeoH->IsSkewedRow(lastIter->first)) {
    //if (fGeometryMap->IsSkewedRow(lastIter->first)) {
      continue;
    }
    bool insert = kFALSE;
    for (int i = 0; i < lastIter->second.size(); i++) {
      if (lastIter->second[i].size() > last) {
        last = lastIter->second[i].size();
        insert = kTRUE;
      }
    }
    if (insert)
      rowMidPair.push_back(std::make_pair(lastIter->first, lastIter->second.size()));
  }

  last = 0;
  for (auto lastIter = tubeStructure.rbegin(); lastIter != tubeStructure.rend(); lastIter++) {
    if (fSttGeoH->IsSkewedRow(lastIter->first)) {
    //if (fGeometryMap->IsSkewedRow(lastIter->first)) {
      continue;
    }
    if (lastIter->second.size() > last) {
      rowLast.push_back(std::make_pair(lastIter->first, lastIter->second.size()));
      last = lastIter->second.size();
    }
  }

  /*
  std::cout << "the first row has tubes: ";
  for (int i = 0; i < mapIterFirst->second.size(); i++) {
    std::cout << "(";
    for (int j = 0; j < mapIterFirst->second[i].size(); j++) {
      std::cout << mapIterFirst->second[i][j]->GetTubeID() << " " << mapIterFirst->second[i][j]->GetEntryNr() << " / ";
    }
    std::cout << "), ";
  }
  std::cout << std::endl;
  std::cout << "the last rows are " << rowMidPair.size() << " rows: ";
  for (int idx = 0; idx < rowMidPair.size(); idx++) {
    std::cout << "row " << rowMidPair[idx].first << " ";
    for (int i = 0; i < tubeStructure[rowMidPair[idx].first].size(); i++) {
      std::cout << "(";
      for (int j = 0; j < tubeStructure[rowMidPair[idx].first][i].size(); j++) {
        std::cout << tubeStructure[rowMidPair[idx].first][i][j]->GetTubeID() << " " << tubeStructure[rowMidPair[idx].first][i][j]->GetEntryNr() << " / ";
      }
      std::cout << "), ";
    }
  }
  std::cout << std::endl;
  */
  // find hits with max distance
  /*
  PndSttHit* firstHit = nullptr;
  PndSttHit* lastHit = nullptr;
  double d = 0;
  for(int i = 0; i < mapIterFirst->second.size()-1; i++){
    for(int j = i+1; j < mapIterFirst->second.size(); j++){
      for(int k = 0; k < mapIterFirst->second[i].size(); k++){
        for(int l = 0; l < mapIterFirst->second[j].size(); l++){
          PndSttHit* hit1 = mapIterFirst->second[i][k];
          PndSttHit* hit2 = mapIterFirst->second[j][l];
          double dtemp = sqrt((hit1->GetX() - hit2->GetX()) * (hit1->GetX() - hit2->GetX()) + (hit1->GetY() - hit2->GetY()) * (hit1->GetY() - hit2->GetY()));
          std::cout << "hit1 (" << hit1->GetX() << "," << hit1->GetY() << "), hit2 (" << hit2->GetX() << "," << hit2->GetY() << ") d = " << dtemp << ", dold = " << d << std::endl;
          if(dtemp > d){
            firstHit = hit1;
            lastHit = hit2;
            d = dtemp;
          }
        }
      }
    }
  }
  std::cout << "done" << std::endl;
  */
  if (fWithTubeReduction) {
    result.fFirstRow = fFunc.TubeReduction(rowFirst, tubeStructure, fMapHitstoCATracklet, 0);
    result.fLastRow = fFunc.TubeReduction(rowLast, tubeStructure, fMapHitstoCATracklet, 0);
    result.fMidRow = fFunc.TubeReduction(rowMidPair, tubeStructure, fMapHitstoCATracklet, 2);
  } else {
    for (int i = 0; i < (mapIterFirst->second).size(); i++) {
      result.fFirstRow.insert(result.fFirstRow.end(), (mapIterFirst->second)[i].begin(), (mapIterFirst->second)[i].end());
    }
    // for (int i = (mapIterFirst->second).size()/2; i < (mapIterFirst->second).size(); i++){
    for (int i = 0; i < (mapIterFirst->second).size(); i++) {
      result.fLastRow.insert(result.fLastRow.end(), (mapIterFirst->second)[i].begin(), (mapIterFirst->second)[i].end());
    }
    // the last and the row with most hits is chosen
    for (auto row : rowMidPair) {
      for (auto group : tubeStructure[row.first]) {
        result.fMidRow.insert(result.fMidRow.end(), group.begin(), group.end());
      }
    }

    // for (int i = 0; i < (mapIterLast->second).size(); i++){
    //  result.fMidRow.insert(result.fMidRow.end(), (mapIterLast->second)[i].begin(), (mapIterLast->second)[i].end());
    //}
  }
  // often the curling track is found partly. In this case the beginning or end is missing
  return result;
}

TripletValues PndCurlingTrackFinder::GetTripletsCurledTracks2()
{
  TripletValues result;
  std::map<int, std::vector<std::vector<PndSttHit *>>> tubeStructure = fFunc.GetTubeStructure(fSttHits, fSttGeoH, fAllHitsCounter, fIsStrongCurling);
  //std::map<int, std::vector<std::vector<PndSttHit *>>> tubeStructure = fFunc.GetTubeStructure(fSttHits, fGeometryMap, fAllHitsCounter, fIsStrongCurling);

  auto mapIterFirst = tubeStructure.begin();
  auto mapIterLast = tubeStructure.end();
  mapIterLast--;
  /*
  std::cout << "lowest row " << mapIterFirst->first;
  for (auto group : mapIterFirst->second) {
    std::cout << "(";
    for (auto hit : group) {
      std::cout << " " << hit->GetEntryNr() << " (" << hit->GetX() << "," << hit->GetY() << "), ";
    }
    std::cout << "), ";
  }
  std::cout << std::endl;
  std::cout << "highes row " << mapIterLast->first;
  for (auto group : mapIterLast->second) {
    std::cout << "(";
    for (auto hit : group) {
      std::cout << " " << hit->GetEntryNr() << " (" << hit->GetX() << "," << hit->GetY() << "), ";
    }
    std::cout << "), ";
  }
  std::cout << std::endl;
  */
  std::vector<std::pair<int, int>> rowFirst;
  std::vector<std::pair<int, int>> rowLast;
  std::vector<std::pair<int, int>> rowMidPair;

  rowFirst.push_back(std::make_pair(mapIterFirst->first, mapIterFirst->second.size()));
  rowLast.push_back(std::make_pair(mapIterFirst->first, mapIterFirst->second.size()));
  rowMidPair.push_back(std::make_pair(mapIterLast->first, mapIterLast->second.size()));

  if (fWithTubeReduction) {
    result.fFirstRow = fFunc.TubeReduction(rowFirst, tubeStructure, fMapHitstoCATracklet, 0);
    result.fLastRow = fFunc.TubeReduction(rowLast, tubeStructure, fMapHitstoCATracklet, 0);
    result.fMidRow = fFunc.TubeReduction(rowMidPair, tubeStructure, fMapHitstoCATracklet, 2);
  } else {
    for (int i = 0; i < (mapIterFirst->second).size(); i++) {
      result.fFirstRow.insert(result.fFirstRow.end(), (mapIterFirst->second)[i].begin(), (mapIterFirst->second)[i].end());
    }
    // for (int i = (mapIterFirst->second).size()/2; i < (mapIterFirst->second).size(); i++){
    for (int i = 0; i < (mapIterFirst->second).size(); i++) {
      result.fLastRow.insert(result.fLastRow.end(), (mapIterFirst->second)[i].begin(), (mapIterFirst->second)[i].end());
    }
    for (int i = 0; i < (mapIterLast->second).size(); i++) {
      result.fMidRow.insert(result.fMidRow.end(), (mapIterLast->second)[i].begin(), (mapIterLast->second)[i].end());
    }
  }

  return result;
}

/**
 * @brief      Determines the index of the tube in the middle between inner and outer tube

 * @param[in]  firstIndex   The index of the first row.
 * @param[in]  lastIndex    The index of the last row.
 * @param[out] midIndex     The index in the middle between first and last row
 */
int PndCurlingTrackFinder::GetMidIndex(int firstIndex, int lastIndex)
{
  if (lastIndex - firstIndex < 2)
    return -1;

  int midIndex = (lastIndex - firstIndex) / 2 + firstIndex; // calculates the mid value between inner and outer row
                                                            //  std::cout << "midIndex: " << midIndex << std::endl;

  if (fSttGeoH->IsSkewedRow(midIndex)) {
  //if (fGeometryMap->IsSkewedRow(midIndex)) {
    if ((7 - firstIndex) < (lastIndex - 16)) {
      midIndex = 16;
    } else {
      midIndex = 7;
    }
  }
  return midIndex;
}
