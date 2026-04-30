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

#ifndef PNDMVDRIEMANNVERTEXFINDERTASK_H_
#define PNDMVDRIEMANNVERTEXFINDERTASK_H_

#include <iostream>
#include "FairTask.h"
//#include "PndSdsHit.h"
#include "TH1F.h"
#include "PndTrackCand.h"
#include "PndMCTrack.h"
#include "TString.h"
#include "TClonesArray.h"
#include "stdio.h"

class PndMvdRiemannVertexFinderTask : public FairTask {
 public:
  PndMvdRiemannVertexFinderTask();
  virtual ~PndMvdRiemannVertexFinderTask();
  PndMvdRiemannVertexFinderTask(const PndMvdRiemannVertexFinderTask &) = delete;
  PndMvdRiemannVertexFinderTask &operator=(const PndMvdRiemannVertexFinderTask &) = delete;

  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual InitStatus ReInit();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);
  virtual void FinishEvent();

  // void PrintResult();  // not implemented
  void SetVerbose(Int_t verbose) { fVerbose = verbose; };
  void SetVertexCut(double cut) { fVertexCut = cut; };
  TH1F *delta;
  TH1F *wrongV;

  std::pair<double, double> eff;
  std::pair<double, double> ghosts;

 private:
  TString fHitBranch;
  TString fHitBranch2;
  TString fTrackBranch;
  TString fIdealTrackCandBranch;
  TString fMCTrackBranch;

  int fEventNr;
  int fVerbose;
  double fVertexCut;

  TClonesArray *fHitArray;
  TClonesArray *fHitArray2;
  TClonesArray *fTrackCandArray;
  //	TClonesArray* fRiemannTrackArray;
  TClonesArray *fTrackArray;
  TClonesArray *fIdealTrackCandArray;
  TClonesArray *fMCTrackArray;

  TClonesArray *fVertex;
  TClonesArray *fMCVertex;

  bool CheckRecoTrack(PndTrackCand *cand, PndMCTrack *myTrack);

  bool CheckVertex(std::vector<int> Combination, std::vector<std::pair<int, int>> PairCand);
  bool CheckTwoCands(int first, int second);
  int FoundCandInMCCands(int candN);
  void refit(std::vector<int> &CheckedCand);
  void FindVertex(std::vector<int> CheckedCand, std::vector<std::pair<int, int>> &PairCand, std::vector<std::pair<int, int>> &TrueMCCand,
                  std::vector<std::pair<int, int>> &FalseMCCand, std::vector<std::pair<int, int>> &MCCand, int &MaxIndex);
  void CalcEfficiency(std::vector<std::pair<int, int>> TrueMCCand, std::vector<std::pair<int, int>> FalseMCCand, std::vector<std::pair<int, int>> MCCand);

  void Register();
  void Reset();
  void ProduceHits();

  ClassDef(PndMvdRiemannVertexFinderTask, 1);
};

#endif /*PndMvdRiemannVertexFinderTask_H_*/
