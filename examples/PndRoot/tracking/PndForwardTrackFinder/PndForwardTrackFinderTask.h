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

#ifndef PndForwardTrackFinderTask_H
#define PndForwardTrackFinderTask_H

#include "FairTask.h"
#include "FairRuntimeDb.h"
#include "FairRunAna.h"
#include "FairHit.h"

#include "PndGeoFtsPar.h"
#include "PndFtsMapCreator.h"
#include "PndFtsCellTrackletGenerator.h"
#include "PndTrackCombiner.h"
#include "PndLine.h"
#include "PndFtsLineApproximator.h"
#include "PndModuleCombiner.h"
#include "PndWayFollower.h"
#include "PndTrackCollection.h"

#include "PndFtsPoint.h"

#include "TClonesArray.h"
#include "TVector3.h"

#include <iostream>
#include <vector>
#include <string>

class PndForwardTrackFinderTask : public FairTask {
 public:
  PndForwardTrackFinderTask() : eventNumber(0)
  {
    PndTrackCombiner::compDist = 0.8;
    PndTrackCombiner::compAngle = 7;
    PndModuleCombiner::compDist = 0.8;
    PndModuleCombiner::compAngle = 7;
    PndTrackCombiner::compQuali = 700;
    PndWayFollower::qualiT = 4000;
    PndModuleCombiner::hitAddT = 3;
  }
  virtual ~PndForwardTrackFinderTask();

  virtual InitStatus Init();
  virtual void SetParContainers();

  virtual void Exec(Option_t *opt);
  virtual void FinishEvent();
  virtual void Finish();

  void saveCombined(vector<PndLineApproximation> combined, Int_t branch, Int_t branchID, Bool_t useOrgHits);
  void saveCorrectedTracklets(map<Int_t, vector<PndFtsExpandedTrackCand>>);
  void saveTrackCollection(vector<PndTrackCollection> coll, Bool_t withOrgHits);

 private:
  Int_t eventNumber;
  // PndGeoFtsPar *fFtsParameters; 	// needed for FtsStrawMap
  // TClonesArray *fTubeArray;		// needed for FtsStrawMap
  TClonesArray *fHits;
  FairRootManager *fIoman;
  std::vector<PndTrackCand> fFirstTrackCand;
  string branchName = "FTSHitErr";

  TClonesArray *planes;    // temp
  TClonesArray *planesHit; // temp
  TClonesArray *fFirstTrackCandArray;
  TClonesArray *correctedTracklets;
  TClonesArray *correctedHits;
  TClonesArray *fLayerTracks;
  TClonesArray *fLayerTracksHits;
  TClonesArray *fGlobalTracks;
  TClonesArray *fGlobalTracksHits;
  TClonesArray *fFinalSolution;
  TClonesArray *fTrackCollection;
  TClonesArray *fTrackCollectionHits;

  TClonesArray *rootBranches[5];

  PndFtsCellTrackletGenerator *fPndFtsCellTrackletGenerator;
  PndFtsLineApproximator *fPndFtsLineApproximator;
  PndTrackCombiner *fPndTrackCombiner;
  PndModuleCombiner *fModuleCombiner;
  PndWayFollower *fWayFollower;
  // ClassDef(PndForwardTrackFinderTask,1);

  Int_t combinedTracksHitNum;
  Int_t combinedTracksTrackNum;

  map<Int_t, PndFtsHit *> fOriginalHits;

  void createStatictcs(vector<PndTrackCollection> c, vector<PndLineApproximation> a);
  PndLineApproximation getBest(PndTrackCollection &c, vector<PndLineApproximation> a);
  Int_t getMcId(vector<PndFtsHit *> hits);
  Int_t dists[20];
  Int_t angles[20];

  ClassDef(PndForwardTrackFinderTask, 1);
};

#endif
