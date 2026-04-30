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

#ifndef BSEMCPSAFPGAPILEUPDETECTION_HH
#define BSEMCPSAFPGAPILEUPDETECTION_HH

#include "RtypesCore.h"
#include <vector>
#include "BSEmcPSAFPGAPeakFeatures_t.h"

class BSEmcPSAFPGAPileUpDetection {
 public:
  BSEmcPSAFPGAPileUpDetection();
  ~BSEmcPSAFPGAPileUpDetection();
  void set(Double_t t_yscale, Double_t t_xoffset, Double_t t_yoffset);
  void put(std::vector<BSEmcPSAFPGAPeakFeatures_t> &t_peaks) const;

 private:
  Double_t fYscale = 0.;
  Double_t fXoffset = 0.;
  Double_t fYoffset = 0.;

};

#endif /*BSEMCPSAFPGAPILEUPDETECTION_HH*/
