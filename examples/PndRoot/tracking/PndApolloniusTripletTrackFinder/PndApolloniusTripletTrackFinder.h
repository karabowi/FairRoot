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
 * PndApolloniusTripletTrackFinder.h
 *
 *  Created on: 06.07.2021
 *      Author: tstockmanns
 */

#pragma once

#include "TClonesArray.h"
#include "TVector3.h"
#include <vector>
#include <iostream>
#include <array>

//#include "PndSttStrawMap.h"
//#include "PndSttGeometryMap.h"
#include "PndSttHit.h"
#include "PndStt2GeoHandler.h"

#include "PndPreselectSttHits.h"
#include "PndSttCA.h"
#include "PndApolloniusTriplet.h"

using namespace PndApollonius;


class PndApolloniusTripletTrackFinder {
 public:
  //PndApolloniusTripletTrackFinder(TClonesArray *tubeArray);
  PndApolloniusTripletTrackFinder(PndStt2GeoHandler *geoH) : fSttGeoH(geoH) {};
  // PndApolloniusTripletTrackFinder(){  }
  virtual ~PndApolloniusTripletTrackFinder();

  void Reset();
  void FindTracks();
  std::vector<TripletSolution> GetSolutions() const { return fFinalSolutions; }
  std::vector<TripletSolution> GetPreselectedTracks() const { return fPreselectedTracks; }
  std::vector<TripletSolution> GetTripletsFirst() const { return fTripletsFirst; }
  std::vector<TripletSolution> GetTripletsMid() const { return fTripletsMid; }
  std::vector<TripletSolution> GetTripletsLast() const { return fTripletsLast; }
  std::vector<TripletSolution> GetTripletsCombi() const { return fTripletsCombi; }
  std::vector<TripletSolution> GetTripletTracks() const { return fTripletTracks; }
  std::vector<TripletSolution> GetContinuousTripletTracks() const { return fContinuousTripletTracks; }
  std::vector<TripletSolution> GetTripletTracksAfterAdding() const { return fTripletTracksAfterAdding; }
  std::vector<TripletSolution> GetCombinedSolutions() const { return fSolutions; }

  virtual void SetPreselector(PndPreselectSttHits *val) { fPreselector = val; }
  virtual void SetWithTubeReduction(bool reduction) { fWithTubeReduction = reduction; };
  virtual void SetCATrackFinder(PndSttCA *CAFinder) { fCATrackFinder = CAFinder; };
  virtual void SetBranchMap(std::map<TString, TClonesArray *> &map) { fBranchMap = map; };
  virtual void SetWithCombiReduction(bool red) { fWithCombiReduction = red; };
  virtual void SetSTTName(TString name) { fSTT = name; };
  virtual void SetSTTBranchOriginal(TClonesArray *array) { fSttBranchOriginal = array; };

 protected:
  std::vector<std::vector<PndSttHit *>> PreselectSttHits();
  TripletValues GenerateTriplets(std::vector<PndSttHit *> hits);

  std::vector<int> CheckForCurlingTracks(std::map<int, std::vector<std::vector<PndSttHit *>>> &tubeStructure);

  TripletValues GetTripletsStraightTracks(std::map<int, std::vector<std::vector<PndSttHit *>>> &tubeStructure);
  TripletValues GetTripletsCurledTracks(std::map<int, std::vector<std::vector<PndSttHit *>>> &tubeStructure);
  std::map<int, std::vector<std::vector<PndSttHit *>>> GenerateCurlingGroups();

  int GetMidIndex(int firstRow, int lastRow);

  TripletValues FindTubesForStraightInCurled(std::map<int, std::vector<std::vector<PndSttHit *>>> &CombinedTubeStructure);
  TripletValues FindTubesForTracksInFirstRow(std::map<int, std::vector<std::vector<PndSttHit *>>> &TubeStructure);
  void DeleteFromCurlingPool(std::vector<TripletSolution> &combinedSolutions, std::map<int, std::vector<std::vector<PndSttHit *>>> &CombinedTubeStructure);

 private:
  std::vector<TripletSolution> fSolutions;
  std::vector<TripletSolution> fFinalSolutions;
  std::vector<TripletSolution> fPreselectedTracks;
  std::vector<TripletSolution> fTripletsFirst;
  std::vector<TripletSolution> fTripletsMid;
  std::vector<TripletSolution> fTripletsLast;
  std::vector<TripletSolution> fTripletsCombi;
  std::vector<TripletSolution> fTripletTracks;
  std::vector<TripletSolution> fContinuousTripletTracks;
  std::vector<TripletSolution> fTripletTracksAfterAdding;

  std::vector<std::map<int, std::vector<std::vector<PndSttHit *>>>> fCurlingGroups;
  std::vector<PndSttHit *> fCurlingGroup;
  PndApollonius::ApolloniusTripletFunctions fFunc;
  PndPreselectSttHits *fPreselector = nullptr; //!
  //PndSttGeometryMap *fGeometryMap = nullptr;   //!
  //PndSttStrawMap *fStrawMap = nullptr;         //!
  PndStt2GeoHandler *fSttGeoH = nullptr;       //!
  PndSttCA *fCATrackFinder = nullptr;          //!
  int fNExpectedTracks = -1;
  bool fWithTubeReduction = true;
  bool fWithCombiReduction = true;
  bool fWithCurlingTracks = true;

  bool fIsCurling = false;
  bool fIsStrongCurling = false;
  std::map<TString, TClonesArray *> fBranchMap;

  std::map<FairLink, int> fMapHitstoCATracklet;

  double fMinDistance = 0.;
  int fAllHitsCounter = 0;
  int fFirstRowNumber = 0;
  int fLastRowNumber = 19;
  int fBeforeSkewedRowNumber = 7;
  int fAfterSkewedRowNumber = 16;
  TString fSTT;
  TClonesArray *fSttBranchOriginal = nullptr;
};
