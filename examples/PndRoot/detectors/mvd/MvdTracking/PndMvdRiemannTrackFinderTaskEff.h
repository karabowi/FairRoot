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

#ifndef PNDMVDRIEMANNTRACKFINDERASKEFF_H_
#define PNDMVDRIEMANNTRACKFINDERASKEFF_H_

#include "FairTask.h"
#include "PndMCTrack.h"
#include "PndTrackCand.h"

#include "TString.h"
#include "TClonesArray.h"

#include <vector>
#include <TH2F.h>

class PndMvdRiemannTrackFinderTaskEff : public FairTask {
 public:
  PndMvdRiemannTrackFinderTaskEff();
  virtual ~PndMvdRiemannTrackFinderTaskEff();
  PndMvdRiemannTrackFinderTaskEff(const PndMvdRiemannTrackFinderTaskEff &) = delete;
  PndMvdRiemannTrackFinderTaskEff &operator=(const PndMvdRiemannTrackFinderTaskEff &) = delete;

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

  // void PrintResult();  // not implemented
  void SetVerbose(Int_t verbose) { fVerbose = verbose; };

  TH2F *eff0H;
  TH2F *effH;
  TH2F *GhH;

 private:
  TString fHitBranch;
  TString fHitBranch2;
  TString fMCTrackBranch;

  TString fIdealTrackBranch;
  TString fFTrackBranch;

  int fEventNr;

  double fMaxSZChi2;
  double fMaxSZDist;
  double fMinPointDist;
  double fMaxDist;

  TClonesArray *fHitArray;
  TClonesArray *fHitArray2;
  TClonesArray *fTrackCandArray;
  TClonesArray *fIdealTrackCandArray;
  TClonesArray *fMCTracksArray;

  void Register();
  void Reset();
  void ProduceHits();

  void ComparingFandR(std::vector<PndTrackCand *> RecoT);
  bool CheckRecoTrack(PndTrackCand *cand, PndMCTrack *myTrack);

  void AddGhostTrack(int trackF);

  ClassDef(PndMvdRiemannTrackFinderTaskEff, 1)
};

#endif /*PNDMVDRIEMANNTRACKFINDERASKEFF_H_*/
