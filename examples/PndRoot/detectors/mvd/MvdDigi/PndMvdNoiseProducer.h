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

// -------------------------------------------------------------------------
// -----                PndMvdNoiseProducer header file                -----
// -----                  Created 01.07.08  by R.Kliemt                -----
// -------------------------------------------------------------------------

/** PndMvdNoiseProducer.h
 *@author R.Kliemt <r.kliemt@physik.tu-dresden.de>
 **
 ** The Noise Producer adds fake hits to silicon sensor channels (strips and
 ** Pixels)
 **/

#ifndef PNDMVDNOISEPRODUCER_H
#define PNDMVDNOISEPRODUCER_H

#include <PndPersistencyTask.h>
#include "FairMCEventHeader.h"

#include <vector>

#include "TRandom.h"
//#include "PndMvdGeoPar.h"
#include "PndGeoHandling.h"
#include "PndSdsStripDigiPar.h"
#include "PndSdsPixelDigiPar.h"
#include "PndSdsTotDigiPar.h"
#include "PndSdsChargeConversion.h"
#include "PndSdsDigiPixelWriteoutBuffer.h"
#include "PndSdsDigiStripWriteoutBuffer.h"
#include "PndSdsDigiStrip.h"
#include "PndSdsFE.h"
#include "TString.h"

class TClonesArray;

class PndMvdNoiseProducer : public PndPersistencyTask {
 public:
  /** Default constructor **/
  PndMvdNoiseProducer();

  /** Destructor **/
  virtual ~PndMvdNoiseProducer();

  PndMvdNoiseProducer(const PndMvdNoiseProducer &) = delete;
  PndMvdNoiseProducer &operator=(const PndMvdNoiseProducer &) = delete;

  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);
  virtual void FinishEvent();

  Double_t CalcDistFraction(Double_t spread, Double_t threshold);
  //   Int_t CalcChanWhite(Int_t chanleft, Double_t frac);
  Int_t CalcChargeAboveThreshold(Double_t spread, Double_t threshold);
  Double_t CalcReadoutCycles(Double_t clock);
  void AddDigiStrip(Int_t &iStrip, Int_t iPoint, Int_t sensorID, Int_t fe, Int_t chan, Double_t charge);
  void AddDigiPixel(Int_t &noisies, Int_t iPoint, Int_t sensorID, Int_t fe, Int_t col, Int_t row, Double_t charge);

  void FillSensorLists();

  void RunTimeBased() { fTimeOrderedDigi = kTRUE; }

 protected:
  Bool_t fTimeOrderedDigi; ///< parameter to switch to time ordered simulation

  TString fBranchName;
  /** In-Output array of PndSdsDigis **/
  TClonesArray *fDigiStripArray;
  TClonesArray *fDigiPixelArray;

  PndSdsDigiPixelWriteoutBuffer *fDigiPixelBuffer;
  PndSdsDigiStripWriteoutBuffer *fDigiStripBuffer;

  /** Parameter Containers **/
  PndSdsStripDigiPar *fDigiParRect;
  PndSdsStripDigiPar *fDigiParTrap;
  PndSdsPixelDigiPar *fDigiParPix;

  PndSdsTotDigiPar *fTotDigiParRect;
  PndSdsTotDigiPar *fTotDigiParTrap;
  PndSdsTotDigiPar *fTotDigiParPix;

  PndGeoHandling *fGeoH; //! Geometry name handling
  FairMCEventHeader *fMCEventheader;

  std::vector<Int_t> fPixelIds2;
  std::vector<Int_t> fPixelIds4;
  std::vector<Int_t> fPixelIds5;
  std::vector<Int_t> fPixelIds6;
  std::vector<Int_t> fStripRectLIds;
  std::vector<Int_t> fStripRectSIds;
  std::vector<Int_t> fStripTrapIds;

  PndSdsChargeConversion *fStripRectChargeConv;
  PndSdsChargeConversion *fStripTrapChargeConv;
  PndSdsChargeConversion *fCurrentChargeConv;
  PndSdsChargeConversion *fPixChargeConv;

  Int_t fNoiseSpread;
  Int_t fThreshold;
  Double_t fPreviousTime;

  //   void Register();

  //   void Reset();

  //   void ProduceHits();

  ClassDef(PndMvdNoiseProducer, 1);
};

#endif
