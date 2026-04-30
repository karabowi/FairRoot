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

/////////////////////////////////////////////////////////////
// PndEmcFpgaPar
//
// Container class for Digitisation parameters
// class is inherited from FairParGenericSet
//
/////////////////////////////////////////////////////////////

#include "PndEmcFpgaPar.h"
#include <iostream>

ClassImp(PndEmcFpgaPar)

  PndEmcFpgaPar::PndEmcFpgaPar(const char *name, const char *title, const char *context)
  : FairParGenericSet(name, title, context), fMAFilterLengthBarrel(0), fCFFilterLengthBarrel(0), fCFFilterRatioBarrel(0), fCFDelayLengthBarrel(0), fCFFitterLengthBarrel(0),
    fMWDFilterUsedBarrel(0), fMWDFilterLengthBarrel(0), fMWDDecayConstantBarrel(0), fPulseThresholdBarrel(0), fMAFilterLengthForward(0), fCFFilterLengthForward(0),
    fCFFilterRatioForward(0), fCFDelayLengthForward(0), fCFFitterLengthForward(0), fMWDFilterUsedForward(0), fMWDFilterLengthForward(0), fMWDDecayConstantForward(0),
    fPulseThresholdForward(0), fMAFilterLengthShashylik(0), fCFFilterLengthShashylik(0), fCFFilterRatioShashylik(0), fCFDelayLengthShashylik(0), fCFFitterLengthShashylik(0),
    fMWDFilterUsedShashylik(0), fMWDFilterLengthShashylik(0), fMWDDecayConstantShashylik(0), fPulseThresholdShashylik(0)

{
  clear(); // WHY?
}

void PndEmcFpgaPar::putParams(FairParamList *list)
{
  if (!list)
    return;
  list->add("BarrelMAFilterLength", fMAFilterLengthBarrel);
  list->add("BarrelCFFilterLength", fCFFilterLengthBarrel);
  list->add("BarrelCFFilterRatio", fCFFilterRatioBarrel);
  list->add("BarrelCFDelayLength", fCFDelayLengthBarrel);
  list->add("BarrelCFFitterLength", fCFFitterLengthBarrel);
  list->add("BarrelMWDFilterUsed", fMWDFilterUsedBarrel);
  list->add("BarrelMWDFilterLength", fMWDFilterLengthBarrel);
  list->add("BarrelMWDFilterLifeT", fMWDDecayConstantBarrel);
  list->add("BarrelPulseThreshold", fPulseThresholdBarrel);

  list->add("ForwardMAFilterLength", fMAFilterLengthForward);
  list->add("ForwardCFFilterLength", fCFFilterLengthForward);
  list->add("ForwardCFFilterRatio", fCFFilterRatioForward);
  list->add("ForwardCFDelayLength", fCFDelayLengthForward);
  list->add("ForwardCFFitterLength", fCFFitterLengthForward);
  list->add("ForwardMWDFilterUsed", fMWDFilterUsedForward);
  list->add("ForwardMWDFilterLength", fMWDFilterLengthForward);
  list->add("ForwardMWDFilterLifeT", fMWDDecayConstantForward);
  list->add("ForwardPulseThreshold", fPulseThresholdForward);

  list->add("ShashylikMAFilterLength", fMAFilterLengthShashylik);
  list->add("ShashylikCFFilterLength", fCFFilterLengthShashylik);
  list->add("ShashylikCFFilterRatio", fCFFilterRatioShashylik);
  list->add("ShashylikCFFitterLength", fCFFitterLengthShashylik);
  list->add("ShashylikCFDelayLength", fCFDelayLengthShashylik);
  list->add("ShashylikMWDFilterUsed", fMWDFilterUsedShashylik);
  list->add("ShashylikMWDFilterLength", fMWDFilterLengthShashylik);
  list->add("ShashylikMWDFilterLifeT", fMWDDecayConstantShashylik);
  list->add("ShashylikPulseThreshold", fPulseThresholdShashylik);
}

Bool_t PndEmcFpgaPar::getParams(FairParamList *list)
{
  // std::cout<<"fill paramters to PndEmcFpgaPar"<<std::endl;
  if (!list)
    return kFALSE;
  if (!list->fill("BarrelMAFilterLength", &fMAFilterLengthBarrel))
    return kFALSE;
  if (!list->fill("BarrelCFFilterLength", &fCFFilterLengthBarrel))
    return kFALSE;
  if (!list->fill("BarrelCFFilterRatio", &fCFFilterRatioBarrel))
    return kFALSE;
  if (!list->fill("BarrelCFFitterLength", &fCFFitterLengthBarrel))
    return kFALSE;
  if (!list->fill("BarrelCFDelayLength", &fCFDelayLengthBarrel))
    return kFALSE;
  if (!list->fill("BarrelMWDFilterUsed", &fMWDFilterUsedBarrel))
    return kFALSE;
  if (!list->fill("BarrelMWDFilterLength", &fMWDFilterLengthBarrel))
    return kFALSE;
  if (!list->fill("BarrelMWDFilterLifeT", &fMWDDecayConstantBarrel))
    return kFALSE;
  if (!list->fill("BarrelPulseThreshold", &fPulseThresholdBarrel))
    return kFALSE;

  if (!list->fill("ForwardMAFilterLength", &fMAFilterLengthForward))
    return kFALSE;
  if (!list->fill("ForwardCFFilterLength", &fCFFilterLengthForward))
    return kFALSE;
  if (!list->fill("ForwardCFFilterRatio", &fCFFilterRatioForward))
    return kFALSE;
  if (!list->fill("ForwardCFFitterLength", &fCFFitterLengthForward))
    return kFALSE;
  if (!list->fill("ForwardCFDelayLength", &fCFDelayLengthForward))
    return kFALSE;
  if (!list->fill("ForwardMWDFilterUsed", &fMWDFilterUsedForward))
    return kFALSE;
  if (!list->fill("ForwardMWDFilterLength", &fMWDFilterLengthForward))
    return kFALSE;
  if (!list->fill("ForwardMWDFilterLifeT", &fMWDDecayConstantForward))
    return kFALSE;
  if (!list->fill("ForwardPulseThreshold", &fPulseThresholdForward))
    return kFALSE;

  if (!list->fill("ShashylikMAFilterLength", &fMAFilterLengthShashylik))
    return kFALSE;
  if (!list->fill("ShashylikCFFilterLength", &fCFFilterLengthShashylik))
    return kFALSE;
  if (!list->fill("ShashylikCFFilterRatio", &fCFFilterRatioShashylik))
    return kFALSE;
  if (!list->fill("ShashylikCFFitterLength", &fCFFitterLengthShashylik))
    return kFALSE;
  if (!list->fill("ShashylikCFDelayLength", &fCFDelayLengthShashylik))
    return kFALSE;
  if (!list->fill("ShashylikMWDFilterUsed", &fMWDFilterUsedShashylik))
    return kFALSE;
  if (!list->fill("ShashylikMWDFilterLength", &fMWDFilterLengthShashylik))
    return kFALSE;
  if (!list->fill("ShashylikMWDFilterLifeT", &fMWDDecayConstantShashylik))
    return kFALSE;
  if (!list->fill("ShashylikPulseThreshold", &fPulseThresholdShashylik))
    return kFALSE;

  return kTRUE;
}
