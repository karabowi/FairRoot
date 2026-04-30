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

#include "PndLmdChargeWeightedPixelMapping.h"
#include "PndSdsIdealChargeConversion.h"
#include "PndSdsPixelDigiPar.h"
#include "PndSdsTotChargeConversion.h"
#include "PndSdsTotDigiPar.h"

PndLmdChargeWeightedPixelMapping::PndLmdChargeWeightedPixelMapping(Int_t verbose) : PndSdsChargeWeightedPixelMapping()
{
  fVerbose = verbose;
  FairRun *ana = FairRun::Instance();
  FairRuntimeDb *rtdb = ana->GetRuntimeDb();
  PndSdsPixelDigiPar *fDigiPar = (PndSdsPixelDigiPar *)(rtdb->getContainer("LMDPixelDigiPar"));
  PndSdsTotDigiPar *fTotDigiPar = (PndSdsTotDigiPar *)(rtdb->getContainer("LMDPixelTotDigiPar"));
  if (fDigiPar->GetChargeConvMethod() == 0) {
    if (fVerbose > 0)
      Info("PndLmdChargeWeightedPixelMapping", "ideal charge conversion");
    fChargeConverter = new PndSdsIdealChargeConversion(fDigiPar->GetNoise());
  } else if (fDigiPar->GetChargeConvMethod() == 1) {
    if (fVerbose > 0)
      Info("PndLmdChargeWeightedPixelMapping", "use TOT charge conversion");
    fChargeConverter =
      new PndSdsTotChargeConversion(fTotDigiPar->GetChargingTime(), fTotDigiPar->GetConstCurrent(), fDigiPar->GetThreshold(), fTotDigiPar->GetClockFrequency(), fVerbose);
  } else {
    Fatal("PndLmdChargeWeightedPixelMapping", "conversion method not defined!");
  }

  flx = fDigiPar->GetXPitch();
  fly = fDigiPar->GetYPitch();
  fcols = fDigiPar->GetFECols();
  frows = fDigiPar->GetFERows();
}

PndLmdChargeWeightedPixelMapping::PndLmdChargeWeightedPixelMapping(PndGeoHandling *geo, Int_t verbose) : PndSdsChargeWeightedPixelMapping(geo)
{
  fVerbose = verbose;
  FairRun *ana = FairRun::Instance();
  FairRuntimeDb *rtdb = ana->GetRuntimeDb();
  PndSdsPixelDigiPar *fDigiPar = (PndSdsPixelDigiPar *)(rtdb->getContainer("LMDPixelDigiPar"));
  PndSdsTotDigiPar *fTotDigiPar = (PndSdsTotDigiPar *)(rtdb->getContainer("LMDPixelTotDigiPar"));
  if (fDigiPar->GetChargeConvMethod() == 0) {
    if (fVerbose > 0)
      Info("PndLmdChargeWeightedPixelMapping", "ideal charge conversion");
    fChargeConverter = new PndSdsIdealChargeConversion(fDigiPar->GetNoise());
  } else if (fDigiPar->GetChargeConvMethod() == 1) {
    if (fVerbose > 0)
      Info("PndLmdChargeWeightedPixelMapping", "use TOT charge conversion");
    fChargeConverter =
      new PndSdsTotChargeConversion(fTotDigiPar->GetChargingTime(), fTotDigiPar->GetConstCurrent(), fDigiPar->GetThreshold(), fTotDigiPar->GetClockFrequency(), fVerbose);
  } else {
    Fatal("PndLmdChargeWeightedPixelMapping", "conversion method not defined!");
  }
  flx = fDigiPar->GetXPitch();
  fly = fDigiPar->GetYPitch();
  fcols = fDigiPar->GetFECols();
  frows = fDigiPar->GetFERows();
}
