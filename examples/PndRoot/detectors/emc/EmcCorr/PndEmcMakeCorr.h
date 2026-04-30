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
// File and Version Information:
// 	$Id:$
//
// Description:
//	Class EmcCalibration
//      Do an energy and theta corrections
//      (at the moment for photons, 4.02.2010)
//
// Author List:
//      A. Biegun
//      M. Babai
//------------------------------------------------------------------------
//#pragma once
#ifndef PNDEMCMAKECORR_H
#define PNDEMCMAKECORR_H

#include <PndPersistencyTask.h>
#include "TObject.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include <TH2.h>
#include <TFile.h>
#include <TString.h>

class TClonesArray;
class TObjectArray;
class PndEmcDigiPar;
class PndEmcRecoPar;

class PndEmcCluster;
class PndEmcDigi;

class PndEmcMakeCorr : public PndPersistencyTask {

 public:
  // Constructors
  PndEmcMakeCorr(Int_t verbose = 0, TString transportModel = "TGeant3", TString clusterType = "EmcBump");

  // Destructor
  virtual ~PndEmcMakeCorr();

  /** Virtual method Init **/
  virtual InitStatus Init();

  // Methods
  TFile *f, *f0, *f1, *f2, *f3;

  Int_t FindTheBin(TH2 *lookup_table, Float_t value_x, Float_t value_y, Int_t &bin_x, Int_t &bin_y);

  Double_t GetValueInZ(TH2 *lookup_table, Float_t value_x, Float_t value_y, Bool_t use_interpolation = kFALSE);

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  // Modifiers
  void SetStorageOfData(Bool_t val);

 protected:
  Int_t fClusterIndex; // Index of cluster the bump is made in TClonesArray

 private:
  /** Input array of PndEmcClusters **/
  TClonesArray *fClusterArray;

  /** Output array of PndEmcBumps **/
  TClonesArray *fClusterArrayCorr;

  PndEmcDigiPar *fDigiPar; /** Reconstruction parameter container **/
  PndEmcRecoPar *fRecoPar; /** Reconstruction parameter container **/

  /** Get parameter containers **/
  virtual void SetParContainers();

  Int_t fVerbose;
  Bool_t fStoreClustersCorr;

  // Data members
  //
  // Target EMC
  TString nameEn[4];
  TString nameTh[4];
  TH2F *hEn[4];
  TH2F *hTh[4];

  // Shashlyk
  TString nameEn5[4];
  TString nameTh5[4];
  TH2F *hEn5[4];
  TH2F *hTh5[4];

  TString fModel;
  TString fClusterType;

  TString corrFileName[4];
  TString fPartName[4];

  PndEmcMakeCorr(const PndEmcMakeCorr &L);
  PndEmcMakeCorr &operator=(const PndEmcMakeCorr &) { return *this; };

  ClassDef(PndEmcMakeCorr, 1)
};
#endif // PNDEMCMAKECORR_HH
