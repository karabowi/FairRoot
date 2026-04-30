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

#ifndef PNDSTTPARROOTFILEIO_H
#define PNDSTTPARROOTFILEIO_H

#include "FairDetParRootFileIo.h"
#include "TFile.h"
#include "TArrayI.h"

class FairParRootFile;
class FairParSet;
class PndSttDigiPar;

class PndSttParRootFileIo : public FairDetParRootFileIo {
 public:
  PndSttParRootFileIo(FairParRootFile *f);
  ~PndSttParRootFileIo();
  Bool_t init(FairParSet *, Int_t *);
  Bool_t read(PndSttDigiPar *, Int_t *);
  ClassDef(PndSttParRootFileIo, 0) // Class for STT parameter I/O from ROOT file
};

#endif /* !PNDSTTPARROOTFILEIO_H */
