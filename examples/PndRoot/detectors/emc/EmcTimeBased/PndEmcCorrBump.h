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

//-----------------------------------------------------------------------
// File and Version Information:
// $Id: $
// Software developed for the PANDA Detector at GSI.
// Author List:
//	Jan Zhong
//---------------------------------------------------------------------
//#pragma once
#ifndef PNDEMCCORRBUMP_H
#define PNDEMCCORRBUMP_H

#include "FairTask.h"
//#include <string>
#include <vector>
//#include <list>
#include "TVectorD.h"
#include "PndEmcDigiCalibrator.h"
class TClonesArray;
class TObjectArray;
class PndEmcCluster;
class PndEmcGeoPar;
class PndEmcDigiPar;
class PndEmcRecoPar;
class PndEmcDigi;
class PndEmcBump;
// class PndEmcDigiCalibrator;

class PndEmcCorrBump : public FairTask {

 public:
  // Constructors

  PndEmcCorrBump(Int_t verbose = 0, Bool_t storeclusters = kTRUE);

  // Destructor

  virtual ~PndEmcCorrBump();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  void SetStorageOfData(Bool_t val); // Method to specify whether clusters are stored or not.
  void FinishTask();

  void RunTimeBased() { fTimeOrderedDigi = kTRUE; }

 private:
  /** Input array of CbmDigis **/
  TClonesArray *fDigiArray;

  /** Input array of Hits and MC Tracks ... needed for MC **/
  TClonesArray *fSharedDigiArray;
  TClonesArray *fBumpArray;

  /** Output array of PndEmcClusters **/
  // TClonesArray* fClusterArray;

  // std::vector<PndEmcCluster*> fClusterList;

  Int_t fMapVersion;

  static Int_t fEventCounter;

  PndEmcGeoPar *fGeoPar;   /** Geometry parameter container **/
  PndEmcDigiPar *fDigiPar; /** Digitisation parameter container **/
  PndEmcRecoPar *fRecoPar; /** Reconstruction parameter container **/
  /** Get parameter containers **/
  virtual void SetParContainers();

  /** Verbosity level **/
  Int_t fVerbose;

  Bool_t fStoreClusters;
  Bool_t fTimeOrderedDigi;
  // for time-order reconstruction
  // std::vector<PndEmcDigi*> fBufferofDigisToBeDetermined;

  PndEmcCorrBump(const PndEmcCorrBump &L);
  PndEmcCorrBump &operator=(const PndEmcCorrBump &) { return *this; };

  // task counter
  Int_t HowManyDigi;
  Int_t HowManyCluster;
  // calibration was replaced by a class
  // coefficients
  // TVectorD CoeffMod3;
  // TVectorD CoeffMod5;
  // TVectorD CoeffModo;
  // static Double_t fTimeWindowOfSeedDigi[5][17];
  // static Double_t fTimeWindowOfShowerDigi[5][17];
  // Double_t CalibrationEvtTimeByDigi(PndEmcDigi* theDigi, bool PrintOut=kFALSE) const;
  // Int_t GetIdxByEnergy(Double_t energy) const;
  PndEmcDigiCalibrator digiCalibrator;

  TClonesArray *fBumpArrayTBD;

  std::vector<Double_t> fClusterPosParam;

  ClassDef(PndEmcCorrBump, 1)
};
#endif // PNDEMCMAKECLUSTER_HH
