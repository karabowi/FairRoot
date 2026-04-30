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


/** PndDrcHitFinder.h
 **
 **/

#ifndef PNDDRCHITFINDER_H
#define PNDDRCHITFINDER_H

#include <PndPersistencyTask.h>
#include "PndDrcDigi.h"
#include "FairTSBufferFunctional.h"
#include "FairGeoVector.h"
#include "FairGeoTransform.h"
#include "FairMCEventHeader.h"
#include "TVector3.h"
//#include "TRandom.h"
#include "TGeoMatrix.h"
#include "TGeoBBox.h"
#include "PndGeoHandling.h"

#include "PndGeoDrc.h"
#include "PndDrcPDHit.h"
#include <string>
#include <vector>

class TClonesArray;

class PndDrcHitFinder : public PndPersistencyTask {
 public:
  /** Default constructor **/
  PndDrcHitFinder();

  /** Standard constructor **/
  PndDrcHitFinder(Int_t iVerbose);

  /** Named constructor **/
  PndDrcHitFinder(const char *name, Int_t iVerbose);

  /** Destructor **/
  virtual ~PndDrcHitFinder();

  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual InitStatus ReInit();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  virtual void FinishEvent();
  virtual void FinishTask();

  void SetPixelFactor(Int_t factor) { fPixelFactor = factor; }

 protected:
  Bool_t fDigiPixelMCInfo; // switch to turn on/off storing additional MC Info of Digis

  /** Input array of PndDrcDigis **/
  TClonesArray *fDigiArray;

  /** Output array of PndDrcPDHits **/
  TClonesArray *fPdHitArray;

  FairMCEventHeader *fMCEventHeader;

  void Register();
  void Reset();
  void ProduceHits();

  TString fInBranchName;
  Int_t fPixelFactor;
  Double_t fPixelSize; // pixel size;
  Double_t fPixelGap;  // gap betwen neighboring pixels
  Double_t fPixelStep;
  Double_t fMcpActiveArea; // dimension of the active area of one MCP
  Int_t fNpix;             // pixel rows in one one MCP
  Int_t fPixelHits;
  PndGeoHandling *fGeoH;
  PndGeoDrc *fGeo;
  Int_t fEventNr;
  PndDrcDigi *fDigi;

  BinaryFunctor *fStopFunctor;
  BinaryFunctor *fGapFunctor;

  std::vector<PndDrcPDHit> fPDHitList;

  ClassDef(PndDrcHitFinder, 1);
};

#endif
