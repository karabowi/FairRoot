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
//  PndApolloniusTripletTrackFinder
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

#include "PndApolloniusTripletTrackFinder.h"
#include "PndHoughApollonius.h"
#include "PndTrackEvaluatorDetStt.h"

#include "FairLogger.h"

using namespace std;

//PndApolloniusTripletTrackFinder::PndApolloniusTripletTrackFinder(TClonesArray *tubeArray)
//{
  //fGeometryMap = new PndSttGeometryMap(tubeArray, 1);
  //fStrawMap = new PndSttStrawMap(tubeArray);
//}

PndApolloniusTripletTrackFinder::~PndApolloniusTripletTrackFinder() {}

void PndApolloniusTripletTrackFinder::Reset()
{
#ifdef DEBUGApolloniusTripletFinder
  fTripletsFirst.clear();
  fTripletsMid.clear();
  fTripletsLast.clear();
  fTripletsCombi.clear();
  fTripletTracksAfterAdding.clear();
  fContinuousTripletTracks.clear();
  fPreselectedTracks.clear();
#endif

  fTripletTracks.clear();
  fCurlingGroups.clear();
  fSolutions.clear();
  fFinalSolutions.clear();
}

/**
 * @brief      Main method to find the tracks of one event
 *             The algorithm consist of different steps:
 *             1. Preselctions of STT hits (PreselectSttHits)
 *             2. Find rows in STT which are used for the triplet generation (GenerateTriplets)
 *             3. Reduce the number of combinations for the triplets (ReduceCombinatorics)
 *             4. Calculates the Apollonius circles for each triplet (GenerateTripletTracks)
 *             5. Takes only the Apollonius circles which are continuous in the STT (CheckContinuitySolutions)
 *             6. Hits from other detectors are added to the found apollonius circles (AddOtherDetectors)
 *             7. Returns only the best solution of the 8 possible apollonoius circles (FindBestSolutions)
 *             8. Combines identical solutions and reduces the number of tracks to the number of expected tracks in the event (CombineIdenticalSolutions, CheckCombinedSolutions)
 *             9. Finds straight tracks within a curcling track (GenerateCurlingGroups, FindStraightInCurled)
 */
void PndApolloniusTripletTrackFinder::FindTracks()
{
  LOG(debug) << "FindTracks";

  if (fBranchMap.size() == 0) {
    LOG(error) << "No data branches given!";
    return;
  }

  // the steps of the algorithms are numbered like in the description
  // 1. Preselctions of STT hits (PreselectSttHits)
  std::vector<std::vector<PndSttHit *>> groups = PreselectSttHits();

  fMinDistance = 0.;

  std::map<int, std::vector<std::vector<PndSttHit *>>> CombinedTubeStructure;
  // + 2 for additionally check remaining hits after usual trplet finding for:
  //    - streight in curled tracks
  //    - tracks only havin hits in first row
  for (int i = 0; i < groups.size() + 2; i++) {
    std::vector<PndSttHit *> group;
    if (i < groups.size()) {
      group = groups[i];
      if (group.size() < 2) {
        LOG(debug) << "Group too small. Skipped!";
        continue;
      }
    } else if (i >= groups.size()) {
      group = fCurlingGroup;
    }

    LOG(debug) << "Group " << i << "/" << groups.size() << " has " << group.size() << " hits: ";
    std::for_each(group.begin(), group.end(), [](PndSttHit *hit) { LOG(debug) << hit->GetTubeID() << " "; });

    std::vector<TripletSolution> groupSolutions;

    TripletValues triplets;

    if (i < groups.size()) {
      // 2. Find rows in STT which are used for the triplet generation (GenerateTriplets)
      triplets = GenerateTriplets(group);

      if (fAllHitsCounter > 150) {
        LOG(debug) << "Group too big. Skipped!";
        continue;
      }
    } else if (i == groups.size()) {
      // 9. Finds straight tracks within a curcling track (GenerateCurlingGroups, FindStraightInCurled)
      fMinDistance = 5.;
      LOG(debug) << "streight in curled";
      CombinedTubeStructure = GenerateCurlingGroups();
      group = fCurlingGroup;
      if (group.size() < 2) {
        LOG(debug) << "Group too small. Skipped!";
        continue;
      }
      triplets = FindTubesForStraightInCurled(CombinedTubeStructure);
      fIsStrongCurling = kFALSE;
    } else if (i == groups.size() + 1) {
      LOG(debug) << "tracks in first row";

      if (group.size() < 2) {
        LOG(debug) << "Group too small. Skipped!";
        continue;
      }
      triplets = FindTubesForTracksInFirstRow(CombinedTubeStructure);
    }

    LOG(debug) << "Generated Triplets: " << triplets << std::endl;
    if (fIsStrongCurling) {
      LOG(debug) << "is strong curling";
      continue;
    }

    // 3. Reduce the number of combinations for the triplets
    std::vector<Triplet> tripletIndices =
      fFunc.ReduceCombinatorics(triplets, fMapHitstoCATracklet, fSttBranchOriginal, fCATrackFinder, fSttGeoH, fMinDistance, fWithCombiReduction);
      //fFunc.ReduceCombinatorics(triplets, fMapHitstoCATracklet, fSttBranchOriginal, fCATrackFinder, fGeometryMap, fMinDistance, fWithCombiReduction);
    for (auto triplet : tripletIndices) {
      LOG(debug) << "Solution for triplet: " << triplet.fTripletHits[0]->GetTubeID() << "/" << triplet.fTripletHits[1]->GetTubeID() << "/" << triplet.fTripletHits[2]->GetTubeID();

      if (fFunc.IsTripletUsed(groupSolutions, triplet)) {
        LOG(debug) << "Triplet used";
        continue;
      }

#ifdef DEBUGApolloniusTripletFinder
      TripletSolution tCombi;
      std::vector<PndSttHit *> TripletVec = {triplet.fTripletHits[0], triplet.fTripletHits[1], triplet.fTripletHits[2]};
      tCombi.AddHits(TripletVec);
      fTripletsCombi.push_back(tCombi);
#endif
      // 4. Calculates the Apollonius circles for each triplet (GenerateTripletTracks)
      std::vector<TripletSolution> tripletSolutions;
      if (i >= groups.size()) {
        // take only relevant hit range
        // std::cout << "triplet: " << triplet << std::endl;
        std::vector<int> sectors;
        sectors.clear();
        for (int i = 0; i < 3; i++) {
          int sectorId = fSttGeoH->GetSector(triplet.fTripletHits[i]->GetTubeID());
          //int sectorId = fStrawMap->GetSector(triplet.fTripletHits[i]->GetTubeID());
          if (std::find(sectors.begin(), sectors.end(), sectorId) == sectors.end()) {
            sectors.push_back(sectorId);
          }
        }

        std::vector<PndSttHit *> subGroup;
        for (int i = 0; i < fCurlingGroup.size(); i++) {
          if (std::find(sectors.begin(), sectors.end(), fSttGeoH->GetSector(fCurlingGroup[i]->GetTubeID())) != sectors.end()) {
          //if (std::find(sectors.begin(), sectors.end(), fStrawMap->GetSector(fCurlingGroup[i]->GetTubeID())) != sectors.end()) {
            subGroup.push_back(fCurlingGroup[i]);
          }
        }
        if (subGroup.size() > 100) {
          LOG(debug) << "Group too big. Skipped!";
          continue;
        }

        tripletSolutions = fFunc.GenerateTripletTracks(triplet, subGroup, fSttGeoH);
        //tripletSolutions = fFunc.GenerateTripletTracks(triplet, subGroup, fGeometryMap);
      } else if (i < groups.size()) {
        tripletSolutions = fFunc.GenerateTripletTracks(triplet, group, fSttGeoH);
        //tripletSolutions = fFunc.GenerateTripletTracks(triplet, group, fGeometryMap);
      }

#ifdef DEBUGApolloniusTripletFinder
      for (auto solution : tripletSolutions) {
        solution.SortAllHits(fSttGeoH);
        //solution.SortAllHits(fGeometryMap);
        std::cout << "fTripletTracks: " << solution.fAllHits.size() << std::endl;
        fTripletTracks.push_back(solution);
      }
#endif

      // 5. Takes only the Apollonius circles which are continuous in the STT (CheckContinuitySolutions)
      fFunc.CheckContinuitySolutions(tripletSolutions, fSttGeoH);
      //fFunc.CheckContinuitySolutions(tripletSolutions, fGeometryMap);

#ifdef DEBUGApolloniusTripletFinder
      for (auto solution : tripletSolutions) {
        solution.SortAllHits(fSttGeoH);
        //solution.SortAllHits(fGeometryMap);
        std::cout << "fContinuousTripletTracks: " << solution.fAllHits.size() << std::endl;
        fContinuousTripletTracks.push_back(solution);
      }
#endif

      // 6. Hits from other detectors are added to the found apollonius circles (AddOtherDetectors)
      fFunc.AddOtherDetectors(tripletSolutions, fSttGeoH, fBranchMap, fSTT);
      //fFunc.AddOtherDetectors(tripletSolutions, fStrawMap, fBranchMap, fSTT);

#ifdef DEBUGApolloniusTripletFinder
      for (auto solution : tripletSolutions) {
        solution.SortAllHits(fSttGeoH);
        //solution.SortAllHits(fGeometryMap);
        std::cout << "fTripletTracksAfterAdding: " << solution.fAllHits.size() << std::endl;
        fTripletTracksAfterAdding.push_back(solution);
      }
#endif

      // 7. Returns only the best solution of the 8 possible apollonoius circles (FindBestSolutions)
      std::vector<TripletSolution> bestSolutions = fFunc.FindBestSolutions(tripletSolutions);
      groupSolutions.insert(groupSolutions.end(), bestSolutions.begin(), bestSolutions.end());
    }

    LOG(debug) << "Grouped Solutions: ";
    for (auto solution : groupSolutions) {
      LOG(debug) << solution;
    }

    // 8. Combines identical solutions and reduces the number of tracks to the number of expected tracks in the event (CombineIdenticalSolutions, CheckCombinedSolutions)
    std::vector<TripletSolution> differentSolutions = fFunc.CombineIdenticalSolutions(groupSolutions);
    LOG(debug) << "Different Solutions: ";
    for (auto solution : differentSolutions) {
      LOG(debug) << solution;
    }
    std::vector<TripletSolution> combinedSolutions = fFunc.CheckCombinedSolutions(differentSolutions, fNExpectedTracks);

    if (i >= groups.size()) {
#ifdef DEBUGApolloniusTripletFinder
      std::cout << "CombinedTubeStructure before" << std::endl;
      for (auto row : CombinedTubeStructure) {
        std::cout << row.first << " : ";
        for (auto group : row.second) {
          std::cout << "(";
          for (auto straw : group) {
            std::cout << straw->GetTubeID() << "/";
          }
          std::cout << ") ";
        }
        std::cout << std::endl;
      }
      std::cout << std::endl;
#endif
      DeleteFromCurlingPool(combinedSolutions, CombinedTubeStructure);
    }

    LOG(debug) << "Combined Solutions: ";
    for (auto solution : combinedSolutions) {
      if (solution.GetNHits() < 5)
        continue;
      solution.SortAllHits(fSttGeoH);
      //solution.SortAllHits(fGeometryMap);
      LOG(debug) << solution;
      fSolutions.push_back(solution);
    }
#ifdef DEBUGApolloniusTripletFinder
    TripletSolution tFirst;
    TripletSolution tMid;
    TripletSolution tLast;
    tFirst.AddHits(triplets.fFirstRow);
    tMid.AddHits(triplets.fMidRow);
    tLast.AddHits(triplets.fLastRow);
    fTripletsFirst.push_back(tFirst);
    fTripletsMid.push_back(tMid);
    fTripletsLast.push_back(tLast);
#endif
  }
  fFinalSolutions = fFunc.CombineIdenticalSolutionsFinal(fSolutions);

  LOG(debug) << "fFinalSolutions: ";
  for (auto solution : fFinalSolutions) {
    solution.SortAllHits(fSttGeoH);
    //solution.SortAllHits(fGeometryMap);
    LOG(debug) << solution;
  }
  LOG(debug) << std::endl;
}

/**
 * @brief      Grouping of hits which could belong to one track
 *
 * @param[in]  SttHits      TClonesArray with all STT hits
 * @param[out] result       A vector of vectors of STTHits, where the SttHits of one vector belong to the same group of hits and could belong to one track
 */
std::vector<std::vector<PndSttHit *>> PndApolloniusTripletTrackFinder::PreselectSttHits()
{
  fMapHitstoCATracklet.clear();

  vector<vector<PndSttHit *>> result;
  result = fPreselector->PreselectSttHits();

  LOG(debug) << "PreselectedSttHits: ";
  int groupIndex = 0;
  for (auto group : result) {
    LOG(debug) << "Group " << groupIndex++;
    for (auto hit : group) {
      LOG(debug) << hit->GetTubeID() << "/";
      // std::cout << "row: " << fGeometryMap->GetRow(hit->GetTubeID()) << std::endl;
    }
  }

  fMapHitstoCATracklet = fFunc.GetHitsToCAMap(fCATrackFinder);
  return result;
}

/**
 * @brief      Generates possible triplets.
 *
 * @param[in]  hits         STTHits of one preselected group
 * @param[out] result       Finds all inner, mid and outer tubes of the group of hits. The three vectors containing the tubes are saved in a structure "TripletValues".
 */
TripletValues PndApolloniusTripletTrackFinder::GenerateTriplets(std::vector<PndSttHit *> hits)
{
  TripletValues result;
  std::map<int, std::vector<std::vector<PndSttHit *>>> tubeStructureByRow = fFunc.GetTubeStructure(hits, fSttGeoH, fAllHitsCounter, fIsStrongCurling);
  //std::map<int, std::vector<std::vector<PndSttHit *>>> tubeStructureByRow = fFunc.GetTubeStructure(hits, fGeometryMap, fAllHitsCounter, fIsStrongCurling);

#ifdef DEBUGApolloniusTripletFinder
  std::cout << "TubeStructure" << std::endl;
  for (auto row : tubeStructureByRow) {
    std::cout << row.first << " : ";
    for (auto group : row.second) {
      std::cout << "(";
      for (auto straw : group) {
        std::cout << straw->GetTubeID() << "/";
      }
      std::cout << ") ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
#endif

  if (fIsStrongCurling) {
    fCurlingGroups.push_back(tubeStructureByRow);
    return result;
  }

  std::vector<int> curlsInRow = CheckForCurlingTracks(tubeStructureByRow);
  if (curlsInRow.size() > 0) {
    LOG(debug) << "curling track found";
    if (!fWithCurlingTracks) {
      fCurlingGroups.push_back(tubeStructureByRow);
      return result;
    }
  }
  //  if (curlsInRow.size() > 0) {
  //    result += GetTripletsCurledTracks(curlsInRow, tubeStructureByRow);
  //  }
  result = GetTripletsStraightTracks(tubeStructureByRow);

  return result;
}

/**
 * @brief      If curling tracks are detected, the tracks are often divided into several preselected groups. This method merges these groups
 *
 * @param[out] CombinedTubeStructure       A map containing the tube structure of the curling track
 */
std::map<int, std::vector<std::vector<PndSttHit *>>> PndApolloniusTripletTrackFinder::GenerateCurlingGroups()
{
  std::map<int, std::vector<std::vector<PndSttHit *>>> CombinedTubeStructure;
  fCurlingGroup.clear();
  if (fCurlingGroups.size() == 0)
    return CombinedTubeStructure;

  CombinedTubeStructure = fCurlingGroups[0];
  for (int i = 1; i < fCurlingGroups.size(); i++) {
    for (auto it = fCurlingGroups[i].begin(); it != fCurlingGroups[i].end(); it++) {
      if (CombinedTubeStructure.find(it->first) == CombinedTubeStructure.end()) {
        // row has to be added as new element into tubestructure
        CombinedTubeStructure[it->first] = it->second;
      } else {
        // row is already in tubestructure and has to be extended
        CombinedTubeStructure[it->first].insert(CombinedTubeStructure[it->first].end(), it->second.begin(), it->second.end());
      }
    }
  }
  for (auto it = CombinedTubeStructure.begin(); it != CombinedTubeStructure.end(); it++) {
    for (auto group : it->second) {
      for (auto hit : group)
        fCurlingGroup.push_back(hit);
    }
  }
  return CombinedTubeStructure;
}

/**
 * @brief      Method to find streight tracks within a curling track
 *
 * @param[in] combinedSolutions         A vector containing the already found tracks
 * @param[in] CombinedTubeStructure     A map containing the tube structure of the curling track and possible streight tracks that can't be distinguished from the curling track
 * @param[out] CombinedTubeStructure    A map containing the tube structure of the curling track. The hits belonging to the streight tracks are deleted.
 */
void PndApolloniusTripletTrackFinder::DeleteFromCurlingPool(std::vector<TripletSolution> &combinedSolutions,
                                                            std::map<int, std::vector<std::vector<PndSttHit *>>> &CombinedTubeStructure)
{

  for (auto solution : combinedSolutions) {
    for (auto hit : solution.fHits[TripletSolution::detID::STT]) {
      PndSttHit *sttHit = (PndSttHit *)hit;
      int row = fSttGeoH->GetRow(sttHit->GetTubeID());
      //int row = fGeometryMap->GetRow(sttHit->GetTubeID());
      fCurlingGroup.erase(std::remove(fCurlingGroup.begin(), fCurlingGroup.end(), sttHit), fCurlingGroup.end());

      if (CombinedTubeStructure[row].size() == 1 && CombinedTubeStructure[row][0].size() < 2) {
        CombinedTubeStructure.erase(row);
      } else {
        for (int i = 0; i < CombinedTubeStructure[row].size(); i++) {
          CombinedTubeStructure[row][i].erase(std::remove(CombinedTubeStructure[row][i].begin(), CombinedTubeStructure[row][i].end(), sttHit), CombinedTubeStructure[row][i].end());
          if (CombinedTubeStructure[row][i].size() == 0) {
            CombinedTubeStructure[row].erase(CombinedTubeStructure[row].begin() + i);
          }
        }
      }
      //if (CombinedTubeStructure[fGeometryMap->GetRow(sttHit->GetTubeID())].size() == 1 && CombinedTubeStructure[fGeometryMap->GetRow(sttHit->GetTubeID())][0].size() < 2) {
        //CombinedTubeStructure.erase(fGeometryMap->GetRow(sttHit->GetTubeID()));
      //} else {
        //for (int i = 0; i < CombinedTubeStructure[fGeometryMap->GetRow(sttHit->GetTubeID())].size(); i++) {
          //CombinedTubeStructure[fGeometryMap->GetRow(sttHit->GetTubeID())][i].erase(std::remove(CombinedTubeStructure[fGeometryMap->GetRow(sttHit->GetTubeID())][i].begin(),
                                                                                                //CombinedTubeStructure[fGeometryMap->GetRow(sttHit->GetTubeID())][i].end(), sttHit),
                                                                                    //CombinedTubeStructure[fGeometryMap->GetRow(sttHit->GetTubeID())][i].end());
          //if (CombinedTubeStructure[fGeometryMap->GetRow(sttHit->GetTubeID())][i].size() == 0) {
            //CombinedTubeStructure[fGeometryMap->GetRow(sttHit->GetTubeID())].erase(CombinedTubeStructure[fGeometryMap->GetRow(sttHit->GetTubeID())].begin() + i);
          //}
        //}
      //}
    }
  }
}

/**
 * @brief      Method to find streight tracks within a curling track
 *
 * @param[in] CombinedTubeStructure     A map containing the tube structure of the curling track and possible streight tracks that can't be distinguished from the curling track
 * @param[out] result                   A set of inner, outer and mid hits that possibly could belong to a streight track within a curling track.
 */
TripletValues PndApolloniusTripletTrackFinder::FindTubesForStraightInCurled(std::map<int, std::vector<std::vector<PndSttHit *>>> &CombinedTubeStructure)
{

  TripletValues result;

  if (fCurlingGroups.size() == 0)
    return result;

  int first = 0;
  int last = 0;
  int AddToLast = 0;

  // first and last row are hit. it could be a streigt track inside a curling track
  std::vector<std::pair<int, int>> rowFirst;
  std::vector<std::pair<int, int>> rowLast;
  std::vector<std::pair<int, int>> rowMidPair;
  std::vector<int> rowMid;
  // check first row
  for (auto firstIter = CombinedTubeStructure.begin(); firstIter != CombinedTubeStructure.end(); firstIter++) {
    if (firstIter->first == fFirstRowNumber) {
      rowFirst.push_back(std::make_pair(firstIter->first, firstIter->second.size()));
      first = firstIter->second.size();
      if (rowFirst.size() > 0)
        break;
    }
  }
  // check "last" row --> row >= 20
  for (auto lastIter = CombinedTubeStructure.rbegin(); lastIter != CombinedTubeStructure.rend(); lastIter++) {
    if (lastIter->first >= fLastRowNumber) {
      if (lastIter->second.size() > last) {
        rowLast.push_back(std::make_pair(lastIter->first, lastIter->second.size()));
        last = lastIter->second.size();
      }
    } else if (lastIter->first >= 18) {
      for (int i = 0; i < lastIter->second.size(); i++) {
        for (int j = 0; j < lastIter->second[i].size(); j++) {
          if (fSttGeoH->IsEdgeStraw(lastIter->second[i][j]->GetTubeID())) {
          //if (fStrawMap->IsEdgeStraw(lastIter->second[i][j]->GetTubeID())) {
            result.fLastRow.push_back(lastIter->second[i][j]);
            AddToLast += 1;
          }
        }
      }
    }
  }

  fNExpectedTracks = last + AddToLast;

  if (rowFirst.size() == 0 || rowLast.size() == 0)
    return result;

  auto frowIter = rowFirst.begin();
  // Get Mid Tubes
  for (auto lrow : rowLast) {
    for (auto frow = frowIter; frow != rowFirst.end(); frow++) {
      // std::cout << lrow.first << "/" << lrow.second << "-" << GetMidIndex(frow->first, lrow.first) << "-" << frow->first << "/" << frow->second << " ";
      rowMid.push_back(GetMidIndex(frow->first, lrow.first));
      rowMidPair.push_back(std::make_pair(GetMidIndex(frow->first, lrow.first), 0));

      if (lrow.second < frow->second) {
        frowIter = frow;
        break;
      }
      if (lrow.second == frow->second) {
        continue;
      }
    }
  }
  if (fWithTubeReduction) {
    result.fFirstRow = fFunc.TubeReduction(rowFirst, CombinedTubeStructure, fMapHitstoCATracklet, 0);
  } else {
    for (auto row : rowFirst) {
      for (auto group : CombinedTubeStructure[row.first])
        result.fFirstRow.insert(result.fFirstRow.end(), group.begin(), group.end());
    }
  }

  if (fWithTubeReduction) {
    auto group = fFunc.TubeReduction(rowLast, CombinedTubeStructure, fMapHitstoCATracklet, 2);
    result.fLastRow.insert(result.fLastRow.end(), group.begin(), group.end());
  } else {
    for (auto row : rowLast) {
      for (auto group : CombinedTubeStructure[row.first])
        result.fLastRow.insert(result.fLastRow.end(), group.begin(), group.end());
    }
  }

  if (fWithTubeReduction) {
    result.fMidRow = fFunc.TubeReduction(rowMidPair, CombinedTubeStructure, fMapHitstoCATracklet, 1);
  } else {
    for (auto row : rowMid) {
      for (auto group : CombinedTubeStructure[row]) {
        if (group.size() < 6)
          result.fMidRow.insert(result.fMidRow.end(), group.begin(), group.end());
      }
    }
  }

  return result;
}

TripletValues PndApolloniusTripletTrackFinder::FindTubesForTracksInFirstRow(std::map<int, std::vector<std::vector<PndSttHit *>>> &TubeStructure)
{

  TripletValues result;

  auto mapIterFirst = TubeStructure.begin();

  if ((mapIterFirst->first == 0 && mapIterFirst->second.size() == 1) || (mapIterFirst->first == 0 && mapIterFirst->second.size() == 3)) {
    for (int i = 0; i < mapIterFirst->second.size(); i++) {
      if (mapIterFirst->second[i].size() > 4) {
        result.fFirstRow.push_back(mapIterFirst->second[i][0]);
        result.fLastRow.push_back(mapIterFirst->second[i][mapIterFirst->second[i].size() - 1]);
        result.fMidRow.push_back(mapIterFirst->second[i][mapIterFirst->second[i].size() / 2]);
      }
    }
  }

  fNExpectedTracks = result.fFirstRow.size();

  return result;
}

/**
 * @brief      This is a check for curling tracks. If tracks have many hits in one row, they are declared as curling track
 *             ToDo: This method has to be finished
 * @param[in]  tubeStructure       A map connecting each row with groups of STT hits. If there are hits in one row that are adjacent these hits are declared as a group
 * @param[out] result
 */
std::vector<int> PndApolloniusTripletTrackFinder::CheckForCurlingTracks(std::map<int, std::vector<std::vector<PndSttHit *>>> &tubeStructure)
{
  std::vector<int> result;
  for (auto row : tubeStructure) {
    for (auto range : row.second) {
      if (range.size() > 3) {
        result.push_back(row.first);
      }
    }
  }
  return result;
}

/**
 * @brief      Finds all inner, mid and outer tubes of the group of hits. The three vectors containing the tubes are saved in a structure "TripletValues".
 * @param[in]  tubeStructure  A map connecting each row with groups of STT hits. If there are hits in one row that are adjacent these hits are declared as a group
 * @param[out] result         Returns all inner, mid and outer tubes of the group of hits. The three vectors containing the tubes are saved in a structure "TripletValues".
 */
TripletValues PndApolloniusTripletTrackFinder::GetTripletsStraightTracks(std::map<int, std::vector<std::vector<PndSttHit *>>> &tubeStructure)
{
  TripletValues result;

  std::vector<std::pair<int, int>> rowLast;
  std::vector<std::pair<int, int>> rowFirst;
  std::vector<std::pair<int, int>> rowMidPair;
  std::vector<int> rowMid;

  int last = 0;

  for (auto lastIter = tubeStructure.rbegin(); lastIter != tubeStructure.rend(); lastIter++) {
    if (fSttGeoH->IsSkewedRow(lastIter->first)) {
    //if (fGeometryMap->IsSkewedRow(lastIter->first)) {
      continue;
    }
    if (lastIter->second.size() > last) {
      rowLast.push_back(std::make_pair(lastIter->first, lastIter->second.size()));
      last = lastIter->second.size();
    }
    //    if (lastIter->first == 7) { // always add the row before the skewed layers
    //      rowLast.push_back(std::make_pair(lastIter->first, lastIter->second.size()));
    //    }
  }
  LOG(debug) << "nExpectedTracks: " << last;
  fNExpectedTracks = last;

  int first = 0;
  for (auto firstIter = tubeStructure.begin(); firstIter != tubeStructure.end(); firstIter++) {
    if (fSttGeoH->IsSkewedRow(firstIter->first)) {
    //if (fGeometryMap->IsSkewedRow(firstIter->first)) {
      continue;
    }
    if (firstIter->second.size() > first) {
      rowFirst.push_back(std::make_pair(firstIter->first, firstIter->second.size()));
      first = firstIter->second.size();
    }
  }

  LOG(debug) << "GetTripletsStraightTracks: lastRows";
  for (auto row : rowLast) {
    LOG(debug) << row.first << "/" << row.second;
  }

  LOG(debug) << "GetTripletsStraightTracks: firstRows";
  for (auto row : rowFirst) {
    LOG(debug) << row.first << "/" << row.second;
  }
  auto frowIter = rowFirst.begin();

  LOG(debug) << "Combinations: ";
  for (auto lrow : rowLast) {
    for (auto frow = frowIter; frow != rowFirst.end(); frow++) {
      LOG(debug) << lrow.first << "/" << lrow.second << "-" << GetMidIndex(frow->first, lrow.first) << "-" << frow->first << "/" << frow->second << " ";
      rowMid.push_back(GetMidIndex(frow->first, lrow.first));
      rowMidPair.push_back(std::make_pair(GetMidIndex(frow->first, lrow.first), 0));

      if (lrow.second < frow->second) {
        frowIter = frow;
        break;
      }
      if (lrow.second == frow->second) {
        frowIter = ++frow;
        break;
      }
    }
  }
  if (fWithTubeReduction) {
    result.fFirstRow = fFunc.TubeReduction(rowFirst, tubeStructure, fMapHitstoCATracklet, 0);
  } else {
    for (auto row : rowFirst) {
      for (auto group : tubeStructure[row.first]) {
        result.fFirstRow.insert(result.fFirstRow.end(), group.begin(), group.end());
      }
    }
  }

  if (fWithTubeReduction) {
    result.fLastRow = fFunc.TubeReduction(rowLast, tubeStructure, fMapHitstoCATracklet, 2);
  } else {
    for (auto row : rowLast) {
      for (auto group : tubeStructure[row.first]) {
        result.fLastRow.insert(result.fLastRow.end(), group.begin(), group.end());
      }
    }
  }

  if (fWithTubeReduction) {
    result.fMidRow = fFunc.TubeReduction(rowMidPair, tubeStructure, fMapHitstoCATracklet, 1);
  } else {
    for (auto row : rowMid) {
      for (auto group : tubeStructure[row]) {
        result.fMidRow.insert(result.fMidRow.end(), group.begin(), group.end());
      }
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
int PndApolloniusTripletTrackFinder::GetMidIndex(int firstIndex, int lastIndex)
{
  if (lastIndex - firstIndex < 2)
    return -1;

  int midIndex = (lastIndex - firstIndex) / 2 + firstIndex; // calculates the mid value between inner and outer row
                                                            //  std::cout << "midIndex: " << midIndex << std::endl;

  if (fSttGeoH->IsSkewedRow(midIndex)) {
  //if (fGeometryMap->IsSkewedRow(midIndex)) {
    if ((fBeforeSkewedRowNumber - firstIndex) < (lastIndex - fAfterSkewedRowNumber)) {
      midIndex = fAfterSkewedRowNumber;
    } else {
      midIndex = fBeforeSkewedRowNumber;
    }
  }
  return midIndex;
}
