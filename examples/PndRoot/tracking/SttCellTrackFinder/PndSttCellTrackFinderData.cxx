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
 * PndSttCellTrackFinderData.cpp
 *
 *
 *  Created on: May 8, 2014
 *      Author: schumann
 */

#include "PndSttCellTrackFinderTask.h" // J.R. 20/04-2018
#include "PndSttCellTrackFinderData.h"
#include "PndStt2GeoHandler.h"
//#include "PndSttStrawMap.h"
//#include "PndSttGeometryMap.h"
#include "PndSttTube.h"
#include "PndSttHit.h"
#include "PndSttSkewedHit.h"


#include "FairRootManager.h"

#include <cmath>
#include <stdio.h>
// macro for printing tubes + neighbors to a file called tubeNeighborings.txt
//#define PRINT_STT_NEIGHBORS

using namespace std;

ClassImp(PndSttCellTrackFinderData);

PndSttCellTrackFinderData::PndSttCellTrackFinderData(PndStt2GeoHandler *geoH) : fAllowDoubleHits(kFALSE), fNumHits(0), fNumHitsWithoutDouble(0), fRunTimeBased(kFALSE), fSttGeoH(geoH)
//PndSttCellTrackFinderData::PndSttCellTrackFinderData(TClonesArray *sttTubeArray) : fAllowDoubleHits(kFALSE), fNumHits(0), fNumHitsWithoutDouble(0), fRunTimeBased(kFALSE)
{

  // Generate information of Straw- and GeometryMap.
  // It is always the same data for all events.

  //fStrawMap = new PndSttStrawMap(sttTubeArray);
  //fGeometryMap = new PndSttGeometryMap(sttTubeArray, 1);

  PndSttTube *tube;
  TVector3 pos;

#ifdef PRINT_STT_NEIGHBORS
  TArrayI neighbors;
  FILE *fp = fopen("tubeNeighborings.txt", "w");
  int minNumNeigh = 100, maxNumNeigh = 0, numSkewed = 0, numSkewedLessNeig = 0;
  bool skewed;
#endif

  TClonesArray *sttTubeArray = geoH->GetTubeArray();
  for (int i = 1; i < sttTubeArray->GetEntriesFast(); ++i) {
    tube = (PndSttTube *)sttTubeArray->At(i);
    pos = tube->GetPosition();
    fMapTubeIdToPos[i] = pos;

#ifdef PRINT_STT_NEIGHBORS
    neighbors = fSttGeoH->GetNeighborings(tube->GetTubeID());
    skewed = fSttGeoH->IsSkewedStraw(tube->GetTubeID());
    //neighbors = fGeometryMap->GetNeighboringsByMap(tube->GetTubeID());
    //skewed = fGeometryMap->IsSkewedStraw(tube->GetTubeID());
    if (skewed)
      ++numSkewed;
    if (skewed && neighbors.GetSize() < 7)
      ++numSkewedLessNeig;
    fprintf(fp, "%d %4i:", skewed, tube->GetTubeID());
    for (int j = 0; j < neighbors.GetSize(); ++j) {
      fprintf(fp, " %4i", neighbors[j]);
    }
    fprintf(fp, "\n");

    if (minNumNeigh > neighbors.GetSize())
      minNumNeigh = neighbors.GetSize();
    if (maxNumNeigh < neighbors.GetSize())
      maxNumNeigh = neighbors.GetSize();
#endif
  }

#ifdef PRINT_STT_NEIGHBORS
  fprintf(fp, "max/min number of neighbors: %3i / %3i\n", maxNumNeigh, minNumNeigh);
  fprintf(fp, "number of skewed straws: %3i \n", numSkewed);
  fprintf(fp, "number of skewed straws with less than 7 neighbors: %3i \n", numSkewedLessNeig);
  fclose(fp);
#endif
}

void PndSttCellTrackFinderData::AddHits(TClonesArray *hits, TString branchName)
{

  PndSttHit *myHit;
  FairLink myID;

  Int_t branchId = FairRootManager::Instance()->GetBranchId("STTHit");

  // J.R. The below is a temporary workaround for being able to use sorted Stt hits
  if (branchName.CompareTo("STTSortedHits") == 0) {
    branchId = FairRootManager::Instance()->GetBranchId("STTSortedHits");
  }
  if (branchName.CompareTo("STTFilteredHits") == 0) {
    branchId = FairRootManager::Instance()->GetBranchId("STTFilteredHits");
  }

  // only STTHits are passed to this functions, but we have to check if SkewedHits are present.

  if (branchName.Contains("skewed", TString::kIgnoreCase)) {
    // I'm not sure if this part of the code really does what it should!
    // since skewed straw tubes are not really used at the moment I can not tell.

    fCombinedSkewedHits.clear();

    for (int i = 0; i < hits->GetEntries(); i++) {
      PndSttSkewedHit *skewedHit = (PndSttSkewedHit *)(hits->At(i));
      int tubeId = skewedHit->GetTubeIDs().first;
      fCombinedSkewedHits.insert(std::pair<int, PndSttSkewedHit *>(tubeId, skewedHit));
      if (skewedHit->GetEntryNr().GetIndex() < 0) {
        myID = FairLink(branchId, i);
        skewedHit->SetEntryNr(FairLink(branchId, i));
      } else
        myID = skewedHit->GetEntryNr();
    }

  } else { //"normal" stt htis are added to the Data map

    fMapHitToFairLinkOrig.clear();
    fHitsOrig.clear();

    for (int i = 0; i < hits->GetEntries(); i++) {
      myHit = (PndSttHit *)(hits->At(i));

      if (myHit->GetEntryNr().GetIndex() < 0) {
        myID = FairLink(branchId, i);
        myHit->SetEntryNr(FairLink(branchId, i));
      } else
        myID = myHit->GetEntryNr();
      myHit->SetDxyz(myHit->GetIsochrone(), myHit->GetIsochrone(), 100);
      fMapHitToFairLinkOrig[i] = myID;
      fHitsOrig.push_back((FairHit *)myHit);
    }
  }

  // std::cout << "Number of hits in original TClonesArray: " << hits->GetEntries() << std::endl;
  // std::cout << "Number of hits which are added: " << fHitsOrig.size() << std::endl;
}

void PndSttCellTrackFinderData::GenerateNeighborhoodData()
{

  // std::cout << "PndSttCellTrackFinderData::GenerateNeighborhoodData " << std::endl;
  // fill set with tubeIDs to remove double hits
  int tubeId;
  set<int> sttHits;
  vector<FairHit *> hitsWithoutDouble;
  map<int, FairLink> mapWithoutDouble;
  int hitIndex = 0;

  for (size_t i = 0; i < fHitsOrig.size(); ++i) {

    // std::cout << "TubeId: " << ((PndSttHit*) fHitsOrig[i])->GetTubeID() << ", Time: " << ((PndSttHit*) fHitsOrig[i])->GetTimeStamp() << std::endl;

    tubeId = ((PndSttHit *)fHitsOrig[i])->GetTubeID();
    if (sttHits.find(tubeId) == sttHits.end()) {
      sttHits.insert(tubeId);
      hitsWithoutDouble.push_back(fHitsOrig[i]);
      mapWithoutDouble[hitIndex] = fMapHitToFairLinkOrig[i];
      ++hitIndex;
    }
  }

  fNumHits = fHitsOrig.size();
  fNumHitsWithoutDouble = sttHits.size();

  if (!fAllowDoubleHits) {
    fHits = hitsWithoutDouble;
    fMapHitToFairLink = mapWithoutDouble;

  } else {
    fHits = fHitsOrig;
    fMapHitToFairLink = fMapHitToFairLinkOrig;
  }

  PndSttHit *sttHit;
  for (size_t i = 0; i < fHits.size(); ++i) {
    sttHit = (PndSttHit *)fHits[i];

    fMapTubeIdToHit[sttHit->GetTubeID()] = i;
  }

  if (fRunTimeBased == kFALSE) {
    FindHitNeighborsEventBased();
  } else {
    FindHitNeighborsTimeBased();
  }

  SeparateNeighbors();
}

void PndSttCellTrackFinderData::FindHitNeighborsEventBased()
{ // if not def RunTimeBased

  /* Approach: At first create a set of the tubeIDs of all hits.
   * Then get the neighbors of each hit/tube and store only those
   * that are included in the set.*/

  PndSttHit *sttHit;
  int tubeId;
  set<int> hitIds;

  // initialize set with straw-ids of hits
  for (size_t i = 0; i < fHits.size(); ++i) {
    sttHit = (PndSttHit *)fHits[i];
    hitIds.insert(sttHit->GetTubeID());
  }

  // fill fHitNeighbors
  for (size_t i = 0; i < fHits.size(); ++i) {
    sttHit = (PndSttHit *)fHits[i];
    tubeId = sttHit->GetTubeID();
    // get neighbors
    TArrayI neighbors = fSttGeoH->GetNeighborings(tubeId);
    //TArrayI neighbors = fGeometryMap->GetNeighboringsByMap(tubeId);

    for (int j = 0; j < neighbors.GetSize(); ++j) {

      // set contains neighbor?
      if (hitIds.find(neighbors[j]) != hitIds.end()) {

        // add to map with all neighbors
        fHitNeighbors[tubeId].push_back(neighbors[j]);

        // actual tube and neighbor are not both on the edge of the stt?
        if (!(fSttGeoH->IsEdgeStraw(tubeId) && fSttGeoH->IsEdgeStraw(neighbors[j]))) {
        //if (!(fStrawMap->IsEdgeStraw(tubeId) && fStrawMap->IsEdgeStraw(neighbors[j]))) {

          // add to the other map
          fHitNeighborsWithoutEdges[tubeId].push_back(neighbors[j]);
        }
      }
    }

    if (fHitNeighbors.find(tubeId) == fHitNeighbors.end()) {
      // no hitNeighbor was found
      vector<int> tmp;
      fHitNeighbors[tubeId] = tmp;
    }

    if (fHitNeighborsWithoutEdges.find(tubeId) == fHitNeighborsWithoutEdges.end()) {
      // no hitNeighbor was found
      vector<int> tmp;
      fHitNeighborsWithoutEdges[tubeId] = tmp;
    }

    if (!fSttGeoH->IsSkewedStraw(tubeId)) {
    //if (!fStrawMap->IsSkewedStraw(tubeId)) {
      for (int j = 0; j < neighbors.GetSize(); ++j) {
        if (hitIds.find(neighbors[j]) != hitIds.end() && !fSttGeoH->IsSkewedStraw(neighbors[j])) {
        //if (hitIds.find(neighbors[j]) != hitIds.end() && !fStrawMap->IsSkewedStraw(neighbors[j])) {
          fHitNeighborsWithoutSkewed[tubeId].push_back(neighbors[j]);
        }
      }
    }
  }
}

void PndSttCellTrackFinderData::FindHitNeighborsTimeBased()
{ // if def RunTimeBased

  /* J.R. Adopted from PndSttCellTrackFinderData::FindHitNeighborsEventBased
   *  Added conditions to check timestamps and perform
   * time clustering. 28/03-2018*/

  PndSttHit *sttHit;
  int tubeId;
  set<int> hitIds;

  // Time difference chosen since drift time of electrons is 200 ns so signals in neighboring subes can have tis delay
  double sttHitTimeStamp;      // Timestamp of stt hit for time clustering
  double sttNeighborTimeStamp; // Timestamp of neighbor for time clustering

  // initialize set with straw-ids of hits
  for (size_t i = 0; i < fHits.size(); ++i) {
    sttHit = (PndSttHit *)fHits[i];

    hitIds.insert(sttHit->GetTubeID());
  }

  // fill fHitNeighbors
  for (size_t i = 0; i < fHits.size(); ++i) {

    sttHit = (PndSttHit *)fHits[i];
    tubeId = sttHit->GetTubeID();

    // sttHitTimestamp=9999;
    sttHitTimeStamp = sttHit->GetTimeStamp(); // Timestamp for time clustering
    sttNeighborTimeStamp = -9999;             // Timestamp for time clustering
    // get neighbors
    TArrayI neighbors = fSttGeoH->GetNeighborings(tubeId);
    //TArrayI neighbors = fGeometryMap->GetNeighboringsByMap(tubeId);

    for (int j = 0; j < neighbors.GetSize(); ++j) {

      // set contains neighbor?
      if (hitIds.find(neighbors[j]) != hitIds.end()) {

        sttHit = (PndSttHit *)fHits[fMapTubeIdToHit[neighbors.At(j)]]; //  Get neighboring STT hit
        sttNeighborTimeStamp = sttHit->GetTimeStamp();

        if (std::abs(sttHitTimeStamp - sttNeighborTimeStamp) < fClusterTime) { // check difference in timestam [ns]

          fHitNeighbors[tubeId].push_back(neighbors.At(j)); // add neighbor separately to set, not entire set of neighbors as obtained by SttGeometryMap
        }                                                   // Difference timestamp end

        // actual tube and neighbor are not both on the edge of the stt?
        if (!(fSttGeoH->IsEdgeStraw(tubeId) && fSttGeoH->IsEdgeStraw(neighbors[j]))) {
        //if (!(fStrawMap->IsEdgeStraw(tubeId) && fStrawMap->IsEdgeStraw(neighbors[j]))) {
          if (std::abs(sttHitTimeStamp - sttNeighborTimeStamp) < fClusterTime) { // check difference in timestam [ns]
            // add to the other map
            fHitNeighborsWithoutEdges[tubeId].push_back(neighbors.At(j));
          } // Difference timestamp end
        }
      }
    }

    if (fHitNeighbors.find(tubeId) == fHitNeighbors.end()) {
      // no hitNeighbor was found
      vector<int> tmp;
      fHitNeighbors[tubeId] = tmp;
    }

    if (fHitNeighborsWithoutEdges.find(tubeId) == fHitNeighborsWithoutEdges.end()) {
      // no hitNeighbor was found
      vector<int> tmp;
      fHitNeighborsWithoutEdges[tubeId] = tmp;
    }

    if (!fSttGeoH->IsSkewedStraw(tubeId)) {
    //if (!fStrawMap->IsSkewedStraw(tubeId)) {
      for (int j = 0; j < neighbors.GetSize(); ++j) {
        if (hitIds.find(neighbors[j]) != hitIds.end() && !fSttGeoH->IsSkewedStraw(neighbors[j])) {
        //if (hitIds.find(neighbors[j]) != hitIds.end() && !fStrawMap->IsSkewedStraw(neighbors[j])) {
          // get neighbors to check timestamps

          sttHit = (PndSttHit *)fHits[fMapTubeIdToHit[neighbors.At(j)]]; // Get neighboring STT hit
          sttNeighborTimeStamp = sttHit->GetTimeStamp();

          if (std::abs(sttHitTimeStamp - sttNeighborTimeStamp) < fClusterTime) { // check difference in timestam [ns]

            fHitNeighborsWithoutSkewed[tubeId].push_back(neighbors.At(j));
          } // Difference timestamp end
        }
      }
    }
  }
}

void PndSttCellTrackFinderData::SeparateNeighbors()
{

  /* Separate the hits/active cells concerning the number of neighbors that made a signal, too.*/

  map<int, vector<int>>::iterator it;

  for (int i = 0; i < 8; ++i) {
    fSeparations[i].clear();
    fSeparationsWithoutEdges[i].clear();
  }

  for (it = fHitNeighbors.begin(); it != fHitNeighbors.end(); ++it) {

    if (it->second.size() > 6) {
      // store cells with more than 6 neighbors in the same entry
      fSeparations[7].push_back(it->first);
    } else {
      // separation concerning 0-6 hit-neighbors
      fSeparations[it->second.size()].push_back(it->first);
    }
  }

  for (it = fHitNeighborsWithoutEdges.begin(); it != fHitNeighborsWithoutEdges.end(); ++it) {

    if (it->second.size() > 6) {
      // store cells with more than 6 neighbors in the same entry
      fSeparationsWithoutEdges[7].push_back(it->first);
    } else {
      // separation concerning 0-6 hit-neighbors
      fSeparationsWithoutEdges[it->second.size()].push_back(it->first);
    }
  }

  for (it = fHitNeighborsWithoutSkewed.begin(); it != fHitNeighborsWithoutSkewed.end(); ++it) {

    if (it->second.size() > 6) {
      // store cells with more than 6 neighbors in the same entry
      fSeparationsWithoutSkewed[7].push_back(it->first);
    } else {
      // separation concerning 0-6 hit-neighbors
      fSeparationsWithoutSkewed[it->second.size()].push_back(it->first);
    }
  }
}

void PndSttCellTrackFinderData::PrintInfo()
{
  cout << "PndSttCellTrackFinderData::PrintInfo()" << endl;
  cout << "#hits: " << fHits.size() << ", #unambiguous: " << fSeparations[0].size() + fSeparations[1].size() + fSeparations[2].size()
       << ", #ambiguous: " << fSeparations[3].size() + fSeparations[4].size() + fSeparations[5].size() + fSeparations[6].size() + fSeparations[7].size() << endl;

  cout << "fHits (*-skewed): ";
  int tubeID;
  for (size_t i = 0; i < fHits.size(); ++i) {
    tubeID = ((PndSttHit *)fHits.at(i))->GetTubeID();
    cout << tubeID;
    if (fSttGeoH->IsSkewedStraw(tubeID))
    //if (fStrawMap->IsSkewedStraw(tubeID))
      cout << "*";
    cout << ", ";
  }
  cout << endl;

  cout << "fSeparations: " << endl;
  for (int i = 0; i < 8; ++i) {
    cout << "#" << i << ": ";
    for (size_t j = 0; j < fSeparations[i].size(); ++j) {
      cout << fSeparations[i].at(j);
      if (fSttGeoH->IsSkewedStraw(tubeID))
      //if (fStrawMap->IsSkewedStraw(tubeID))
        cout << "*";
      cout << ", ";
    }
    cout << endl;
  }

  cout << "fHitNeighbors: " << endl;
  for (map<int, vector<int>>::iterator it = fHitNeighbors.begin(); it != fHitNeighbors.end(); ++it) {
    cout << it->first << ": ";
    for (size_t i = 0; i < it->second.size(); ++i) {
      cout << it->second.at(i) << " ";
    }
    cout << endl;
  }
}
