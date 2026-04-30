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

// -------------------------------------------------------------------------
// -----                      PndStraightLineTrackFinderTask                    -----
// -----                  Created 22/10/09  by M. Michel               -----
// -------------------------------------------------------------------------

#ifndef PndStraightLineTrackFinderTask_H
#define PndStraightLineTrackFinderTask_H

#include "FairTask.h"
#include "PndMCTrack.h"
#include "PndSdsGeoPar.h"
#include "PndSdsHit.h"
#include "PndSdsMCPoint.h"
#include "PndSdsClusterStrip.h"
#include "PndTrackCand.h"

#include <string>
#include <vector>
#include <utility>

class TClonesArray;

class PndStraightLineTrackFinderTask : public FairTask {
 public:
  /** Default constructor **/
  PndStraightLineTrackFinderTask(Int_t inFinderMode = 0, TString hitBranch = "LMDHitsStrip", TString clusterBranch = "LMDStripClusterCand", TString digiBranch = "LMDStripDigis",
                                 Int_t innSensPP = 8);
  /** Destructor **/
  virtual ~PndStraightLineTrackFinderTask();

  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual InitStatus ReInit();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  void SetVerbose(Int_t verbose) { fVerbose = verbose; };
  void SetInaccuracy(Double_t accu) { dXY = accu; };
  void SetSensStripFlag(bool fS) { flagStipSens = fS; };
  void SetSensPixelFlag(bool fS) { flagPixelSens = fS; };

 private:
  bool flagStipSens;
  bool flagPixelSens;
  Double_t dXY;
  Int_t fFinderMode;
  Int_t nSensPP;
  //    std::vector<Int_t> GetHitPerCluster(PndSdsClusterStrip* clusterCand);
  Double_t GetTrackDip(PndMCTrack *myTrack);
  Double_t GetTrackCurvature(PndMCTrack *myTrack);

  TString fHitBranchStrip;
  TString fClusterBranchStrip;
  TString fDigiBranchStrip;

  /** Input array of PndSdsDigis **/
  TClonesArray *fStripHitArray;
  TClonesArray *fStripClusterArray;
  TClonesArray *fStripDigiArray;

  /** Output array of PndSdsHits **/
  TClonesArray *fTrackCandArray;

  void Register();
  void Reset();
  void ProduceHits();

  bool SortHitsByZ(std::vector<std::vector<std::pair<Int_t, bool>>> &hitsd, Int_t nStripHits);
  bool SortHitsByDet(std::vector<std::vector<std::pair<Int_t, bool>>> &hitsd, Int_t nStripHits);
  bool SortHitsByDet2(std::vector<std::vector<std::pair<Int_t, bool>>> &hitsd, Int_t nStripHits); // uses PndLmdDim
  void FindHitsI(std::vector<PndTrackCand> &tofill, std::vector<std::vector<std::pair<Int_t, bool>>> &hitsd, Int_t nStripHits);
  void FindHitsII(std::vector<PndTrackCand> &tofill, std::vector<std::vector<std::pair<Int_t, bool>>> &hitsd, Int_t nStripHits);
  void FindHitsIII(std::vector<PndTrackCand> &tofill, std::vector<std::vector<std::pair<Int_t, bool>>> &hitsd, Int_t nStripHits);

  ClassDef(PndStraightLineTrackFinderTask, 2);
};

#endif /* PndStraightLineTrackFinderTask_H */
