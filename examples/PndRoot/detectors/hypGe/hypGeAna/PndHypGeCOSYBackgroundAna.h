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

#ifndef PNDHYPGECOSYBACKGROUNDANA_H
#define PNDHYPGECOSYBACKGROUNDANA_H 1

#include <FairTask.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TVector3.h>
#include <TString.h>
#include <TClonesArray.h>
#include <TRandom.h>
#include <fstream>
#include <TGeoManager.h>
#include <TROOT.h>
//#include "PndHypGeCOSYBackgroundAnaStorage.h"
#include <TSpectrum.h>
#include <TF1.h>
#include <TCanvas.h>

class PndHypGeCOSYBackgroundAna : public FairTask {
 public:
  PndHypGeCOSYBackgroundAna();
  PndHypGeCOSYBackgroundAna(Int_t nEvents);
  ~PndHypGeCOSYBackgroundAna();

  virtual InitStatus Init();
  virtual void Exec(Option_t *opt);
  void Finish();

 protected:
  TGeoManager *fgeom;

  TClonesArray *fMcTr;
  TClonesArray *fHyp;
  TClonesArray *fHypGe;

  TH1D *hNHits;
  TH1D *hCrystalHit;
  TH1D *hNeutronOrigin;
  TH2D *hCrystalOrigin;
  TH1D *hNeutronEkin;
  TH2D *hNeutronEkinOrigin;
  TH1D *hNeutronEnergyLoss;

  TH1D *hAllParticlesGe;
  TH1D *hAllParticlesCrystal1;
  TH1D *hAllParticlesCrystal2;
  TH1D *hAllParticlesPiezo;
  TH1D *hAllParticlesSiPm1;
  TH1D *hAllParticlesSiPm2;

  TH2D *hEkinAllParticles;
  TH2D *hEkinAllParticlesCrystal1;
  TH2D *hEkinAllParticlesCrystal2;
  TH2D *hEkinAllParticlesPiezo;
  TH2D *hEkinAllParticlesSiPm1;
  TH2D *hEkinAllParticlesSiPm2;

  TH2D *hGammaEkinOrigin;

  TCanvas *cNHits;
  TCanvas *cCrystalHit;
  TCanvas *cNeutronOrigin;
  TCanvas *cCrystalOrigin;
  TCanvas *cNeutronEkin;
  TCanvas *cNeutronEkinOrigin;
  TCanvas *cNeutronEnergyLoss;
  TCanvas *cAllParticlesGe;
  TCanvas *cAllParticlesCrystal1;
  TCanvas *cAllParticlesCrystal2;
  TCanvas *cEkinAllParticles;
  TCanvas *cGammaEkinOrigin;

  TCanvas *cAllParticlesPiezo;
  TCanvas *cAllParticlesSiPm1;
  TCanvas *cAllParticlesSiPm2;
  TCanvas *cEkinAllParticlesSplit;

  TVector3 StartVertex;
  TString VertexVolumeName;

  TVector3 NeutronMomentum;
  Double_t NeutronEkin;
  TVector3 ParticleMomentum;
  Double_t ParticleEkin;

  Long_t EvtCount;
  Long_t NumberOfEvents;

  TString fName;

  // Analysis

  ClassDef(PndHypGeCOSYBackgroundAna, 1);
};

#endif /* PNDHYPGECOSYBACKGROUNDANA_H */
