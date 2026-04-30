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

#ifndef PNDMVDSTTGEMRIEMANNTRACKFINDERTASK_H_
#define PNDMVDSTTGEMRIEMANNTRACKFINDERTASK_H_

#include "FairTask.h"
//#include "PndSdsHit.h"
#include "TH2F.h"
#include "PndGeoHandling.h"
#include "PndGeoSttPar.h"

#include "FairTSBufferFunctional.h"

#include <vector>

class PndMvdSttGemRiemannTrackFinderTask : public FairTask {
 public:
  PndMvdSttGemRiemannTrackFinderTask();
  virtual ~PndMvdSttGemRiemannTrackFinderTask();
  PndMvdSttGemRiemannTrackFinderTask(const PndMvdSttGemRiemannTrackFinderTask &) = delete;
  PndMvdSttGemRiemannTrackFinderTask &operator=(const PndMvdSttGemRiemannTrackFinderTask &) = delete;

  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual InitStatus ReInit();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);
  virtual void FinishEvent();

  void AddHitBranch(TString branchName); // Has to be called before Init() is used otherwise the default names are taken!

  void SetMaxSZChi2(double val) { fMaxSZChi2 = val; }
  void SetMaxSZDist(double val) { fMaxSZDist = val; }
  void SetMinPointDist(double val) { fMinPointDist = val; }
  void SetMaxDist(double val) { fMaxDist = val; }

  void SetCutDistH(TH2F *hist) { fCutDistH = hist; }
  void SetCutChi2H(TH2F *hist) { fCutChi2H = hist; }

  void SetB(Double_t B) { fB = B; }

  // void PrintResult();  // not implemented
  void SetVerbose(Int_t verbose) { fVerbose = verbose; };
  void SetPersistence(Bool_t val) { fPersistence = val; };
  //  void SetGeoH(PndGeoHandling geoH){ fGeoH=geoH;};

  void FillHitArray();
  void InitHitArray(TString branchName);

 private:
  std::vector<TString> fHitBranch;
  TString fTrackBranch;

  int fEventNr;

  double fMaxSZChi2;
  double fMaxSZDist;
  double fMinPointDist;
  double fMaxDist;

  double fB;

  std::vector<TClonesArray *> fHitArray;
  TClonesArray *fTrackCandArray;
  TClonesArray *fRiemannTrackArray;
  TClonesArray *fTrackArray;
  TClonesArray *fCorrectedSttHitArray;

  TH2F *fCutDistH;
  TH2F *fCutChi2H;

  Bool_t fInitDone;
  Bool_t fPersistence;

  BinaryFunctor *fStopFunctor;
  BinaryFunctor *fTimeGapFunctor;

  PndGeoSttPar *fSttParameters; //  CHECK added
  TClonesArray *fTubeArray;

  // PndGeoHandling* fGeoH;

  void Register();
  void Reset();
  void ProduceHits();

  ClassDef(PndMvdSttGemRiemannTrackFinderTask, 1);
};

#endif /*PNDMVDRIEMANNTRACKFINDERTASK_H_*/
