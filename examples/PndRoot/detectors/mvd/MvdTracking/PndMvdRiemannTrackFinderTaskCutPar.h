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

#ifndef PNDMVDRIEMANNTRACKFINDERTASKCUTPAR_H_
#define PNDMVDRIEMANNTRACKFINDERTASKCUTPAR_H_

#include "FairTask.h"
#include "PndSdsHit.h"
#include "TH2F.h"
#include "TVector3.h"

#include "TString.h"

#include "TClonesArray.h"

#include "PndRiemannHit.h"

class PndMvdRiemannTrackFinderTaskCutPar : public FairTask {
 public:
  PndMvdRiemannTrackFinderTaskCutPar();
  virtual ~PndMvdRiemannTrackFinderTaskCutPar();
  PndMvdRiemannTrackFinderTaskCutPar(const PndMvdRiemannTrackFinderTaskCutPar &) = delete;
  PndMvdRiemannTrackFinderTaskCutPar &operator=(const PndMvdRiemannTrackFinderTaskCutPar &) = delete;

  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual InitStatus ReInit();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);
  virtual void FinishEvent();

  void SetMaxSZChi2(double val) { fMaxSZChi2 = val; }
  void SetMaxSZDist(double val) { fMaxSZDist = val; }
  void SetMinPointDist(double val) { fMinPointDist = val; }
  void SetMaxDist(double val) { fMaxDist = val; }

  void CalcCuts();

  // void PrintResult();  // not implemented
  void SetVerbose(Int_t verbose) { fVerbose = verbose; };

 private:
  TString fHitBranch;
  TString fHitBranch2;
  TString fMCTrackBranch;
  TString fTrackBranch;
  int fEventNr;

  double fMaxSZChi2;
  double fMaxSZDist;
  double fMinPointDist;
  double fMaxDist;

  TClonesArray *fHitArray;
  TClonesArray *fHitArray2;
  TClonesArray *fTrackCandArray;
  TClonesArray *fMCTrackArray;
  TClonesArray *fRiemannTracks;

  void Register();
  void Reset();
  void ProduceHits();

  bool CheckTooCloseHits(PndRiemannHit hit1, PndRiemannHit hit2);
  void CalcRiemannTracks();
  void CalcParHists();

  unsigned int fNCut; ///< cut max number of hits in IdealTrack

  int fNbin;                  ///< number of bins in the cut hists  during calculation of the cut parameters
  double frangeDist;          ///< max dist
  double frangeChi2;          ///< max cutChi2
  double fPtS;                ///< min Pt
  double fPtF;                ///< max Pt
  static const int fNPt = 10; ///< number of Pt bins
  double fThetaS;             ///< min Theta
  double fThetaF;             ///< max Theta
  static const int fNTh = 10; ///< number of Theta bins

  TH1F *fhistsDist[fNPt][fNTh];
  TH1F *fhistsChi2[fNPt][fNTh];
  TH2F *fCutDistH; ///< output cutDist histogram
  TH2F *fCutChi2H; ///< output cutChi2 histogram

  ClassDef(PndMvdRiemannTrackFinderTaskCutPar, 1);
};

#endif /*PndMvdRiemannTrackFinderTaskCutPar_H_*/
