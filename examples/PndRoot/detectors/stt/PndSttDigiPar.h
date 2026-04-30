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

#ifndef PNDSTTDIGIPAR_H
#define PNDSTTDIGIPAR_H

#include "TObject.h"
#include "FairParSet.h"
#include <fstream>

class PndSttDigiPar : public FairParSet {
 protected:
 public:
  PndSttDigiPar(const char *name = "SttDigiPar", const char *title = "Stt Digitisation Parameters", const char *context = "TestDefaultContext");
  ~PndSttDigiPar();
  Int_t getSize() { return 0; }
  Bool_t init(FairParIo *input);
  Int_t write(FairParIo *output);
  void clear();
  void printParam();
  void readline(const char *, Int_t *);
  void readline(const char *, Int_t *, std::fstream *);
  void putAsciiHeader(TString &);
  Bool_t writeline(char *, Int_t, Int_t);
  ClassDef(PndSttDigiPar, 1) // Container for the Stt Digitisation parameters
};

#endif /*!PNDSTTDIGIPAR_H*/
