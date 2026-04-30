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

#ifndef PNDMDTTRKPRODUCERIDEAL_H
#define PNDMDTTRKPRODUCERIDEAL_H 1

#include "FairTask.h"
#include "PndMdtTrk.h"

class TClonesArray;

class PndMdtTrkProducerIdeal : public FairTask {

 public:
  /** Default constructor **/
  PndMdtTrkProducerIdeal();

  /** Destructor **/
  ~PndMdtTrkProducerIdeal();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  PndMdtTrk *AddTrk(PndMdtTrk *track);

 private:
  /** Input array of PndMdtPoint **/
  TClonesArray *fPointArray;

  /** Input array of PndMdtHit **/
  TClonesArray *fHitArray;

  /** Input array of PndMCTrack **/
  TClonesArray *fMCArray;

  /** Output array of PndMdtTrk **/
  TClonesArray *fTrkArray;

  ClassDef(PndMdtTrkProducerIdeal, 1);
};

#endif
