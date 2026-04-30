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

#include "PndMQMvdChargeWeightedPixelMapping.h"
#include "PndSdsIdealChargeConversion.h"
#include "PndSdsTotChargeConversion.h"
#include "FairLogger.h"

PndMQMvdChargeWeightedPixelMapping::PndMQMvdChargeWeightedPixelMapping(Int_t verbose) : PndMQSdsChargeWeightedPixelMapping()
{
  fVerbose = verbose;
  FairRun *ana = FairRun::Instance();
  FairRuntimeDb *rtdb = ana->GetRuntimeDb();
  fDigiPar = (PndSdsPixelDigiPar *)(rtdb->getContainer("MVDPixelDigiPar"));
  fTotDigiPar = (PndSdsTotDigiPar *)(rtdb->getContainer("MVDPixelTotDigiPar"));

  SetChargeConverter();
}

PndMQMvdChargeWeightedPixelMapping::PndMQMvdChargeWeightedPixelMapping(PndGeoHandling *geo, Int_t verbose) : PndMQSdsChargeWeightedPixelMapping(geo)
{
  fVerbose = verbose;
  FairRun *ana = FairRun::Instance();
  FairRuntimeDb *rtdb = ana->GetRuntimeDb();
  fDigiPar = (PndSdsPixelDigiPar *)(rtdb->getContainer("MVDPixelDigiPar"));
  fTotDigiPar = (PndSdsTotDigiPar *)(rtdb->getContainer("MVDPixelTotDigiPar"));

  SetChargeConverter();
}

PndMQMvdChargeWeightedPixelMapping::PndMQMvdChargeWeightedPixelMapping(PndGeoHandling *geo, PndSdsPixelDigiPar *digiPar, PndSdsTotDigiPar *totPar)
  : PndMQSdsChargeWeightedPixelMapping(geo), fDigiPar(digiPar), fTotDigiPar(totPar)
{
  SetChargeConverter();
}

void PndMQMvdChargeWeightedPixelMapping::SetChargeConverter()
{
  LOG(info) << "ChargeConversionMethod: " << fDigiPar->GetChargeConvMethod() << " " << fDigiPar->GetThreshold();
  if (fDigiPar->GetChargeConvMethod() == 0) {
    if (fVerbose > 0)
      Info("PndMQMvdChargeWeightedPixelMapping", "ideal charge conversion");
    fChargeConverter = new PndSdsIdealChargeConversion(fDigiPar->GetNoise());
  } else if (fDigiPar->GetChargeConvMethod() == 1) {
    if (fVerbose > 0)
      Info("PndMQMvdChargeWeightedPixelMapping", "use TOT charge conversion");
    fChargeConverter =
      new PndSdsTotChargeConversion(fTotDigiPar->GetChargingTime(), fTotDigiPar->GetConstCurrent(), fDigiPar->GetThreshold(), fTotDigiPar->GetClockFrequency(), fVerbose);
  } else {
    Fatal("PndMQMvdChargeWeightedPixelMapping", "conversion method not defined!");
  }
  flx = fDigiPar->GetXPitch();
  fly = fDigiPar->GetYPitch();
  fcols = fDigiPar->GetFECols();
  frows = fDigiPar->GetFERows();
  LOG(info) << "ChargeConversionMethod: " << fDigiPar->GetChargeConvMethod() << " " << fChargeConverter;
}
