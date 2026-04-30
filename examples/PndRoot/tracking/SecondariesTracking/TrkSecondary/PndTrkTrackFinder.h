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

#ifndef PNDTRKTRACKFINDER_H
#define PNDTRKTRACKFINDER_H

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
#include "PndTrkSciTHitList.h"
#include "PndTrkGemHitList.h"
#include "PndTrkCluster.h"
#include "PndTrkClusterList.h"
#include "PndTrkTrack.h"
#include "PndTrkTrackList.h"
#include "PndTrkConformalHitList.h"
#include "PndTrkSkewHitList.h"
#include "PndTrkFitter.h"
#include "PndTrkNeighboringMap.h"
#include "PndTrkGemCombinatorial.h"

// ROOT
#include "TH2F.h"
#include "TCanvas.h"
#include <TStopwatch.h>

#define MAXNOFHITS 1000 // CHECK

class TClonesArray;
class TObjectArray;
class PndTrkLegendreTransform;
class PndTrkConformalTransform;
class PndTrkTrackFinder : public FairTask {

 public:
  /** Default constructor **/
  PndTrkTrackFinder();
  PndTrkTrackFinder(int verbose);

  /** Destructor **/
  ~PndTrkTrackFinder();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  void SetParContainers();

  //  void Apollonius(PndTrkCluster *cluster, double *X, double *Y, double *R);
  void Apollonius(PndTrkCluster *cluster, std::vector<double> &X, std::vector<double> &Y, std::vector<double> &R);
  void Apollonius(PndTrkHit *hit1, PndTrkHit *hit2, PndTrkHit *hit3, std::vector<double> &X, std::vector<double> &Y, std::vector<double> &R);
  void CircleBy3Points(PndTrkHit *hit1, PndTrkHit *hit2, PndTrkHit *hit3, double &X, double &Y, double &R);

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
  void FromConformalToRealTrackParabola(double fita, double fitb, double fitc, double &x0, double &y0, double &R, double &epsilon);
  void FromRealToConformalTrack(double x0, double y0, double R, double &fitm, double &fitp);

  void SearchSecondaryTracks() { fSecondary = kTRUE; }

  PndTrkClusterList CreateFullClusterization();
  //  PndTrkClusterList CreateFullClusterization2();

  Int_t CountTracksInCluster(PndTrkCluster *cluster);
  Int_t CountTracksInSkewSector(PndTrkCluster *cluster);
  Int_t CountTracksInCluster(PndTrkCluster *cluster, Int_t where);
  Int_t CountPossibleTracks();

  Int_t ClusterToConformal(PndTrkCluster *cluster);
  PndTrkTrack *LegendreFit(PndTrkCluster *cluster);
  PndTrkCluster *CreateClusterAroundTrack(PndTrkTrack *track);

  Bool_t AnalyticalFit(PndTrkCluster *cluster, double xc, double yc, double R, double &fitm, double &fitq);
  void AnalyticalFit2(PndTrkCluster *cluster, double fitm, double fitp, double &fitm2, double &fip2);
  Bool_t AnalyticalParabolaFit(PndTrkCluster *cluster, double xc, double yc, double R, double &fita, double &fitb, Double_t &fitc, Double_t &epsilon);

  void IntersectionFinder(PndTrkHit *hit, double xc, double yc, double R);
  void IntersectionFinder(PndTrkConformalHit *chit, double fitm, double fitp);

  void FillHitMap();

  PndTrkCluster CreateSkewHitList(PndTrkTrack *track);
  PndTrkCluster CleanUpSkewHitList(PndTrkCluster *skewhitlist);
  void DrawZGeometry(double phimin = 0, double phimax = 360, double zmin = -43, double zmax = 113);

  Bool_t MinuitFit(PndTrkCluster *cluster, double mstart, double qstart, double &fitm, double &fitq);
  Bool_t MinuitFit2(PndTrkCluster *cluster, double xstart, double ystart, double rstart, double &xc, double &yc, double &R, double &sign);

  void DeletePrimaryHits() { fDelPrim = kTRUE; }
  Int_t RecreateHitArrays(std::map<int, std::vector<int>> &det_to_hitids);
  std::map<int, bool> PrimaryCheck(Int_t detid, std::map<int, std::vector<int>> &det_to_hitids);

 private:
  Int_t fNofMvdPixHits, fNofMvdStrHits, fNofSttHits, fNofTriplets, fNofHits, fNofSciTHits, fNofGemHits;

  /** Input array of PndSttPoints **/
  TClonesArray *fSttPointArray;
  /** Input array of PndSttHit **/
  TClonesArray *fSttHitArray;

  /** Input array of MvdPixelHitArray **/
  TClonesArray *fMvdPixelHitArray;
  /** Input array of MvdStripHitArray **/
  TClonesArray *fMvdStripHitArray;

  /** Input array of SciTHitArray **/
  TClonesArray *fSciTHitArray;
  /** Input array of GemHitArray **/
  TClonesArray *fGemHitArray;

  TClonesArray *fTrackArray, *fTrackCandArray, *fTrkTrackArray, *fPrimaryTrackArray;

  TClonesArray *fTubeArray;

  PndGeoSttPar *fSttParameters; //  CHECK added
  char fSttBranch[200], fMvdPixelBranch[200], fMvdStripBranch[200], fSciTBranch[200], fGemBranch[200];

  PndSttMapCreator *fMapper;

  Int_t fEventCounter; // , fVerbose;

  PndTrkGemCombinatorial *fCombiFinder;

  PndTrkSttHitList *stthitlist;
  PndTrkSdsHitList *mvdpixhitlist;
  PndTrkSdsHitList *mvdstrhitlist;
  PndTrkSciTHitList *scithitlist;
  PndTrkGemHitList *gemhitlist;
  Double_t fSttParalDistance, fSttToMvdStripDistance;

  double fDeltaThetaRad;
  //  TSpectrum2 *s;
  PndTrkLegendreTransform *legendre;
  Bool_t fDisplayOn, fPersistence, fUseMVDPix, fUseMVDStr, fUseSTT, fUseSCIT, fUseGEM, fSecondary, fInitDone;
  Double_t fMvdPix_RealDistLimit, fMvdStr_RealDistLimit, fStt_RealDistLimit, fMvdPix_ConfDistLimit, fMvdStr_ConfDistLimit, fStt_ConfDistLimit;
  double fUmin, fUmax, fVmin, fVmax, fRmin, fRmax, fThetamin, fThetamax;
  PndTrkHit *fRefHit;
  Bool_t fDelPrim;
  Int_t fNofPrimaries;

  PndTrkConformalTransform *conform;
  PndTrkConformalHitList *fConformalHitList;
  PndTrkTools *tools;

  std::vector<std::pair<double, double>> fFoundPeaks;
  double fTime;
  TStopwatch *fTimer;
  PndTrkFitter *fFitter;

  PndTrkNeighboringMap *fHitMap;

  // display
  TH2F *hxy, *hxz, *hzphi;
  TCanvas *display;
  TH2F *huv;
  TH2F *fLineHisto;
  PndTrkCluster *fCluster;
  PndTrkCluster *fFinalCluster;
  PndTrkCluster *fIndivCluster;
  PndTrkCluster *fSkewCluster;
  PndTrkCluster *fFinalSkewCluster;
  PndTrkCluster *fIndivisibleHitList;

  PndTrkTrackList *fTrackList;

  ClassDef(PndTrkTrackFinder, 1);
};

#endif
