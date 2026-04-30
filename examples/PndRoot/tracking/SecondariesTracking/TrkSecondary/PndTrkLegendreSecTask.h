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

#ifndef PNDTRKLEGENDSECRETASK_H
#define PNDTRKLEGENDSECRETASK_H

#include "PndTrkLegendreTransform.h"
// fairroot
#include "FairTask.h"
// pandaroot
#include "PndTrack.h"
#include "PndMCTrack.h"
// stt
#include "PndGeoSttPar.h"
#include "PndSttHit.h"
#include "PndSttMapCreator.h"
// tracking
#include "PndTrkSttHitList.h"
#include "PndTrkSdsHitList.h"
#include "PndTrkCluster.h"
#include "PndTrkClusterList.h"
#include "PndTrkTrack.h"
#include "PndTrkConformalHitList.h"
#include "PndTrkFitter.h"

// ROOT
#include "TH2F.h"
#include "TCanvas.h"

#define MAXNOFHITS 1000 // CHECK

class TClonesArray;
class TObjectArray;
class TSpectrum2;
class PndTrkLegendreTransform;
class PndTrkConformalTransform;
class PndTrkLegendreSecTask : public FairTask {

 public:
  /** Default constructor **/
  PndTrkLegendreSecTask();
  PndTrkLegendreSecTask(int verbose);

  /** Destructor **/
  ~PndTrkLegendreSecTask();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  void SetParContainers();

  void Initialize();
  void Reset();

  Int_t FillConformalHitList(int isec = -1);
  void FillLegendreHisto(Int_t mode);
  void FillLegendreHisto(PndTrkCluster *cluster);

  PndTrkCluster CreateSttCluster(PndTrkHit *firsthit);
  Bool_t IsSttAssociate(PndTrkHit *hit1, PndTrkHit *hit2);

  PndTrkCluster CreateClusterByConfDistance(double fitm, double fitq);
  PndTrkCluster CreateClusterByRealDistance(double xc0, double yc0, double R0);
  PndTrkCluster CreateClusterByMixedDistance(double fitm, double fitq);
  PndTrkCluster CreateClusterByDistance(Int_t mode, double fitm, double fitq);
  PndTrkCluster CreateSkewHitList(PndTrkTrack *track);

  void FromConformalToRealTrack(double fitm, double fitp, double &x0, double &y0, double &R);
  PndTrkCluster CleanUpSkewHitList(PndTrkCluster *skewhitlist);

  void RegisterTrack(PndTrkTrack *track);

  void ComputeTraAndRot(PndTrkHit *hit, Double_t &delta, Double_t trasl[2]);

  PndTrkHit *FindSttReferenceHit(int isec = -1);
  PndTrkHit *FindMvdPixelReferenceHit();
  PndTrkHit *FindMvdStripReferenceHit();
  PndTrkHit *FindMvdReferenceHit();
  PndTrkHit *FindReferenceHit();
  PndTrkCluster Cleanup(PndTrkCluster cluster);

  void SearchSecondaryTracks() { fSecondary = kTRUE; }

  Bool_t fDisplayOn;
  TH2F *hxy, *hxz, *hzphi;
  TCanvas *display;
  inline void SwitchOnDisplay() { fDisplayOn = kTRUE; }
  void DrawGeometry();
  void DrawHits(PndTrkHitList *hitlist);
  void Refresh();
  // conformal
  TH2F *huv;
  void RefreshConf();
  void DrawGeometryConf(double x1, double y1, double x2, double y2);
  void DrawConfHit(double x, double y, double r, int marker = 2);
  void LightCluster(PndTrkCluster *cluster);
  void DrawLegendreHisto();
  // z
  void RefreshZ();
  void DrawZGeometry(int whichone = 1, double phimin = 0, double phimax = 360, double zmin = -43, double zmax = 113);
  void DontUseMvdPix() { fUseMVDPix = kFALSE; }
  void DontUseMvdStr() { fUseMVDStr = kFALSE; }
  void DontUseStt() { fUseSTT = kFALSE; }

  Bool_t DoesRealHitBelong(PndTrkHit *hit, double x0, double y0, double R);
  Bool_t DoesConfHitBelong(PndTrkConformalHit *hit, double fitm, double fitp);

  void RePrepareLegendre(PndTrkCluster *cluster);
  void PrepareLegendre();
  Int_t ApplyLegendre(double &theta_max, double &r_max);
  Int_t ApplyLegendre(PndTrkCluster *cluster, double &theta_max, double &r_max);
  Int_t ExtractLegendre(Int_t mode, double &theta_max, double &r_max);

  Bool_t ZPhiFit(int iter, PndTrkCluster *cluster, double &fitm, double &fitp);
  PndTrkCluster *CleanupZPhiFit(PndTrkCluster *cluster, double fitm, double fitp);
  double ComputeZRediduals(PndTrkCluster *cluster, double fitm, double fitp);
  double CorrectZ(PndTrkCluster *cluster, double deltaz, double fitm, double fitp);

 private:
  Int_t fNofMvdPixHits, fNofMvdStrHits, fNofSttHits, fNofTriplets, fNofHits;

  /** Input array of PndSttPoints **/
  TClonesArray *fSttPointArray;
  /** Input array of PndSttHit **/
  TClonesArray *fSttHitArray;

  /** Input array of MvdPixelHitArray **/
  TClonesArray *fMvdPixelHitArray;
  /** Input array of MvdStripHitArray **/
  TClonesArray *fMvdStripHitArray;

  TClonesArray *fTrackArray, *fTrackCandArray;

  TClonesArray *fTubeArray;

  PndGeoSttPar *fSttParameters; //  CHECK added
  char fSttBranch[200], fMvdPixelBranch[200], fMvdStripBranch[200];

  PndSttMapCreator *fMapper;

  Int_t fEventCounter; // , fVerbose;

  PndTrkSttHitList *stthitlist;
  PndTrkSdsHitList *mvdpixhitlist;
  PndTrkSdsHitList *mvdstrhitlist;
  Double_t fSttParalDistance, fSttToMvdStripDistance;

  double fDeltaThetaRad;
  //  TSpectrum2 *s;
  PndTrkLegendreTransform *legendre;
  Bool_t fPersistence, fUseMVDPix, fUseMVDStr, fUseSTT, fSecondary, fInitDone;

  PndTrkConformalTransform *conform;
  PndTrkConformalHitList *conformalhitlist;
  PndTrkTools *tools;

  PndTrkHit *fRefHit;

  Double_t fMvdPix_RealDistLimit, fMvdStr_RealDistLimit, fStt_RealDistLimit, fMvdPix_ConfDistLimit, fMvdStr_ConfDistLimit, fStt_ConfDistLimit;

  std::vector<std::pair<double, double>> fFoundPeaks;
  double fTime;
  TStopwatch *fTimer;
  PndTrkFitter *fFitter;
  ClassDef(PndTrkLegendreSecTask, 1);
};

#endif
