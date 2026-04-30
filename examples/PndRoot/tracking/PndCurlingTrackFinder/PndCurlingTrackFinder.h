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
 * PndCurlingTrackFinder.h
 *
 *  Created on: 06.07.2021
 *      Author: tstockmanns
 */

#pragma once

#include "TClonesArray.h"
#include "TVector3.h"
#include <vector>
#include <iostream>

//#include "PndSttStrawMap.h"
//#include "PndSttGeometryMap.h"
#include "PndSttHit.h"

#include "PndPreselectSttHits.h"
#include "PndSttCA.h"
#include "PndApolloniusTriplet.h"
#include "PndStt2GeoHandler.h"

using namespace PndApollonius;

//class PndSttStrawMap;
//class PndSttGeometryMap;

class PndCurlingTrackFinder {
 public:
  //PndCurlingTrackFinder(TClonesArray *tubeArray);
  PndCurlingTrackFinder(PndStt2GeoHandler *geoH) : fSttGeoH(geoH) {};
  // PndCurlingTrackFinder(){  }
  virtual ~PndCurlingTrackFinder();

  void Reset();
  void FindTracks();
  std::vector<TripletSolution> GetSolutions() const { return fSolutions; }

  virtual void SetWithTubeReduction(bool reduction) { fWithTubeReduction = reduction; };
  virtual void SetCATrackFinder(PndSttCA *CAFinder) { fCATrackFinder = CAFinder; };
  virtual void SetUnassignedSttHits(std::vector<PndSttHit *> &hits) { fUnassignedSttHits = hits; };
  virtual void SetSttHits(std::vector<PndSttHit *> &hits) { fSttHits = hits; };
  virtual void SetBranchMap(std::map<TString, TClonesArray *> &map) { fBranchMap = map; };

  std::vector<TripletSolution> GetTripletTracks() const { return fTripletTracks; }
  std::vector<TripletSolution> GetContinuousTripletTracks() const { return fContinuousTripletTracks; }
  std::vector<TripletSolution> GetTripletTracksAfterAdding() const { return fTripletTracksAfterAdding; }

 protected:
  TripletValues GetTripletsCurledTracks(std::map<int, std::vector<std::vector<PndSttHit *>>> &tubeStructure);
  TripletValues GetTripletsCurledTracks2();

  // std::map<int, std::vector<std::vector<PndSttHit *>>> DeleteFromCurlingPool(std::vector<TripletSolution> &combinedSolutions, std::map<int, std::vector<std::vector<PndSttHit
  // *>>> &CombinedTubeStructure);
  int GetMidIndex(int firstIndex, int lastIndex);

 private:
  ApolloniusTripletFunctions fFunc;
  std::vector<TripletSolution> fSolutions;
  std::vector<TripletSolution> fTripletTracks;
  std::vector<TripletSolution> fContinuousTripletTracks;
  std::vector<TripletSolution> fTripletTracksAfterAdding;

  std::vector<PndSttHit *> fUnassignedSttHits;
  std::vector<PndSttHit *> fSttHits;
  int fAllHitsCounter;
  bool fIsStrongCurling;
  bool fWithTubeReduction;
  bool fWithCombiReduction = false;
  PndSttCA *fCATrackFinder = nullptr;
  //PndSttGeometryMap *fGeometryMap = nullptr; //!
  //PndSttStrawMap *fStrawMap = nullptr;       //!
  PndStt2GeoHandler *fSttGeoH = nullptr;       //!
  std::map<TString, TClonesArray *> fBranchMap;
  std::map<FairLink, int> fMapHitstoCATracklet;
  int fBeforeSkewedRowNumber = 7;
  int fAfterSkewedRowNumber = 16;
};
