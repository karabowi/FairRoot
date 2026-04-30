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

#ifndef BSEMCPSAFPGAFILTERMA_HH
#define BSEMCPSAFPGAFILTERMA_HH

#include <vector>

#include "RtypesCore.h"

class BSEmcPSAFPGAFilterMA {
 public:
  BSEmcPSAFPGAFilterMA();
  ~BSEmcPSAFPGAFilterMA();
  void set(UInt_t t_newBufferSize);
  void resetToZero();
  Float_t put(Float_t t_valueToStore);

 private:
  std::vector<Float_t> fBuffer{};
  UInt_t fBPointer{0};
  Float_t fSum{0};
};

#endif /*BSEMCPSAFPGAFILTERMA_HH*/
