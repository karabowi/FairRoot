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

#include "BSEmcPSAFPGAPileUpDetection.h"
#include "FairLogger.h"

BSEmcPSAFPGAPileUpDetection::BSEmcPSAFPGAPileUpDetection() {}

BSEmcPSAFPGAPileUpDetection::~BSEmcPSAFPGAPileUpDetection() {}

void BSEmcPSAFPGAPileUpDetection::set(Double_t t_yscale, Double_t t_xoffset, Double_t t_yoffset)
{
  fYscale = t_yscale;
  fXoffset = t_xoffset;
  fYoffset = t_yoffset;
}

void BSEmcPSAFPGAPileUpDetection::put(std::vector<BSEmcPSAFPGAPeakFeatures_t> &t_peaks) const
{
  for (BSEmcPSAFPGAPeakFeatures_t &peak : t_peaks) {
    Double_t ratio = double(peak.Integral) / double(peak.MaximumValue);
    Double_t cut = fYscale / (double(peak.MaximumValue) - fXoffset) + fYoffset;
    if (ratio <= cut) {
      peak.PileUp = false;
    } else {
      peak.PileUp = true;
    }
  }
}
