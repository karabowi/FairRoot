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

/******************************************************

Analysis Task created by M.Steinen steinen@kph.uni-mainz.de
Analysis of Gamma Simulation with hypGe detectors
*******************************************************/

#ifndef PNDHYPGEGAMMAANA_H
#define PNDHYPGEGAMMAANA_H 1

#include <FairTask.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TClonesArray.h>
#include <TRandom.h>
#include <fstream>
#include <set>
//#include "PndHypGeGammaAnaStorage.h"
#include "PndHypGePoint.h"
#include "PndHypPoint.h"
#include "PndMCTrack.h"
#include <TSpectrum.h>
#include <TF1.h>
#include "TGeoManager.h"

class PndHypGeGammaAna : public FairTask {
 public:
  PndHypGeGammaAna();
  PndHypGeGammaAna(TString TxtFileName_Ext, Double_t GammaEnergy_Ext, Int_t nEvents = 0, Int_t nPeaks_Ext = 1, Int_t PeakToLook_Ext = 1);
  ~PndHypGeGammaAna();

  virtual InitStatus Init();
  virtual void Exec(Option_t *opt);
  void Finish();
  void SetParContainers();
  void SetTarget(Bool_t TargetAvailable = 1);
  void SetOmegaQuadrupolMode(Bool_t useOmegaQuadrupolMode_ext = 1);
  void SetPeakWidtchStrechFactor(Double_t PeakWidtchStrechFactor_ext = 1);

 protected:
  Double_t Compton(Double_t E, Double_t Th);
  Double_t invCompton(Double_t E, Double_t Eprime);
  Double_t SmearHit(Double_t Energy);
  Double_t CalculatePeakWidth(Double_t Energy);
  void FitSpectrum();
  void FittingOutput();
  void FindPeaks();
  void GetPeakContent();
  void ActivateComptonCalculations(Bool_t isActivated = 1);
  void CalculateCompton();
  void WriteHistogramsToFile();
  void HistogramCosmetics(TH1D *histo, TString XTitle, Double_t XTitleSize, Double_t XTitleOffset, Double_t XLabelSize, TString YTitle, Double_t YTitleSize, Double_t YTitleOffset,
                          Double_t YLabelSize);
  void HistogramCosmetics2D(TH2D *histo, TString XTitle, Double_t XTitleSize, Double_t XTitleOffset, Double_t XLabelSize, TString YTitle, Double_t YTitleSize,
                            Double_t YTitleOffset, Double_t YLabelSize);

  bool InTargetAbsorbedGammaWouldHitGermanium();
  TString TxtFileName;
  ofstream TxtFile;

  Bool_t useOmegaQuadrupolMode;

  Double_t ResConstPar;
  Double_t ResGradientPar;
  Double_t PeakWidtchStrechFactor;
  Double_t PeakWidth;

  Double_t bufferEnergy;

  TH1D *hGamEnergy;
  TH1D *hGamEnergyTargetBackground;
  TH1D *hNumberOfHits;
  TH1D *hThetaCheck;
  TH1D *hThetaCheckPrimary;
  TH1D *hAbsorption;
  TH2D *h2AbsorptionDistanceAngle;

  TClonesArray *fMcTr;
  TClonesArray *fMc;
  TClonesArray *fGe;
  TClonesArray *fGeAl;
  TClonesArray *fSilicon;
  TClonesArray *fTargetOther;

  PndHypGePoint *fHitGe;
  ;
  PndHypPoint *fHitSi;
  PndHypPoint *fHitTargetOther;
  PndMCTrack *fMCGam;
  PndMCTrack *fMCGamPrimary;

  Long_t EvtCount;
  Long_t NumberOfEvents;
  Double_t GammaEnergy;
  Double_t iHistoUpperLimit;
  TString fName;

  // PndHypGeGammaAnaStorage*	fStorage;

  FairRootManager *ioman;
  // Exec
  std::set<int> SetOfCrystalHit;
  TVector3 fStartVertex;
  TString fVertexVolumeName;

  // Analysis
  Int_t nPeaks;
  Int_t PeakToLook;
  Double_t PeakX;
  Double_t PeakY;
  Double_t DPeakY;
  Double_t SumPeak;
  Double_t DSumPeak;
  Bool_t DoComptonCalculations;
  Double_t lowComptonAngle;
  Double_t highComptonAngle;
  Double_t SumCompton;
  Double_t DSumCompton;
  Double_t PeakToCompton;
  Double_t DPeakToCompton;

  TF1 *GausBG;

  Float_t *xpeaks; //!
  Float_t *ypeaks; //!

  Bool_t fTargetAvailable;

  ClassDef(PndHypGeGammaAna, 1);
};

#endif /* PNDHYPGEGAMMAANA_H */
