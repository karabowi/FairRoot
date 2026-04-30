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

#ifndef PNDSTTPARASCIIFILEIO_H
#define PNDSTTPARASCIIFILEIO_H

#include <fstream>

#include "TObject.h"
#include "TArrayI.h"
#include "FairDetParAsciiFileIo.h"

class FairParSet;

class PndSttParAsciiFileIo : public FairDetParAsciiFileIo {
 public:
  PndSttParAsciiFileIo(std::fstream *);
  ~PndSttParAsciiFileIo() {}
  Bool_t init(FairParSet *);
  Int_t write(FairParSet *);
  template <class T>
  Bool_t read(T *, Int_t *, Bool_t needsClear = kFALSE);
  template <class T>
  Int_t writeFile2(T *);
  ClassDef(PndSttParAsciiFileIo, 0) // Class for STT parameter I/O from Ascii files
};

#endif /* !PNDSTTPARASCIIFILEIO_H */
