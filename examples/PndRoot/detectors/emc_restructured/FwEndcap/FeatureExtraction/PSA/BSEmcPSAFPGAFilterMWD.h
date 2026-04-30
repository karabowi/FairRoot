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

#ifndef BSEMCPSAFPGAFILTERMWD_HH
#define BSEMCPSAFPGAFILTERMWD_HH

#include <vector>

#include "RtypesCore.h"

#include "BSEmcPSAFPGAFilterLine.h"

class BSEmcPSAFPGAFilterMWD : public BSEmcPSAFPGAFilterLine {

 public:
  BSEmcPSAFPGAFilterMWD();
  virtual ~BSEmcPSAFPGAFilterMWD();
  void set(Float_t t_tau, UInt_t t_newBufferSize);
  virtual Float_t put(Float_t t_valueToStore) /*override*/;
  void resetToZero();

 private:
  std::vector<Float_t> fBuffer{};
  UInt_t fBPointer{0};
  Float_t fSum{0};
  Float_t fLambda{0};
};

#endif /*BSEMCPSAFPGAFILTERMWD_HH*/
