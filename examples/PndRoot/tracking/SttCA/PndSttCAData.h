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
 * PndSttCAData.h
 *
 *  Created on: May 8, 2014
 *      Author: schumann
 */

#ifndef PNDSttCADATA_H_
#define PNDSttCADATA_H_

#include <vector>
#include <map>

#include "TVector3.h"
#include "PndStt2GeoHandler.h"
//#include "PndSttStrawMap.h"
//#include "PndSttGeometryMap.h"
#include "FairHit.h"

class TClonesArray;
class PndSttSkewedHit;

class PndSttCAData {

 public:
  PndSttCAData(PndStt2GeoHandler *sttGeoH);
  //PndSttCAData(TClonesArray *fTubeArray);

  virtual ~PndSttCAData()
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

  PndStt2GeoHandler *GetSttGeoH() const { return fSttGeoH; }
  //PndSttStrawMap *GetStrawMap() const { return fStrawMap; }

  //PndSttGeometryMap *GetGeometryMap() const { return fGeometryMap; }

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

 private:
  std::map<int, FairLink> fMapHitToFairLink;                 // map< index of hit in fHit, FairLink of SttHit>
  std::map<int, FairLink> fMapHitToFairLinkOrig;             // map< index of hit in fHitOrig, FairLink of SttHit>
  std::vector<FairHit *> fHits;                              // vector with selected hits of an event
  std::vector<FairHit *> fHitsOrig;                          // vector with all originally hits of an event
  std::multimap<int, PndSttSkewedHit *> fCombinedSkewedHits; //<(inner) Tube-ID of combined stt hits of skewed layers, corresponding hit>

  Bool_t fAllowDoubleHits;
  int fNumHits;
  int fNumHitsWithoutDouble;

  PndStt2GeoHandler *fSttGeoH = nullptr;       //!
  //PndSttStrawMap *fStrawMap;               // for getting more information about the tubes
  //PndSttGeometryMap *fGeometryMap;         // for initializing the neighbors of each tube
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
  void FindHitNeighbors();

  ClassDef(PndSttCAData, 1);
};

#endif /* PNDSttCADATA_H_ */
