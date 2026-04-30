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
 * PndAnalysisForwardTask.h
 *
 *  Created on: Jul 15, 2016
 *      Author: kibellus
 */

#ifndef PNDTOOLS_PNDFORWARDTRACKFINDER_PNDANALYSISFORWARDTASK_H_
#define PNDTOOLS_PNDFORWARDTRACKFINDER_PNDANALYSISFORWARDTASK_H_

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
#include "PndFtsPoint.h"

#include "TClonesArray.h"
#include "TVector3.h"

#include <iostream>
#include <vector>
#include <fstream>

class PndAnalysisForwardTask : public FairTask {
 public:
  PndAnalysisForwardTask() : eventNum(0)
  {
    for (int i = 0; i < 10; i++) {
      hitArray[i] = 0;
      momentumArray[i] = 0;
      trackCountArray[i] = 0;
      hitArray2[i] = 0;
      momentumArray2[i] = 0;
      trackCountArray2[i] = 0;
    }
  }
  virtual ~PndAnalysisForwardTask();

  virtual InitStatus Init();

  virtual void Exec(Option_t *opt);
  virtual void FinishEvent();
  virtual void Finish();

  map<Int_t, vector<PndFtsHit *>> getMcTracks();
  map<Int_t, vector<PndFtsHit *> *> getReconstructedTracks();
  void analyzeMCTracks(vector<PndFtsHit *> hits, map<Int_t, vector<PndFtsHit *> *> tracks, Int_t trackID, Int_t trackCount);
  vector<Int_t> analyzeMCTrack(vector<PndFtsHit *> mcHits, vector<PndFtsHit *> *reconstructedHits);
  void anaSpecialCases(map<Int_t, vector<PndFtsHit *>> mcTracks);

 private:
  FairRootManager *fIoman;
  TClonesArray *fHits;
  TClonesArray *fSolution;
  TClonesArray *fMCTracks;
  TClonesArray *fIdealTrack;
  Int_t eventNum;

  Int_t completeClean = 0;
  Int_t completeUnclean = 0;
  Int_t incompleteClean = 0;
  Int_t incompleteUnclean = 0;
  Int_t notEnoughFound = 0;
  Int_t notFound = 0;

  map<Int_t, Int_t> mcToHitCount;
  map<Int_t, Double_t> momentum;

  Int_t hitArray[10];
  Double_t momentumArray[10];
  Int_t trackCountArray[10];

  Int_t hitArray2[10];
  Double_t momentumArray2[10];
  Int_t trackCountArray2[10];

  Int_t oneHitPerLayer = 0;
  Int_t moreThan3Hits = 0;
  Int_t twoTracksOneTube = 0;

  ClassDef(PndAnalysisForwardTask, 1);
};

#endif /* PNDTOOLS_PNDFORWARDTRACKFINDER_PNDANALYSISFORWARDTASK_H_ */
