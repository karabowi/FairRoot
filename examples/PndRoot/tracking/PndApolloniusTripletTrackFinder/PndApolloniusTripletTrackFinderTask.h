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
//  PndApolloniusTripletTrackFinderTask
//  Finds Track
/////////////////////////////////////////////////////////////////

/** PndApolloniusTripletTrackFinderTask
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

#include "PndApolloniusTripletTrackFinder.h"
//#include "PndSttStrawMap.h"
#include "PndGeoSttPar.h"

#include "PndPreselectSttHits.h"
#include "PndSttCA.h"
using namespace PndApollonius;

class PndStt2GeoHandler;
struct TrackSolution;

class PndApolloniusTripletTrackFinderTask : public FairTask {
 public:
  /** Constructor **/
  PndApolloniusTripletTrackFinderTask();

  /** Destructor **/
  virtual ~PndApolloniusTripletTrackFinderTask();

  /** @brief Initializes and loads the data for the PndApolloniusTripletTrackFinderTask */
  virtual InitStatus Init();

  virtual void AddBranchName(TString name) { fBranchMap[name] = nullptr; } ///< Search for tracks only in given branches. If no BranchName is given all tracking detectors are taken

  void SetParContainers();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  virtual void Finish();
  virtual void SetWithTubeReduction(bool reduction) { fWithTubeReduction = reduction; };
  virtual void SetWithCombiReduction(bool reduction) { fWithCombiReduction = reduction; };
  virtual void SetSTTName(TString name) { fSTT = name; };
  virtual void AddFoundTrackName(TString name) { fBranchNameFoundTrack = name; }
  void SetPreselector(PndPreselectSttHits *val) { fPreselector = val; }

 protected:
  void FillPndTrack(std::vector<TripletSolution> &solutions);
  void FillPndTrackDebug(std::vector<TripletSolution> &solutions, TClonesArray *tracks, TClonesArray *trackCands);
  bool CheckZInfo(PndTrackCand &cand);

 private:
  FairRootManager *ioman = nullptr; //!

  PndGeoSttPar *fSttParameters = nullptr; //!
  TClonesArray *fTubeArray = nullptr;     //!

  PndApolloniusTripletTrackFinder *fFinder = nullptr; //!

  PndStt2GeoHandler *fSttGeoH;
  //PndSttStrawMap *fStrawMap;
  std::vector<TString> fBranchNames;            //!
  std::map<TString, TClonesArray *> fBranchMap; //!

  TClonesArray *fTrackCandArray = nullptr; //!
  TClonesArray *fTrackArray = nullptr;     //!

  PndPreselectSttHits *fPreselector = nullptr; //!
  PndSttCA *fCATrackFinder = nullptr;          //!

  double fBz = 2.; //!

  bool fWithTubeReduction;  //!
  bool fWithCombiReduction; //!
  TString fSTT;
  TString fBranchNameFoundTrack;

  TClonesArray *fPreselectedTracksCands = nullptr;        //!
  TClonesArray *fPreselectedTracks = nullptr;             //!
  TClonesArray *fTripletsFirstCands = nullptr;            //!
  TClonesArray *fTripletsFirst = nullptr;                 //!
  TClonesArray *fTripletsMidCands = nullptr;              //!
  TClonesArray *fTripletsMid = nullptr;                   //!
  TClonesArray *fTripletsLastCands = nullptr;             //!
  TClonesArray *fTripletsLast = nullptr;                  //!
  TClonesArray *fTripletsCombiCands = nullptr;            //!
  TClonesArray *fTripletsCombi = nullptr;                 //!
  TClonesArray *fTripletTracksCands = nullptr;            //!
  TClonesArray *fTripletTracks = nullptr;                 //!
  TClonesArray *fContinuousTripletTracksCands = nullptr;  //!
  TClonesArray *fContinuousTripletTracks = nullptr;       //!
  TClonesArray *fCombinedTripletTracksCands = nullptr;    //!
  TClonesArray *fCombinedTripletTracks = nullptr;         //!
  TClonesArray *fTripletTracksCandsAfterAdding = nullptr; //!
  TClonesArray *fTripletTracksAfterAdding = nullptr;      //!
  TClonesArray *fBranchFoundTrack = nullptr;              //!
  TClonesArray *fSttBranchOriginal = nullptr;             //!

  ClassDef(PndApolloniusTripletTrackFinderTask, 1);
};
