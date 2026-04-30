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

#ifndef PNDTRKLEGENDRENEW_H
#define PNDTRKLEGENDRENEW_H

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
#include "PndTrkNeighboringMap.h"

// ROOT
#include "TH2F.h"
#include "TCanvas.h"

#define MAXNOFHITS 1000 // CHECK

class TClonesArray;
class TObjectArray;
class PndTrkLegendreTransform;
class PndTrkConformalTransform;
class PndTrkLegendreNew : public FairTask {

 public:
  /** Default constructor **/
  PndTrkLegendreNew();
  PndTrkLegendreNew(int verbose);

  /** Destructor **/
  ~PndTrkLegendreNew();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  void SetParContainers();

  void Initialize();
  void Reset();

  inline void SwitchOnDisplay() { fDisplayOn = kTRUE; }
  void DrawGeometry();
  void DrawHits(PndTrkHitList *hitlist);
  void DrawLists();
  void DrawNeighborings();
  void DrawNeighboringsToHit(PndTrkHit *hit);
  void Refresh();
  void RefreshConf();
  void DrawGeometryConf(double x1, double x2, double y1, double y2);
  void DrawConfHit(double x, double y, double r, int marker = 2);

  void LightCluster(PndTrkCluster *cluster);
  void DrawLegendreHisto();

  void ComputePlaneExtremities(PndTrkCluster *cluster);

  Int_t FillConformalHitList(PndTrkCluster *cluster);
  void FillLegendreHisto(PndTrkCluster *cluster);
  void ComputeTraAndRot(PndTrkHit *hit, Double_t &delta, Double_t trasl[2]);
  PndTrkHit *FindSttReferenceHit(int isec = -1);
  PndTrkHit *FindMvdPixelReferenceHit();
  PndTrkHit *FindMvdStripReferenceHit();
  PndTrkHit *FindMvdReferenceHit();
  PndTrkHit *FindReferenceHit();
  PndTrkHit *FindReferenceHit(PndTrkCluster *cluster);

  void RePrepareLegendre(PndTrkCluster *cluster);
  Int_t ApplyLegendre(PndTrkCluster *cluster, double &theta_max, double &r_max);
  Int_t ExtractLegendre(Int_t mode, double &theta_max, double &r_max);

  void FromConformalToRealTrack(double fitm, double fitp, double &x0, double &y0, double &R);
  void FromRealToConformalTrack(double x0, double y0, double R, double &fitm, double &fitp);

  void SearchSecondaryTracks() { fSecondary = kTRUE; }

  PndTrkClusterList CreateFullClusterization();
  PndTrkClusterList CreateFullClusterization2();

  Int_t CountTracksInCluster(PndTrkCluster *cluster);
  Int_t CountTracksInSkewSector(PndTrkCluster *cluster);
  Int_t CountTracksInCluster(PndTrkCluster *cluster, Int_t where);

  Int_t ClusterToConformal(PndTrkCluster *cluster);
  PndTrkTrack *LegendreFit(PndTrkCluster *cluster);
  PndTrkCluster *CreateClusterAroundTrack(PndTrkTrack *track);

  void AnalyticalFit(PndTrkCluster *cluster, double xc, double yc, double R, double &fitm, double &fitq);
  void AnalyticalFit2(PndTrkCluster *cluster, double fitm, double fitp, double &fitm2, double &fip2);
  void IntersectionFinder(PndTrkHit *hit, double xc, double yc, double R);
  void IntersectionFinder(PndTrkConformalHit *chit, double fitm, double fitp);

  void FillHitMap();

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

  PndTrkNeighboringMap *fHitMap;
  double fUmin, fUmax, fVmin, fVmax, fRmin, fRmax, fThetamin, fThetamax;

  // display
  Bool_t fDisplayOn;
  TH2F *hxy, *hxz, *hzphi;
  TCanvas *display;
  TH2F *huv;

  ClassDef(PndTrkLegendreNew, 1);
};

#endif
