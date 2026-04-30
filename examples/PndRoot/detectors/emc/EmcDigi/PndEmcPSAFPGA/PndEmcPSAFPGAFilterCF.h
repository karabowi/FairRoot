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

#ifndef PNDEMCFPGAFILTERCF_HH
#define PNDEMCFPGAFILTERCF_HH

#include "PndEmcPSAFPGAFilterLine.h"
#include <vector>

class PndEmcPSAFPGAFilterCF : public PndEmcPSAFPGAFilterLine {

 public:
  PndEmcPSAFPGAFilterCF();
  virtual ~PndEmcPSAFPGAFilterCF();
  void set(unsigned int newBufferSize, float newRatio);
  void resetToZero();
  virtual float put(float valueToStore);

 private:
  std::vector<float> buffer;
  unsigned int bpointer;
  float ratio;
};

#endif
