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

//--------------------------------------------------------------------------
// Description:
//	Class PndEmcClusterCalibrator
//      Do an energy and position corrections for EMC cluster
//      PndEmcClusterCalibrator is a factory which call specific method depending on the input parameter "method"
//      method=1 - PndEmcClusterHistCalibrator is called, which perform correction from the interpolation on 2-dimensional histogram on (Energy, theta)
//      method=2 - PndEmcClusterSimpleCalibrator is called, where energy correction is parametrised as a function of (Energy, theta)
//      PndEmcAbsClusterCalibrator - abstract interface class
//
// Author List:
//      D.Melnychuk
//      PndEmcClusterHistCalibrator class is based on code PndEmcMakeCorr.cxx
//      (A. Biegun, M. Babai)
//      PndEmcClusterSimpleCalibrator class is based on EmcPhotonSimpleCalib class in Babar framework (Jan Zhong)
//------------------------------------------------------------------------
//#pragma once
#ifndef PNDEMCCLUSTERCALIBRATOR_H
#define PNDEMCCLUSTERCALIBRATOR_H

#include "TObject.h"
#include "TVector3.h"
#include <TH2.h>
#include <TFile.h>
#include <TString.h>
#include <cstdlib>

class PndEmcCluster;
class PndEmcDigi;
class PndEmcClusterCalibrationParObject;

// Abstract interface class for EMC Cluster calibrator
class PndEmcAbsClusterCalibrator {

 public:
  // Constructors
  PndEmcAbsClusterCalibrator(Int_t version = 1);

  // Destructor
  virtual ~PndEmcAbsClusterCalibrator();

  // Methods
  virtual Double_t Energy(PndEmcCluster *clust, Int_t pid = 22) = 0;
  virtual TVector3 Where(PndEmcCluster *clust, Int_t pid = 22) = 0;

 protected:
  Int_t fVersion;
  TString fPath;
};

// Factory for creation EMC Cluster calibrator
class PndEmcClusterCalibrator {

 public:
  // Constructors
  PndEmcClusterCalibrator();

  // Destructor
  virtual ~PndEmcClusterCalibrator();

  static PndEmcAbsClusterCalibrator *MakeEmcClusterCalibrator(Int_t method, Int_t version = 1);

 private:
  PndEmcClusterCalibrator(const PndEmcClusterCalibrator &L);
  PndEmcClusterCalibrator &operator=(const PndEmcClusterCalibrator &) { return *this; };
};

class PndEmcClusterHistCalibrator : public PndEmcAbsClusterCalibrator {
 public:
  PndEmcClusterHistCalibrator(Int_t version = 1);
  virtual ~PndEmcClusterHistCalibrator();

  // Methods
  virtual Double_t Energy(PndEmcCluster *clust, Int_t pid = 22);
  virtual TVector3 Where(PndEmcCluster *clust, Int_t pid = 22);

 private:
  Int_t FindTheBin(TH2 *lookup_table, Float_t value_x, Float_t value_y, Int_t &bin_x, Int_t &bin_y);

  Double_t GetValueInZ(TH2 *lookup_table, Float_t value_x, Float_t value_y, Bool_t use_interpolation = kFALSE);

  TFile *fPhoton, *fElectron, *fPion;
  TH2F *fHEnergyRatioBarrelPhoton, *fHEnergyRatioFwdPhoton, *fHEnergyRatioBwdPhoton, *fHEnergyRatioShashlykPhoton;
  TH2F *fHThetaDiffBarrelPhoton, *fHThetaDiffFwdPhoton, *fHThetaDiffBwdPhoton, *fHThetaDiffShashlykPhoton;
  // 	TH2F *fHEnergyRatioBarrelElectron, *fHEnergyRatioFwdElectron, *fHEnergyRatioBwdElectron, *fHEnergyRatioShashlykElectron;
  // 	TH2F *fHThetaDiffBarrelElectron, *fHThetaDiffFwdElectron, *fHThetaDiffBwdElectron, *fHThetaDiffShashlykElectron;
  // 	TH2F *fHEnergyRatioBarrelPion, *fHEnergyRatioFwdPion, *fHEnergyRatioBwdPion, *fHEnergyRatioShashlykPion;
  // 	TH2F *fHThetaDiffBarrelPion, *fHThetaDiffFwdPion, *fHThetaDiffBwdPion, *fHThetaDiffShashlykPion;

  PndEmcClusterHistCalibrator(const PndEmcClusterHistCalibrator &L);
  PndEmcClusterHistCalibrator &operator=(const PndEmcClusterHistCalibrator &) { return *this; };
};

class PndEmcClusterSimpleCalibrator : public PndEmcAbsClusterCalibrator {
 public:
  PndEmcClusterSimpleCalibrator(Int_t version = 1);
  virtual ~PndEmcClusterSimpleCalibrator();

  void Init(){};

  // Methods
  virtual Double_t Energy(PndEmcCluster *clust, Int_t pid = 22);
  virtual TVector3 Where(PndEmcCluster *clust, Int_t pid = 22);

 private:
  PndEmcClusterCalibrationParObject *fParObject;

  PndEmcClusterSimpleCalibrator(const PndEmcClusterSimpleCalibrator &L);
  PndEmcClusterSimpleCalibrator &operator=(const PndEmcClusterSimpleCalibrator &) { return *this; };
};
#endif // PndEmcClusterCalibrator_HH
