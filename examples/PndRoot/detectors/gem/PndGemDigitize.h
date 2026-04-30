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

//* $Id: */

// -------------------------------------------------------------------------
// -----                    PndGemDigitize header file                 -----
// -----                  Created 12/02/2009 by R. Karabowicz          -----
// -------------------------------------------------------------------------

/** PndGemDigitise
 *@author Radoslaw Karabowicz <r.karabowicz@gsi.de>
 *@since 12.02.2009
 *@version 1.0
 **
 ** PANDA task class for digitising GEM
 ** Task level SIM
 ** Produces objects of type PndGemDigi out of PndGemMCPoint.
 **/

#ifndef PNDGEMDIGITIZE_H
#define PNDGEMDIGITIZE_H 1

#include <PndPersistencyTask.h>
#include "TStopwatch.h"
#include "TRandom.h"
#include "PndGemMCPoint.h"
#include "PndGemSensor.h"

#include <list>
#include <map>

class TClonesArray;
class PndGemDigiPar;
class PndGemDigiWriteoutBuffer;

class PndGemDigitize : public PndPersistencyTask {

 public:
  /** Default constructor **/
  PndGemDigitize();

  /** Standard constructor **/
  PndGemDigitize(Int_t iVerbose);

  /** Constructor with name **/
  PndGemDigitize(const char *name, Int_t iVerbose);

  /** Destructor **/
  virtual ~PndGemDigitize();

  /** Execution **/
  virtual void Exec(Option_t *opt);

  /** Switch saving outside hits **/
  void SaveOutsideHits(Bool_t bt = kTRUE) { fSaveOutsideHits = bt; }

  void SetRealisticResponse(Bool_t bt = kTRUE) { fRealisticResponse = bt; }

  void RunTimeBased(Bool_t bt = kTRUE) { fTimeOrderedDigi = bt; }

  Bool_t GetTimeBased() const { return fTimeOrderedDigi; }

  void SetShowerSigma(Double_t dd) { fSSigma = dd; } // sigam for realistic response

 private:
  PndGemDigiPar *fDigiPar;
  TClonesArray *fPoints;      /** Input array of PndGemPoint **/
  TClonesArray *fDigis;       /** Output array of PndGemDigi **/
  TClonesArray *fDigiMatches; /** Output array of PndGemDigiMatches**/
  Double_t fSSigma;
  Bool_t fSaveOutsideHits; /** whether to save the hits **/
  TClonesArray *fHitOutsideArray;
  Bool_t fRealisticResponse;
  Int_t fTNofEvents;
  Int_t fTNofPoints;
  Int_t fTNofDigis;
  PndGemDigiWriteoutBuffer *fDataBuffer;
  Bool_t fTimeOrderedDigi;

  Int_t fNPoints;
  Int_t fNFailed;
  Int_t fNOutside;
  Int_t fNMulti;
  Int_t fNDigis;

  TStopwatch fTimer;

  /** Map of active channels (pair detectorId, channel number)
   ** to index of PndGemDigi **/
  std::map<std::pair<Int_t, Int_t>, Int_t> fChannelMap; //!

  /** Get parameter containers **/
  virtual void SetParContainers();

  /** Intialisation **/
  virtual InitStatus Init();

  /** Reinitialisation **/
  virtual InitStatus ReInit();

  /** Reset eventwise counters **/
  void Reset();

  /** Finish at the end of each event **/
  virtual void Finish();

  /** Digitize MC points in one event **/
  void DigitizeEvent();

  /** Digitize MC points in one event **/
  void DigitizeRealisticEvent();

  /** Simulated rectangular response **/
  void SimulateRectangularResponse(Int_t sensorDetId, Int_t side, Double_t channelInd, Double_t stripWidth, Double_t showerSigma, Double_t showerStrength, Int_t iPoint);

  /** Simulated gaussian response **/
  void SimulateGaussianResponse(PndGemSensor *sensor, Int_t side, PndGemMCPoint *gemPoint, Double_t showerSigma, Double_t showerStrength, Int_t iPoint);

  /** Activate channel **/
  void ActivateChannel(Int_t sensorDetId, Int_t sensorSide, Int_t channelNumber, Double_t signalHeight, Double_t signalTime, Int_t iPoint);

  /** Print Digis **/
  void PrintDigis();

  ClassDef(PndGemDigitize, 1);
};

#endif
