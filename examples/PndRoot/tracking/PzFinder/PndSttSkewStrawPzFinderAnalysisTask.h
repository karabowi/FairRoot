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
 * PndSttSkewStrawPzFinderAnalysisTask.h
 *
 *  Created on: Apr 6, 2016
 *      Author: walan603
 */

#ifndef PNDSTTSKEWSTRAWPZFINDERANALYSISTASK_H_
#define PNDSTTSKEWSTRAWPZFINDERANALYSISTASK_H_

#include <FairTask.h>
#include <FairMCPoint.h>
#include <PndSttSkewStrawPzFinder.h>

class TClonesArray;
class TCanvas;
class PndGeoSttPar;
class PndSttHit;
class PndTrack;
class PndTrackCand;

// class PndMCMatch;
// class PndMCResult;

using namespace std;

class PndSttSkewStrawPzFinderAnalysisTask : public FairTask {
 public:
  ClassDef(PndSttSkewStrawPzFinderAnalysisTask, 1);

  PndSttSkewStrawPzFinderAnalysisTask();

  virtual ~PndSttSkewStrawPzFinderAnalysisTask();
  void setDraw(bool input) { fDraw = input; }

  void AddHitsBranchName(TString name) { fBranchNames.push_back(name); }
  void setIdealTrackBranch(TString input) { IdealTrackBranchName = input; }
  void setIdealTrackCandBranch(TString input) { IdealTrackCandBranchName = input; }
  void setTrackBranch(TString input) { TrackBranchName = input; }
  void setTrackCandBranch(TString input) { TrackCandBranchName = input; }
  void setPzDataBranch(TString input) { PzDataBranchName = input; }

  virtual InitStatus Init();

  virtual void Exec(Option_t *opt);

  virtual void Finish();

 private:
  Int_t fVerbose;
  bool fDraw;
  TString IdealTrackBranchName;
  TString IdealTrackCandBranchName;
  TString TrackBranchName;
  TString TrackCandBranchName;
  TString PzDataBranchName;

  std::vector<TString> fBranchNames;
  std::map<TString, TString> fPointBranchMap;

  TClonesArray *fEventHeader;
  TCanvas *SttXYproj;
  TCanvas *SZCanvas;

  vector<PndSttHit *> fSkewedHits;
  PndGeoSttPar *fSttParameters; // for filling fTubeArray
  TClonesArray *fTubeArray;

  TClonesArray *fMCTrack;

  TClonesArray *fIdealTrackCand;
  TClonesArray *fIdealTrack;

  TClonesArray *fFinalTrackCand;
  TClonesArray *fFinalTrack;
  //	TClonesArray *fFinalRiemannTrack;

  TClonesArray *fPzData;

  // QA histograms
  TH1F *hSttSZErr;
  TH1F *hSttSZErrAbs;
  TH1F *hSttSZmct;
  TH1F *hSttSZErrTrue;
  TH1F *hSttSZErrAbsTrue;
  TH1F *hSttSZmctTrue;

  void GetCenterOfTrack(PndTrack *temp, TVector2 &center, double &radius);

  void DrawSkewTubeLines(PndSttHit *hit);

  void DrawStt(bool isskew);

  void DrawHitIsochrone(PndSttHit *hit, Color_t color);

  void DrawHitSkewedIsochrone(PndSttHit *hit, Color_t color, Double_t beta, Double_t a, Double_t b);

  void DrawTrackParams(PndTrack *track);

  void DrawCombiLines(vector<vector<TVector2>> SZPairVector);

  //	void DrawCombiLinesResult(vector<vector<TVector2>> TrueSZ);

  void DrawSZBase(PndSttSkewStrawPzFinderData *data);

  void DrawTrack(PndTrack *temp);
};

#endif /* PNDSTTSKEWSTRAWPZFINDERANALYSISTASK_H_ */
