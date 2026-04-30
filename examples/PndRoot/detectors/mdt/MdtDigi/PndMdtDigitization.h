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

#ifndef PNDMDTDIGITIZATION_H
#define PNDMDTDIGITIZATION_H 1

#include <PndPersistencyTask.h>
#include "TVector3.h"

class TClonesArray;
class PndMdtIGeometry;
class PndMdtDigi;
class PndMdtWaveform;

class PndMdtDigitization : public PndPersistencyTask {

 public:
  /** Default constructor **/
  PndMdtDigitization(Int_t verbose = 0, Bool_t store = kTRUE);
  /** Destructor **/
  ~PndMdtDigitization();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  void SetStorageOfData(Bool_t) { ; } // val //[R.K.03/2017] unused variable(s)
  void FinishTask();

  void RunTimeBased() { fTimeOrderedDigi = kTRUE; }

 private:
  PndMdtDigi *AddDigiBox(Int_t detID, TVector3 &pos, Int_t);
  PndMdtDigi *AddDigiStrip(Int_t detID, TVector3 &pos, Int_t);
  PndMdtDigi *AddDigi(Int_t detID, TVector3 &pos, Int_t evtNo);
  Bool_t Digitize(PndMdtWaveform *theWf, Double_t &time, Double_t &amp, Bool_t isWire);
  void exec_t();
  void exec_e();

  Double_t fWireNoiseSigma;
  Double_t fStripNoiseSigma;
  Double_t fSamplingInterval;

  PndMdtIGeometry *fGeoIF;

  /** Input array of PndMdtPoint **/
  TClonesArray *fWaveformArray;

  /** Output array of PndMdtDigi **/
  TClonesArray *fDigiBoxArray;
  TClonesArray *fDigiStripArray;
  TClonesArray *fDigiArray;

  Long_t HowManyWireWf;
  Long_t HowManyWireDigi;
  Long_t HowManyStripWf;
  Long_t HowManyStripDigi;

  Bool_t fTimeOrderedDigi;

  ClassDef(PndMdtDigitization, 1);
};

#endif
