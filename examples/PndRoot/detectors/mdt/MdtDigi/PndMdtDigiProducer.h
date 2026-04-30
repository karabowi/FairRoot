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

#ifndef PNDMDTDIGIPRODUCER_H
#define PNDMDTDIGIPRODUCER_H 1

#include "FairTask.h"
#include "PndMdtDigi.h"
#include "TVector3.h"
#include <vector>

class TClonesArray;

class PndMdtDigiProducer : public FairTask {

 public:
  /** Default constructor **/
  PndMdtDigiProducer();

  /** Destructor **/
  ~PndMdtDigiProducer();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  PndMdtDigi *AddDigiBox(Int_t detID, TVector3 &pos, std::vector<Int_t> pointList);
  PndMdtDigi *AddDigiStrip(Int_t detID, TVector3 &pos, std::vector<Int_t> pointList);

  void SetStripMode(Bool_t mode = kTRUE) { fStripMode = mode; };

 private:
  Bool_t fStripMode;    //! Strip Mode
  Float_t fBarrelStart; //!
  Float_t fEndcapStart; //!
  Float_t fMFStart;     //!

  /** Input array of PndMdtPoint **/
  TClonesArray *fPointArray;

  /** Output array of PndMdtDigi **/
  TClonesArray *fDigiBoxArray;
  TClonesArray *fDigiStripArray;

  ClassDef(PndMdtDigiProducer, 1);
};

#endif
