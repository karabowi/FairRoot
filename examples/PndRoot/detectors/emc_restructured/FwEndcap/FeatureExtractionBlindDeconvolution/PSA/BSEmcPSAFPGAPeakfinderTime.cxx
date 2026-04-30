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

#include "BSEmcPSAFPGAPeakfinderTime.h"
#include "FairLogger.h"

BSEmcPSAFPGAPeakfinderTime::BSEmcPSAFPGAPeakfinderTime() {}

BSEmcPSAFPGAPeakfinderTime::~BSEmcPSAFPGAPeakfinderTime() {}

void BSEmcPSAFPGAPeakfinderTime::set() {}

void BSEmcPSAFPGAPeakfinderTime::put(const std::vector<Double_t> &t_samples, std::vector<BSEmcPSAFPGAPeakFeatures_t> &t_peaks) const
{

  for (BSEmcPSAFPGAPeakFeatures_t &peak : t_peaks) {
    Int_t time = peak.BinCounter;
    Double_t sample1 = t_samples[time - 1];
    Double_t sample2 = t_samples[time];
    Double_t sample3 = t_samples[time + 1];
    Double_t diff_prev = sample2 - sample1;
    Double_t diff_past = sample3 - sample2;

    Double_t new_time = time - diff_past / (diff_past - diff_prev) + 0.5; //+0.5 to correct for deriviation offset

    peak.PeakTime = (new_time);
  }
}
