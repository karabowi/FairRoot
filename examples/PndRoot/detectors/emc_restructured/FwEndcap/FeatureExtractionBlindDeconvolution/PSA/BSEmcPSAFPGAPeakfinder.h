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

#ifndef BSEMCPSAFPGAPEAKFINDER_HH
#define BSEMCPSAFPGAPEAKFINDER_HH

#include "RtypesCore.h"

#include <vector>

#include "BSEmcPSAFPGAPeakFeatures_t.h"

class BSEmcPSAFPGAPeakfinder {
 public:
  BSEmcPSAFPGAPeakfinder();
  ~BSEmcPSAFPGAPeakfinder();
  void set(UInt_t t_threshold, UInt_t t_llimit, UInt_t t_ulimit);
  void resetToZero();
  std::vector<BSEmcPSAFPGAPeakFeatures_t> put(const std::vector<Double_t> &tBuffer) const;

 private:
  Int_t fThreshold = 0;
  Int_t fLLimit = 0;
  Int_t fULimit = 0;
};

#endif /*BSEMCPSAFPGAPEAKFINDER_HH*/
