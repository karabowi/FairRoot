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

#include "BSEmcShashlikIdealDigitizationProcess.h"

#include <math.h>

#include "TRandom.h"

// BSEmcShashlikIdealDigitizationProcess
// ------------------------------------------------
BSEmcShashlikIdealDigitizationProcess::BSEmcShashlikIdealDigitizationProcess() : BSEmcIdealDigitizationProcess() {}

// ~BSEmcShashlikIdealDigitizationProcess
// -----------------------------------------------
BSEmcShashlikIdealDigitizationProcess::~BSEmcShashlikIdealDigitizationProcess() {}

// GausSmearing ----------------------------------------------------------------
Double_t BSEmcShashlikIdealDigitizationProcess::GausSmearing(Double_t t_energy) const
{
  // sampling factor was missing in PndEmcMakeDigi, causing missing energy by a factor or ~3.
  // This factor is now applied as calibration value on the calibrated digi energy for Shashlik digis.
  // additionally, the last term of the sigma_E was also squared, as the rest
  // leading to a substantially smaller sigma width of the smearing.
  // sigma/E=5.6/E+2.4/sqrt(E)+1.3 (%)
  t_energy *= 1000; // Energy must be in MeV!!! Otherwise, crystal energies of up to 56 MeV will be smeared
  // with gaussians of the same width, which is not in agreement with the TDR, stating that one once 1 MeV
  // noise and single crystal thresholds of 3 MeV.
  const Double_t sigma_E = sqrt(pow(0.056 / t_energy, 2) + pow(0.024 / sqrt(t_energy), 2) + pow(0.013, 2));
  const Double_t result = gRandom->Gaus(t_energy, sigma_E * t_energy) / 1000;

  return result;
}
