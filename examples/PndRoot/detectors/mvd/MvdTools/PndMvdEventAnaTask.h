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
// -----                 PndMvdEventAnaTask header file             -----
// -----                  Created 20/03/07  by R.Kliemt               -----
// -------------------------------------------------------------------------

/** PndMvdEventAnaTask.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 **
 ** Displays all available informations for a given event
 **/

#ifndef PNDMVDEVENTANATASK_H
#define PNDMVDEVENTANATASK_H

// framework includes
#include "FairTask.h"
#include "FairHit.h"
#include "TH1.h"
#include "GFTrackCand.h"

#include <vector>
#include <map>

class TClonesArray;
class PndSdsCluster;

class PndMvdEventAnaTask : public FairTask {
 public:
  /** Default constructor **/
  PndMvdEventAnaTask();

  /** Destructor **/
  virtual ~PndMvdEventAnaTask();

  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  virtual void Finish();

  void DrawTracksPerEvent(TString opt = "") { fHTracksPerEvent->DrawClone(opt); }
  void DrawHitsPerTrack(TString opt = "") { fHHitsPerTrack->DrawClone(opt); }
  void DrawEnergyPerHit(TString opt = "") { fHEnergyPerHit->DrawClone(opt); }

  void DrawPointRes(TString opt = "") { fHPointRes->DrawClone(opt); }
  void DrawPointResS(TString opt = "") { fHPointResS->DrawClone(opt); }
  void DrawPointResD(TString opt = "") { fHPointResD->DrawClone(opt); }
  void DrawPointResM(TString opt = "") { fHPointResM->DrawClone(opt); }
  void DrawEnergyRes(TString opt = "") { fHEnergyRes->DrawClone(opt); }
  void DrawDigisPerCluster(TString opt = "") { fHDigisPerCluster->DrawClone(opt); }

  void DrawPointResStrip(TString opt = "") { fHPointResStrip->DrawClone(opt); }
  void DrawEnergyResStrip(TString opt = "") { fHEnergyResStrip->DrawClone(opt); }
  void DrawDigisPerClusterStrip(TString opt = "") { fHDigisPerClusterStrip->DrawClone(opt); }

  void DrawPtRes(TString opt = "") { fHPtRes->DrawClone(opt); }
  void DrawPRes(TString opt = "") { fHPRes->DrawClone(opt); }

  void DrawRiemannRes(TString opt = "") { fHRiemannRes->DrawClone(opt); }
  void DrawRiemannFakes(TString opt = "") { fHRiemannFakes->DrawClone(opt); }
  void DrawRiemannTracksPerTrack(TString opt = "") { fHRiemannTracksPerTrack->DrawClone(opt); }
  void DrawRiemannTracksPerTrackAdd(TString opt = "") { fHRiemannTracksPerTrackAdd->DrawClone(opt); }

  void DrawRiemannVertexResolutionX(TString opt = "") { fHRiemannVertexResolutionX->DrawClone(opt); }
  void DrawRiemannVertexResolutionY(TString opt = "") { fHRiemannVertexResolutionY->DrawClone(opt); }
  void DrawRiemannVertexResolutionZ(TString opt = "") { fHRiemannVertexResolutionZ->DrawClone(opt); }

 private:
  TClonesArray *fMCTracks;
  TClonesArray *fMCHits;
  TClonesArray *fPixDigis;
  TClonesArray *fStripDigis;
  TClonesArray *fPixReco;
  TClonesArray *fStripReco;
  TClonesArray *fPixCluster;
  TClonesArray *fStripCluster;
  TClonesArray *fTrackCand;

  std::vector<int> fGhostCand;

  TH1 *fHTracksPerEvent;
  TH1 *fHHitsPerTrack;
  TH1 *fHEnergyPerHit;

  TH1 *fHPointRes;
  TH1 *fHPointResS;
  TH1 *fHPointResD;
  TH1 *fHPointResM;
  TH1 *fHDigisPerCluster;
  TH1 *fHEnergyRes;
  TH1 *fHPRes;
  TH1 *fHPtRes;

  TH1 *fHPointResStrip;
  TH1 *fHPointResSStrip;
  TH1 *fHPointResDStrip;
  TH1 *fHPointResMStrip;
  TH1 *fHDigisPerClusterStrip;
  TH1 *fHEnergyResStrip;

  TH1 *fHRiemannRes;
  TH1 *fHRiemannFakes;
  TH1 *fHRiemannTracksPerTrack;
  TH1 *fHRiemannTracksPerTrackAdd;

  TH1 *fHRiemannVertexResolutionX;
  TH1 *fHRiemannVertexResolutionY;
  TH1 *fHRiemannVertexResolutionZ;

  bool fPrintTrack;
  bool fPrintMCHit;
  bool fPrintCluster;
  bool fPrintPixDigis;
  bool fPrintPixHit;
  bool fPrintStripCluster;
  bool fPrintStripDigis;
  bool fPrintStripHit;
  bool fPrintTrackMatch;
  bool fPrintGhosts;

  int fNTracks;
  int fNPossibleTracks;
  int fNCompleteTracks;
  int fNPartTracks;
  int fNNotFoundPossibleTracks;
  int fNNotFoundTracks;
  int fNGhostTracks;

  int fEventNr;

  std::map<int, std::vector<int>> fTrackPixHitIdMap;   // Track -> PixHitId
  std::map<int, std::vector<int>> fTrackStripHitIdMap; // Track -> StripHitId

  bool MCHitBelongsToCluster(int HitIndex, PndSdsCluster *cluster, bool pixCluster);
  void GetTrackCandsForMCTrack(std::vector<int> pixHitId, std::vector<int> stripHitId, std::vector<int> &matches, std::vector<int> &result);

  int GetRecoHit(int clIndex, bool pixel) const;
  std::vector<int> GetClusters(int MCHit, bool pixel);

  void PrintClusterDigiInfo(int clIndex, std::vector<Int_t> digiInd, bool pixel);
  void PrintRecoHitInfo(int hitInd, int digiSize, TVector3 MCPos, double MCEnergy, bool pixel) const;

  void PrintTrackCand(GFTrackCand *cand) const;
  void Register();

  void Reset();

  void ProduceHits();

  FairHit *GetFairHit(Int_t detId, Int_t hitId);

  std::map<int, std::vector<int>> AssignHitsToTracks();

  ClassDef(PndMvdEventAnaTask, 1);
};

#endif
