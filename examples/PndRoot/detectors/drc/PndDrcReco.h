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

// -----------------------------------------
// PndDrcReco.h
//
// Created on: 04.03.2016
// Author: R.Dzhygadlo at gsi.de
// -----------------------------------------
// look-up-table and time imaging reconstruction

#ifndef PNDDRCRECO_H
#define PNDDRCRECO_H

#include "FairTask.h"
#include "TClonesArray.h"
#include "PndMCTrack.h"
#include "PndDrcBarPoint.h"
#include "PndDrcEVPoint.h"
#include "PndDrcPDPoint.h"
#include "PndDrcHit.h"
#include "PndDrcPDHit.h"
#include "PndDrcDigi.h"
#include "PndGeoDrc.h"

#include "TString.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TSpectrum.h"
#include "TKey.h"
#include "TRandom.h"
#include "TCanvas.h"
#include "TTree.h"

#include "PndDrcTrackInfo.h"
#include "PndDrcPhotonInfo.h"
#include "PndDrcAmbiguityInfo.h"

using std::array;

class PndDrcReco : public FairTask {

 public:
  PndDrcReco();
  PndDrcReco(TString outFile, TString lutFile, TString pdfFile, int verbose = 0, double r1 = 0, double r2 = 0);

  void SetMethod(int var) { fMethod = var; }
  virtual ~PndDrcReco(){};

  virtual InitStatus Init();

  virtual void Exec(Option_t *option);

  virtual void Finish();

 private:
  void DetermineCherenkov(int boxId, int barId);
  void DetermineBarId(double &boxPhi, int &barId);
  void LookUpTable(int barId, int sensorId);
  void TimeImaging(int sensorId);
  void BuildPdfs(TF1 &pdfuf, TF1 &pdfus, int ch, int barId, TVector3 mom, double posz, bool reflected);
  void TimeImagingWithAnalyticalPdf(int barId, int ch);

  double FindPeak();
  int FindPdg(double mom, double cangle);
  void CanvasAdd(TString name = "c", int w = 800, int h = 400);
  void CanvasSave(TString path = "data/reco");
  PndGeoDrc *fGeo;
  double fBboxNum, fPipehAngle, fDphi, fBarPhi;

  TClonesArray *fMCArray; // DRC MCPoints in the photon detector
  TClonesArray *fBarPointArray;
  TClonesArray *fEVPointArray;
  TClonesArray *fPDPointArray; // DRC points in the photon detector
  TClonesArray *fDigiArray;
  TClonesArray *fPDHitArray; // DRC Hits in the photon detector
  TClonesArray *fDrcTrackInfoArray;
  array<TClonesArray *, 5> fLut;

  TFile *fFile;
  TFile *fFileOut;
  TTree *fTree;
  TTree *fTreeOut;

  PndMCTrack *fMCTrack;
  PndDrcBarPoint *fBarPoint;
  PndDrcEVPoint *fEVPoint;
  PndDrcPDPoint *fPDPoint;
  PndDrcDigi *fDigi;
  PndDrcPDHit *fPDHit;

  // Set the parameters to the default values.
  void SetDefaultParameters();

  // Verbosity level
  int fVerbose;
  int nevents;
  bool fHist_flag;
  TString fOutFile;
  TString fLutFile;
  TString fPdfFile;
  TString fCorrFile;
  int fEvType, fRadType, fLensType;
  TH1F *fHist, *fHist1, *fHist2;
  TF1 *fFit;
  TSpectrum *fSpect;

  int fMethod;
  TVector3 fMomInBar;
  TVector3 fPosInBar;
  double fTimeInBar;
  double fTimeHit;
  double fLenz;
  double fCriticalAngle;
  bool fReflected;

  array<double, 5> fPdg, fMass, fAngle, fSpr, fCangle, fEfficiency, fMissId;
  array<double, 3> fLikelihood, fSeparation;
  array<TF1 *, 5> fFunc, fFnph;
  array<int, 2> fPidLike;
  array<int, 5> fEvents, fEventsEff, fEventsMis, fHits, fHits1;
  array<int, 3000> fParticleArray;
  array<array<int, 5>, 3> fHitsE;
  array<array<double, 5>, 3> fLk, fNph;
  array<TH1F *, 5> fhTang, fhTime, fhDiff;
  array<array<TH1F *, 5>, 3> fhNph, fhLk;
  array<array<TH1F *, 5>, 800> fhTimeA;
  array<TH1F *, 9> fhCorr;
  array<array<array<array<TH1F *, 1100>, 120>, 40>, 5> fhPdf;
  array<array<array<int, 150>, 50>, 9> fCorr_spr, fCorr_mean;
  array<array<array<TH1F *, 150>, 40>, 5> fhNphArr;

  double fMom, fTheta, fPhi;
  int fMcTrackId, fPidTrue, fPidDist;
  TVector3 fNx, fNy;
  double fR1, fR2;
  bool fStoreCorr;
  int fBin_mom, fBin_theta;

  TList *fCanvasList;
  TCanvas *gg_c;

  ClassDef(PndDrcReco, 1)
};

#endif
