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

#include "BSEmcPSAFPGAPeakfinder.h"
#include "FairLogger.h"

#include <numeric>

BSEmcPSAFPGAPeakfinder::BSEmcPSAFPGAPeakfinder() {}

BSEmcPSAFPGAPeakfinder::~BSEmcPSAFPGAPeakfinder() {}

void BSEmcPSAFPGAPeakfinder::set(UInt_t t_threshold, UInt_t t_llimit, UInt_t t_ulimit)
{
  fThreshold = t_threshold;
  fLLimit = t_llimit;
  fULimit = t_ulimit;
  // LOG(info) << "Threshold = " << fThreshold;
}

void BSEmcPSAFPGAPeakfinder::resetToZero() {}

std::vector<BSEmcPSAFPGAPeakFeatures_t> BSEmcPSAFPGAPeakfinder::put(const std::vector<Double_t> &t_samples) const
{

  Int_t counter = 0;
  Int_t binCounter = 0;
  Int_t maxCounter = 0;
  Int_t max = 0;
  Int_t integral = 0;
  std::vector<BSEmcPSAFPGAPeakFeatures_t> result{};

  Int_t bufferSize = t_samples.size();
  for (Double_t sample : t_samples) {
    if (max < std::round(sample)) {
      max = std::round(sample);
      ++counter;
    } else if (max == std::round(sample)) {
      // LOG(info) << "equal -> do nothing";
    }

    else {
      if ((counter > fThreshold) && (bufferSize - binCounter > 22)) {
        integral = std::accumulate(t_samples.begin() + binCounter - fLLimit - 1, t_samples.begin() + binCounter + fULimit - 1, 0.0);
        BSEmcPSAFPGAPeakFeatures_t foundMax{};
        foundMax.MaximumBin = (maxCounter);
        foundMax.BinCounter = (binCounter - 1);
        foundMax.MaximumValue = (max);
        foundMax.Integral = (integral);

        result.push_back(foundMax);
        ++maxCounter;
        counter = 0;
        max = 0;
        integral = 0;
      } else {
        counter = 0;
        max = 0;
      }
    }
    ++binCounter;
  }

  return result;
}
