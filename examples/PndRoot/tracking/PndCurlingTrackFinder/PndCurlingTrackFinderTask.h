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
//  PndCurlingTrackFinderTask
//  Finds Track
/////////////////////////////////////////////////////////////////

/** PndCurlingTrackFinderTask
 *@author Anna Alicke <a.alicke@fz-juelich.de>
 *@since 14.09.2021
 *
 *@version 1.0
 **
 ** PANDA task class for finding tracks based on the triplet finder and the apollonius calculation
 ** Task level RECO
 **/

#pragma once

#include "FairTask.h"
#include <vector>

#include "PndCurlingTrackFinder.h"
#include "PndGeoSttPar.h"
//#include "PndSttStrawMap.h"
//#include "PndSttGeometryMap.h"


#include "PndPreselectSttHits.h"
#include "PndSttCA.h"

struct TrackSolution;
class PndStt2GeoHandler;

class PndCurlingTrackFinderTask : public FairTask {
 public:
  /** Constructor **/
  PndCurlingTrackFinderTask();

  /** Destructor **/
  virtual ~PndCurlingTrackFinderTask();

  /** @brief Initializes and loads the data for the PndCurlingTrackFinderTask */
  virtual InitStatus Init();

  virtual void AddBranchName(TString name) { fBranchMap[name] = nullptr; } ///< Search for tracks only in given branches. If no BranchName is given all tracking detectors are taken
  virtual void AddBranchNameUnassigned(TString name)
  {
    fBranchMapUnassigned[name] = nullptr;
  } ///< Search for tracks only in given branches. If no BranchName is given all tracking detectors are taken
  virtual void AddFoundTrackName(TString name) { fBranchNameFoundTrack = name; }
  virtual void AddFoundTrackCandName(TString name) { fBranchNameFoundTrackCand = name; }
  void SetParContainers();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  virtual void Finish();

 protected:
  void FillPndTrack(std::vector<TripletSolution> &solutions);
  void FillPndTrackDebug(std::vector<TripletSolution> &solutions, TClonesArray *tracks, TClonesArray *trackCands);
  PndTrack GetPndTrack(double B, std::vector<FairHit *> &hits, TVector3 &circle, PndTrackCand &cand);

 private:
  FairRootManager *ioman = nullptr;

  PndGeoSttPar *fSttParameters = nullptr; // needed for SttStrawMap
  //TClonesArray *fTubeArray = nullptr;     // needed for SttStrawMap

  //  PndSttStrawMap fStrawMap; // for getting more information about the tubes
  //PndSttGeometryMap *fGeometryMap = nullptr;
  PndStt2GeoHandler *fSttGeoH = nullptr;

  PndCurlingTrackFinder *fFinder = nullptr;

  std::vector<TString> fBranchNames;
  std::map<TString, TClonesArray *> fBranchMap;
  std::vector<TString> fBranchNamesUnassigned;
  std::map<TString, TClonesArray *> fBranchMapUnassigned;
  TString fBranchNameFoundTrack;
  TString fBranchNameFoundTrackCand;
  TClonesArray *fBranchFoundTrack;
  TClonesArray *fBranchFoundTrackCand;
  TClonesArray *fTrackCandArray = nullptr;
  TClonesArray *fTrackArray = nullptr;
  TClonesArray *fTripletTracksCands = nullptr;
  TClonesArray *fTripletTracks = nullptr;
  TClonesArray *fContinuousTripletTracksCands = nullptr;
  TClonesArray *fContinuousTripletTracks = nullptr;
  TClonesArray *fTripletTracksCandsAfterAdding = nullptr;
  TClonesArray *fTripletTracksAfterAdding = nullptr;
  TClonesArray *fFinalTrackCandArray = nullptr;
  TClonesArray *fFinalTrackArray = nullptr;
  PndSttCA *fCATrackFinder = nullptr;

  bool fWithTubeReduction;
  bool fWithCombiReduction;
  int fVerbose;

  ClassDef(PndCurlingTrackFinderTask, 1);
};
