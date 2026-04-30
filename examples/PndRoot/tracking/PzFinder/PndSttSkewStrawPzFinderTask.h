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
 * PndSttSkewStrawPzFinderTask.h
 *
 *  Created on: Feb 8, 2016
 *      Author: walan603
 */

#ifndef PNDSTTSKEWSTRAWPZFINDERTASK_H_
#define PNDSTTSKEWSTRAWPZFINDERTASK_H_

#include <FairTask.h>
#include <PndSttSkewStrawPzFinder.h>
//#include <PndSttSkewStrawPzFinderData.h>

class TClonesArray;
class TH1;
class TH2;
class TCanvas;
class PndGeoSttPar;
class FairHit;
class PndSttHit;
class PndTrack;
class PndTrackCand;

using namespace std;

class PndSttSkewStrawPzFinderTask : public FairTask {
 public:
  ClassDef(PndSttSkewStrawPzFinderTask, 1);

  PndSttSkewStrawPzFinderTask();

  virtual ~PndSttSkewStrawPzFinderTask();

  virtual InitStatus Init();

  virtual void Exec(Option_t *opt);

  virtual void FinishEvent();

  void setTrackBranch(TString input) { TrackBranchName = input; }
  void setTrackCandBranch(TString input) { TrackCandBranchName = input; }
  void setRiemannBranch(TString input) { TrackRiemannBranchName = input; }
  void setOutputPrefix(TString input) { OutputPrefix = input; }
  int getMethod() const { return fMethod; }
  void setMethod(int input) { fMethod = input; }
  void setStepTheta(double input) { fStepTheta = input; }
  void setStepR(int input) { fStepR = input; }

  void StoreData(bool set) { fStoredata = set; }
  void WithRiemann(bool set) { fWithRiemann = set; };

 private:
  bool fStoredata;
  bool fWithRiemann;
  PndSttSkewStrawPzFinder *PzFinder;
  // PndSttSkewStrawPzFinderData *PzFinderData;

  int fMethod;
  Double_t fStepTheta;
  Int_t fStepR;
  TString TrackBranchName;
  TString TrackCandBranchName;
  TString TrackRiemannBranchName;
  TString OutputPrefix;

  TClonesArray *fEventHeader;
  TCanvas *SttXYproj;
  vector<PndSttHit *> fSkewedHits;
  TClonesArray *fSTTHits;
  TClonesArray *fMVDHitsStrip;
  TClonesArray *fMVDHitsPixel;
  TClonesArray *fGEMHits;
  PndGeoSttPar *fSttParameters; // for filling fTubeArray
  TClonesArray *fTubeArray;
  map<int, vector<int>> fMapTubeIDToHits;
  map<int, int> fMapHitIndexToTubeID;

  TClonesArray *fTrackCand;
  TClonesArray *fTrack;
  TClonesArray *fRiemannTrack;

  vector<PndTrackCand> fVectorPndTrackCand;
  vector<PndTrack> fVectorPndTrack;

  TClonesArray *fFinalTrackCand;
  TClonesArray *fFinalTrack;
  TClonesArray *fFinalRiemannTrack;
  TClonesArray *fFinalPzData;
};

#endif /* PNDSTTSKEWSTRAWPZFINDERTASK_H_ */
