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
 * PndSttCellTrackFinderAnalysisTask.h
 *
 *  Created on: Jun 26, 2013
 *      Author: schumann
 */

#ifndef PndSttCellTrackFinderAnalysisTask_H_
#define PndSttCellTrackFinderAnalysisTask_H_

#include "FairTask.h"
#include "TH1I.h"
#include "TCanvas.h"
#include "FairMultiLinkedData.h"

#include <map>
#include <vector>
#include <fstream>

class TClonesArray;
class PndMCMatch;
class PndGeoSttPar;
class PndMCResult;

class PndSttCellTrackFinderAnalysisTask : public FairTask {
 public:
  PndSttCellTrackFinderAnalysisTask() : FairTask("Stt Cell TrackFinder Analysis Task"){};
  virtual ~PndSttCellTrackFinderAnalysisTask(){};

  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);
  virtual void FinishEvent();
  virtual void Finish();

  void SetVerbose(Int_t verbose) { fVerbose = verbose; };

 private:
  /*Method for testing functionality of Conformal Mapping (Circle Fit)*/
  void CheckConformalMap();

  /*Method verifies the result of the first step of the trackfinding-algorithm*/
  void CheckFirstTracklets();

  /*Method verifies the result of the second step of the trackfinding-algorithm*/
  void CheckTrackletCombinations();

  /*Method for drawing STTHits + Circles of Riemann Fit for the first step of trackfinding*/
  void DrawFirstRiemannPlots(int eventNumber);

  /*Method for drawing STTHits + Circles of Riemann Fit for the second step of trackfinding*/
  void DrawCombiRiemannPlots(int eventNumber);

  int GetNumLinksOfHits(FairMultiLinkedData &hitLinks, PndMCResult &sttHitsToMCTrack, PndMCResult &sttHitToPoint);

  void TestRecoQualityFirstStep();

  void TestRecoQualityCombi();

  void CountMaxHitsFirstStep();

  void CountMaxHitsCombi();

  std::map<int, bool> fFoundMC;

  PndMCMatch *fMCMatch;

  TClonesArray *fMCTrack;

  TClonesArray *fFirstTrackCand;
  TClonesArray *fFirstRiemannTrack;
  TClonesArray *fCombiTrackCand;
  TClonesArray *fCombiRiemannTrack;

  TClonesArray *fEventHeader;
  TClonesArray *fSTTHits;

  std::map<int, std::vector<int>> fMapTubeIDToHits; // map<tube-id, vector<indices of SttHits in fSTTHits>>
  std::map<int, int> fMapHitIndexToTubeID;          // map<index of SttHit in fSTTHits, tube-id>

  PndGeoSttPar *fSttParameters; // for filling fTubeArray
  TClonesArray *fTubeArray;     // contains at index tube-id corresponding PndSttTube

  // histograms for first step of trackfinding
  TH1I *fHistoNumberOfLinks1;
  TH1I *fHistoNumberOfTracklets1;
  TH1I *fHistoNumberOfAssignedHits1;
  TH1I *fHistoNumberOfHits1;
  TH1I *fHistoNumberOfErrors1;
  TH1I *fHistoMaxHitsOfMCTrack1;
  TH1I *fHistoNumberOfMissingHits1;

  TH1I *fHistoQualityFirstStep;

  // histograms for second step of trackfinding
  TH1I *fHistoNumberOfLinks2;
  TH1I *fHistoNumberOfTracklets2;
  TH1I *fHistoNumberOfHits2;
  TH1I *fHistoNumberOfErrors2;
  TH1I *fHistoMaxHitsOfMCTrack2;
  TH1I *fHistoNumberOfMissingHits2;

  TH1I *fHistoQualityCombi;
  TH1I *fHistoNumberOfHitsMCTrack;

  TCanvas *fCanvas;

  ClassDef(PndSttCellTrackFinderAnalysisTask, 1);
};

#endif /* PndSttCellTrackFinderAnalysisTask_H_ */
