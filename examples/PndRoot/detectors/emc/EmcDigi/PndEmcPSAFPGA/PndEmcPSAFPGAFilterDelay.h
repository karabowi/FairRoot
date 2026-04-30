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

#ifndef PNDEMCFPGAFILTERDELAY_HH
#define PNDEMCFPGAFILTERDELAY_HH

#include <vector>

class PndEmcPSAFPGAFilterDelay {
 public:
  PndEmcPSAFPGAFilterDelay();
  ~PndEmcPSAFPGAFilterDelay();
  void set(unsigned int newBufferSize);
  float put(float valueToStore);
  void resetToZero();

 private:
  std::vector<float> buffer;
  unsigned int bpointer;
  int numbSamples;

};

#endif
