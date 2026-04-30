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
 * PndSttCellTrackFinderData.h
 *
 *  Created on: May 8, 2014
 *      Author: schumann
 */

#ifndef PNDSTTCELLTRACKFINDERDATA_H_
#define PNDSTTCELLTRACKFINDERDATA_H_

#include <vector>
#include <map>

#include "TVector3.h"
//#include "PndSttGeometryMap.h"
//#include "PndSttStrawMap.h"
#include "FairHit.h"

class TClonesArray;
class PndSttSkewedHit;
class PndStt2GeoHandler;

class PndSttCellTrackFinderData {

 public:
  PndSttCellTrackFinderData(PndStt2GeoHandler *geoH);
  //PndSttCellTrackFinderData(TClonesArray *fTubeArray);

  virtual ~PndSttCellTrackFinderData()
  {
    //delete fStrawMap;
    //delete fGeometryMap;
    for (size_t i = 0; i < fHits.size(); ++i)
      delete fHits.at(i);
    for (size_t i = 0; i < fHitsOrig.size(); ++i)
      delete fHitsOrig.at(i);
  }
  // only support STTHits , uses brachNAme to distinguish between normal and skewed straws

  void AddHits(TClonesArray *hits, TString branchName);

  void GenerateNeighborhoodData();

  void clear()
  {
    fHits.clear();
    fHitsOrig.clear();
    fMapHitToFairLink.clear();
    fMapTubeIdToHit.clear();
    fHitNeighbors.clear();
    fSeparations.clear();
    fCombinedSkewedHits.clear();
    fHitNeighborsWithoutEdges.clear();
    fSeparationsWithoutEdges.clear();
    fHitNeighborsWithoutSkewed.clear();
    fSeparationsWithoutSkewed.clear();
  }

  void PrintInfo();

  void SetAllowDoubleHits(Bool_t value) { fAllowDoubleHits = value; }

  Bool_t GetAllowDoubleHits() { return fAllowDoubleHits; }

  std::vector<FairHit *> GetHits() const { return fHits; }

  std::multimap<int, PndSttSkewedHit *> GetCombinedSkewedHits() const { return fCombinedSkewedHits; }

  //PndSttStrawMap *GetStrawMap() const { return fStrawMap; }

  //PndSttGeometryMap *GetGeometryMap() const { return fGeometryMap; }
  
  PndStt2GeoHandler *GetSttGeoH() const { return fSttGeoH; }

  std::map<int, FairLink> GetMapHitToFairLink() const { return fMapHitToFairLink; }

  std::map<int, int> GetMapTubeIdToHit() const { return fMapTubeIdToHit; }

  std::map<int, TVector3> GetMapTubeIdToPos() const { return fMapTubeIdToPos; }

  std::map<int, std::vector<int>> GetHitNeighbors() const { return fHitNeighbors; }

  std::map<int, std::vector<int>> GetSeparations() const { return fSeparations; }

  std::map<int, std::vector<int>> GetHitNeighborsWithoutEdges() const { return fHitNeighborsWithoutEdges; }

  std::map<int, std::vector<int>> GetSeparationsWithoutEdges() const { return fSeparationsWithoutEdges; }

  std::map<int, std::vector<int>> GetHitNeighborsWithoutSkewed() const { return fHitNeighborsWithoutSkewed; }

  std::map<int, std::vector<int>> GetSeparationsWithoutSkewed() const { return fSeparationsWithoutSkewed; }

  int GetNumHits() { return fNumHits; }

  int GetNumHitsWithoutDouble() { return fNumHitsWithoutDouble; }
  void SetRunTimeBased(Bool_t val) { fRunTimeBased = val; };
  void SetClusterTime(double val) { fClusterTime = val; };

 private:
  std::map<int, FairLink> fMapHitToFairLink;                 // map< index of hit in fHit, FairLink of SttHit>
  std::map<int, FairLink> fMapHitToFairLinkOrig;             // map< index of hit in fHitOrig, FairLink of SttHit>
  std::vector<FairHit *> fHits;                              // vector with selected hits of an event
  std::vector<FairHit *> fHitsOrig;                          // vector with all originally hits of an event
  std::multimap<int, PndSttSkewedHit *> fCombinedSkewedHits; //<(inner) Tube-ID of combined stt hits of skewed layers, corresponding hit>

  double fClusterTime; // J.R. 17/04-2018
  Bool_t fAllowDoubleHits;
  int fNumHits;
  int fNumHitsWithoutDouble;
  Bool_t fRunTimeBased;

  //PndSttStrawMap *fStrawMap;               // for getting more information about the tubes
  //PndSttGeometryMap *fGeometryMap;         // for initializing the neighbors of each tube
  PndStt2GeoHandler *fSttGeoH;
  std::map<int, TVector3> fMapTubeIdToPos; // map<straw id, position of the center of the tube>
  std::map<int, int> fMapTubeIdToHit;      // map< id of straw tube, index of hit in fHit>

  std::map<int, std::vector<int>> fHitNeighbors; // map<straw id, vector<ids of hit-neighbors>>
  std::map<int, std::vector<int>> fSeparations;  // map<#active neighbors, vector<straw ids>>

  std::map<int, std::vector<int>> fHitNeighborsWithoutEdges; // map<straw id, vector<ids of hit-neighbors>>
  std::map<int, std::vector<int>> fSeparationsWithoutEdges;  // map<#active neighbors, vector<straw ids>>

  std::map<int, std::vector<int>> fHitNeighborsWithoutSkewed; // map<straw id, vector<ids of hit-neighbors>>
  std::map<int, std::vector<int>> fSeparationsWithoutSkewed;  // map<#active neighbors, vector<straw ids>>

  /* Method grades the active cells according to the number of hit-neighbors.*/
  void SeparateNeighbors();

  /* Method searches for hit-neighbors of each cell.*/

  void FindHitNeighborsEventBased();

  void FindHitNeighborsTimeBased();

  ClassDef(PndSttCellTrackFinderData, 1);
};

#endif /* PNDSTTCELLTRACKFINDERDATA_H_ */
