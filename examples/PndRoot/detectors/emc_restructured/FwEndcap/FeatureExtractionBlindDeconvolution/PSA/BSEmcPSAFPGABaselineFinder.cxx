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

#include "BSEmcPSAFPGABaselineFinder.h"
#include "FairLogger.h"
#include "TFile.h"
#include "TSystem.h"
#include <memory>
#include <numeric>

BSEmcPSAFPGABaselineFinder::BSEmcPSAFPGABaselineFinder() {}

BSEmcPSAFPGABaselineFinder::~BSEmcPSAFPGABaselineFinder() {}

void BSEmcPSAFPGABaselineFinder::set(Int_t t_newBufferSize)
{
  fAverageLength = t_newBufferSize;
}

Double_t BSEmcPSAFPGABaselineFinder::put(const std::vector<Double_t> &t_samples) const
{
  const Double_t baselineValue = std::accumulate(t_samples.begin(), t_samples.begin() + fAverageLength, 0.0);
  return baselineValue / fAverageLength;
}
