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

#ifndef PNDMVDEVENTMERGER_H
#define PNDMVDEVENTMERGER_H

#include "TString.h"
#include "TObject.h"
#include "TObjArray.h"
#include "TClonesArray.h"

#include <vector>

class PndMvdEventMerger {
 public:
  PndMvdEventMerger();
  PndMvdEventMerger(TString fileName, TString branchName, Int_t nEvents, Int_t nMerged);
  virtual ~PndMvdEventMerger();

  TClonesArray *GetEvent(Int_t iEvent);
  Int_t AddTClonesArray(TClonesArray *target, TClonesArray *source);

 private:
  std::vector<TClonesArray *> fBuffer;
  Int_t FillBuffer(TString fileName, TString branchName, Int_t nEvents, Int_t nMerged);

  ClassDef(PndMvdEventMerger, 1);
};

#endif /*PNDMVDEVENTMERGER_H*/
