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


/** PndRichHitFinder.h
 **
 **/

#ifndef PNDRICHHITFINDER_H
#define PNDRICHHITFINDER_H

#include <PndPersistencyTask.h>
#include "PndRichDigi.h"
#include "FairTSBufferFunctional.h"
#include "FairGeoVector.h"
#include "FairGeoTransform.h"
#include "FairMCEventHeader.h"
#include "TVector3.h"
//#include "TRandom.h"
#include "TGeoMatrix.h"
#include "TGeoBBox.h"
#include "PndGeoHandling.h"

#include "PndRichGeo.h"
#include "PndRichPDHit.h"
#include <string>
#include <vector>

class TClonesArray;

class PndRichHitFinder : public PndPersistencyTask {
 public:
  /** Default constructor **/
  PndRichHitFinder();

  /** Standard constructor **/
  PndRichHitFinder(Int_t iVerbose);

  /** Named constructor **/
  PndRichHitFinder(const char *name, Int_t iVerbose);

  /** Destructor **/
  virtual ~PndRichHitFinder();

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

  /** Input array of PndRichDigis **/
  TClonesArray *fDigiArray;

  /** Output array of PndRichPDHits **/
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
  PndRichGeo *fGeo;
  Int_t fEventNr;
  PndRichDigi *fDigi;
  Int_t fHitNumber;

  Double_t fThreshold;
  Double_t petime;
  Double_t fTimeStep;
  Int_t fTimeGate;
  Int_t fInd;
  Int_t fIBuffer;
  Int_t fIBufferPrev;
  std::vector<Int_t> fBufferNumHits;
  std::vector<Double_t> fBufferStartTime;
  std::vector<std::vector<Double_t>> fHitsBuffer;

  BinaryFunctor *fStopFunctor;
  BinaryFunctor *fGapFunctor;

  std::vector<PndRichPDHit> fPDHitList;

  ClassDef(PndRichHitFinder, 1);
};

#endif
