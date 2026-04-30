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

#include "BSEmcBaselineAdder.h"

#include <vector>

#include "TRandom.h"

#include "fairlogger/Logger.h"

BSEmcBaselineAdder::BSEmcBaselineAdder() : BSEmcAbsWaveformModifier() {}

BSEmcBaselineAdder::BSEmcBaselineAdder(Double_t t_distributionMean, Double_t t_distributionWidth)
  : BSEmcAbsWaveformModifier(), fBaselineDistributionMean(t_distributionMean), fBaselineDistributionWidth(t_distributionWidth)
{
  LOG(info) << "BSEmcBaselineAdder::BSEmcBaselineAdder - fBaselineDistributionMean " << fBaselineDistributionMean << " fBaselineDistributionWidth " << fBaselineDistributionWidth;
}

void BSEmcBaselineAdder::Modify(BSEmcWaveform *t_wf)
{
  Double_t baselineOffset = gRandom->Gaus(fBaselineDistributionMean, fBaselineDistributionWidth);
  for (double &sample : GetWaveformReference(t_wf)) {
    sample += baselineOffset;
  }
}
