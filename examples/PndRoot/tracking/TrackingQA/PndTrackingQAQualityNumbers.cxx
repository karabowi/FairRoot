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

/*
 * PndTrackingQAQualityNumbers.cxx
 *
 *  Created on: 22.06.2023
 *      Author: tstockmanns
 */

#include "PndTrackingQAQualityNumbers.h"

int TrackingQA::GetRecoQuality(PndTrackingQARecoInfo *recoInfo, double purityThreshold, double efficiencyThreshold)
{
  int result;
  bool pure = false;
  bool complete = false;
  if (!RecoTrackFound(recoInfo, purityThreshold, efficiencyThreshold)) {
    return -1;
  }
  if (recoInfo->GetPurity() > 0.99) {
    pure = true;
  }
  if (recoInfo->GetEfficiency() > 0.99) {
    complete = true;
  }
  if (pure) {
    if (complete) {
      result = TrackingQA::qualityNumbers::kFullyPure;
    } else {
      result = TrackingQA::qualityNumbers::kPartiallyPure;
    }
  } else {
    if (complete) {
      result = TrackingQA::qualityNumbers::kFullyImpure;
    } else {
      result = TrackingQA::qualityNumbers::kPartiallyImpure;
    }
  }
  return result;
}

bool TrackingQA::RecoTrackFound(PndTrackingQARecoInfo *recoInfo, double purityThreshold, double efficiencyThreshold)
{
  return (recoInfo->GetPurity() > purityThreshold && recoInfo->GetEfficiency() > efficiencyThreshold);
}
