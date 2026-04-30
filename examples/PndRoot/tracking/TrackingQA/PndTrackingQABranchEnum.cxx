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
 * PndTrackingQABranchEnum.cxx
 *
 *  Created on: 06.07.2023
 *      Author: tstockmanns
 */

#include "PndTrackingQABranchEnum.h"

TrackingQA::branchEnum TrackingQA::StringToBranchEnum(TString branchString)
{
  if (branchString == "MVDHitsPixel")
    return TrackingQA::branchEnum::MVDHitsPixel;
  if (branchString == "MVDHitsStrip")
    return TrackingQA::branchEnum::MVDHitsStrip;
  if (branchString == "GEMHit")
    return TrackingQA::branchEnum::GEMHit;
  if (branchString == "FTSHit")
    return TrackingQA::branchEnum::FTSHit;
  if (branchString == "STTHit")
    return TrackingQA::branchEnum::STTHit;
  if (branchString == "STTHitParal")
    return TrackingQA::branchEnum::STTHitParal;
  if (branchString == "STTHitSkew")
    return TrackingQA::branchEnum::STTHitSkew;
  return branchEnum::Undefined;
}

TString TrackingQA::BranchEnumToString(TrackingQA::branchEnum branch)
{
  switch (branch) {
  case TrackingQA::branchEnum::MVDHitsPixel: return "MVDHitsPixel"; break;
  case TrackingQA::branchEnum::MVDHitsStrip: return "MVDHitsStrip"; break;
  case TrackingQA::branchEnum::GEMHit: return "GEMHit"; break;
  case TrackingQA::branchEnum::FTSHit: return "FTSHit"; break;
  case TrackingQA::branchEnum::STTHit: return "STTHit"; break;
  case TrackingQA::branchEnum::STTHitParal: return "STTHitParal"; break;
  case TrackingQA::branchEnum::STTHitSkew: return "STTHitSkew"; break;
  default: return "Undefined";
  }
  return "Undefined";
}
