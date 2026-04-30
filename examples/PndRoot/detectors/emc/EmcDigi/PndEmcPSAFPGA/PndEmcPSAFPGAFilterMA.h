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

#ifndef PNDEMCFPGAFILTERMA_HH
#define PNDEMCFPGAFILTERMA_HH
#include <vector>
#include <iostream>
class PndEmcPSAFPGAFilterMA
{
 public:
  PndEmcPSAFPGAFilterMA();
  ~PndEmcPSAFPGAFilterMA();
  void set(unsigned int newBufferSize);
  void resetToZero();
  float put(float valueToStore);
 private:
  std::vector<float> buffer;
  unsigned int bpointer;
  float sum;
};

#endif
