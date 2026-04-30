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
// -----                 PndSttFullTCAProducer header file             -----
// -------------------------------------------------------------------------

#ifndef PNDSTTFULLTCAPRODUCER_H
#define PNDSTTFULLTCAPRODUCER_H 1
#include "FairTask.h"

class TClonesArray;

class PndSttFullTCAProducer : public FairTask {
 public:
  /** Default constructor **/
  PndSttFullTCAProducer();

  /** Destructor **/
  ~PndSttFullTCAProducer();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

 private:
  TClonesArray *fSttHitArray1;
  TClonesArray *fSttHitArray2;
  TString fBranchName1, fBranchName2;

  /** Output array of PndSttHits **/
  TClonesArray *fSttHitArray0;

  ClassDef(PndSttFullTCAProducer, 1);
};

#endif
