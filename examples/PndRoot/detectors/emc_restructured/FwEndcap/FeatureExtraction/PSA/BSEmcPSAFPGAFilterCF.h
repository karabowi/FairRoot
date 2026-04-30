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

#ifndef BSEMCFPGAFILTERCF_HH
#define BSEMCFPGAFILTERCF_HH

#include <vector>

#include "RtypesCore.h"

#include "BSEmcPSAFPGAFilterLine.h"

class BSEmcPSAFPGAFilterCF : public BSEmcPSAFPGAFilterLine {

 public:
  BSEmcPSAFPGAFilterCF();
  virtual ~BSEmcPSAFPGAFilterCF();
  void set(UInt_t t_newBufferSize, Float_t t_newRatio);
  void resetToZero();
  virtual Float_t put(Float_t t_valueToStore) /*override*/;

 private:
  std::vector<Float_t> fBuffer{};
  UInt_t fBPointer{0};
  Float_t fRatio{0};
};

#endif /*BSEMCFPGAFILTERCF_HH*/
