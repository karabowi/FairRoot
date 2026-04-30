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

#ifndef BSEMCPSAFPGAPEAKFINDERTIME_HH
#define BSEMCPSAFPGAPEAKFINDERTIME_HH

#include "RtypesCore.h"
#include <vector>
#include "BSEmcPSAFPGAPeakFeatures_t.h"

class BSEmcPSAFPGAPeakfinderTime {
 public:
  BSEmcPSAFPGAPeakfinderTime();
  ~BSEmcPSAFPGAPeakfinderTime();
  void set();
  void put(const std::vector<Double_t> &t_samples, std::vector<BSEmcPSAFPGAPeakFeatures_t> &t_peaks) const;

 private:
};

#endif /*BSEMCPSAFPGAPEAKFINDERTIME_HH*/
