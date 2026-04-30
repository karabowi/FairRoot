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

#ifndef BSEMCPSAFPGABASELINEFINDER_HH
#define BSEMCPSAFPGABASELINEFINDER_HH

#include "RtypesCore.h"
#include <vector>

class BSEmcPSAFPGABaselineFinder {
 public:
  BSEmcPSAFPGABaselineFinder();
  ~BSEmcPSAFPGABaselineFinder();
  void set(Int_t t_newBufferSize);
  Double_t put(const std::vector<Double_t> &t_samples) const;

 private:
  Int_t fAverageLength = 0;
};

#endif /*BSEMCPSAFPGABASELINEFINDER_HH*/
