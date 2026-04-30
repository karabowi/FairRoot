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

#ifndef PNDFTSSIGNALOVERLAP_H
#define PNDFTSSIGNALOVERLAP_H 1

#include "PndFtsHit.h"

#include <vector>

class TClonesArray;

class PndFtsSignalOverlap : public TObject {

 public:
  /** Default constructor **/
  PndFtsSignalOverlap();

  PndFtsSignalOverlap(TClonesArray *OriginalHitArray);
  PndFtsSignalOverlap(Int_t verbose, TClonesArray *OriginalHitArray);

  /** Destructor **/
  ~PndFtsSignalOverlap();

  Bool_t OverlapSimultaneousSignals(TClonesArray *OverlapHitArray);
  Bool_t WriteToOutputHit(Int_t hitid);

 private:
  TClonesArray *fOriginalHitArray;
  TClonesArray *fOverlapHitArray;
  Int_t fVerbose;
  ClassDef(PndFtsSignalOverlap, 1);
};

#endif
