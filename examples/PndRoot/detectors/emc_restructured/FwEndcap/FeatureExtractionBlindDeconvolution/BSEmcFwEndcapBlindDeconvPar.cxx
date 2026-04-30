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

#include "BSEmcFwEndcapBlindDeconvPar.h"

#include "FairParamList.h"

ClassImp(BSEmcFwEndcapBlindDeconvPar)

  const std::string BSEmcFwEndcapBlindDeconvPar::fgParameterName = "EmcFwEndcapBlindDeconvPar";

BSEmcFwEndcapBlindDeconvPar::BSEmcFwEndcapBlindDeconvPar(const char *t_name, const char *t_title, const char *t_context) : FairParGenericSet(t_name, t_title, t_context)
{
  clear();
}

void BSEmcFwEndcapBlindDeconvPar::putParams(FairParamList *t_list)
{
  if (t_list == nullptr) {
    return;
  }

  t_list->add("DeconvolutionFilename", fDeconvolutionFilename);

  t_list->add("PeakFinderThreshold", fPeakFinderThreshold);
  t_list->add("PeakFinderLowerLimit", fPeakFinderLowerLimit);
  t_list->add("PeakFinderUpperLimit", fPeakFinderUpperLimit);

  t_list->add("PileUpDetectionYScale", fPileUpDetectionYScale);
  t_list->add("PileUpDetectionXOffset", fPileUpDetectionXOffset);
  t_list->add("PileUpDetectionYOffset", fPileUpDetectionYOffset);
  t_list->add("BaselineSampleNumber", fBaselineSampleNumber);
}

Bool_t BSEmcFwEndcapBlindDeconvPar::getParams(FairParamList *t_list)
{

  if (t_list == nullptr) {
    return kFALSE;
  }
  if (!t_list->fill("DeconvolutionFilename", fDeconvolutionFilename, 100)) {
    return kFALSE;
  }
  if (!t_list->fill("PeakFinderThreshold", &fPeakFinderThreshold)) {
    return kFALSE;
  }
  if (!t_list->fill("PeakFinderLowerLimit", &fPeakFinderLowerLimit)) {
    return kFALSE;
  }
  if (!t_list->fill("PeakFinderUpperLimit", &fPeakFinderUpperLimit)) {
    return kFALSE;
  }

  if (!t_list->fill("PileUpDetectionYScale", &fPileUpDetectionYScale)) {
    return kFALSE;
  }
  if (!t_list->fill("PileUpDetectionXOffset", &fPileUpDetectionXOffset)) {
    return kFALSE;
  }
  if (!t_list->fill("PileUpDetectionYOffset", &fPileUpDetectionYOffset)) {
    return kFALSE;
  }
  if (!t_list->fill("BaselineSampleNumber", &fBaselineSampleNumber)) {
    return kFALSE;
  }
  return kTRUE;
}
