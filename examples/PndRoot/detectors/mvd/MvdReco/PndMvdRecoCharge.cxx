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

#include "PndMvdRecoCharge.h"
#include "FairLogger.h"

/// Default constructor
PndMvdRecoCharge::PndMvdRecoCharge()
{
  ftr = 100.;
  fa = 60.;
  fthreshold = 3000;
  LOG(warn) << " <PndMvdRecoCharge::PndMvdRecoCharge()>: constructor with default parameters used";
};

/// Main constructor
PndMvdRecoCharge::PndMvdRecoCharge(Double_t tr, Double_t a, Double_t threshold)
{
  ftr = tr;
  fa = a;
  fthreshold = threshold;
};

Double_t PndMvdRecoCharge::GetCharge(PndSdsDigi &digi)
{
  if (digi.GetTot() < 0) {
    LOG(error) << " <PndMvdRecoCharge::GetCharge(PndSdsDigi &digi)>: time over threshold not calculated in digitization step";
    return -1;
  }
  return (-fa * ftr + fthreshold + digi.GetTot() * fa) / 2. + sqrt(pow((fa * ftr - fthreshold - digi.GetTot() * fa), 2) / 4. + fa * fthreshold * ftr);
}

Double_t PndMvdRecoCharge::GetCharge(Double_t tot)
{
  if (tot < 0) {
    LOG(error) << " <PndMvdRecoCharge::GetCharge(Double_t tot)>: time over threshold not calculated in digitization step";
    return -1;
  }
  return (-fa * ftr + fthreshold + tot * fa) / 2. + sqrt(pow((fa * ftr - fthreshold - tot * fa), 2) / 4. + fa * fthreshold * ftr);
}
