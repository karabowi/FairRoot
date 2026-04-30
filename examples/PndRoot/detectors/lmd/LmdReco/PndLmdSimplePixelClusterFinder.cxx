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

// A simple clusterfinder for pixel detectors
#include "PndLmdSimplePixelClusterFinder.h"
#include "PndSdsIdealChargeConversion.h"
#include "PndSdsPixelDigiPar.h"
#include "PndSdsTotChargeConversion.h"
#include "PndSdsTotDigiPar.h"

PndLmdSimplePixelClusterFinder::PndLmdSimplePixelClusterFinder(Int_t verbose) : PndSdsSimplePixelClusterFinder()
{
  fVerbose = verbose;
  FairRun *ana = FairRun::Instance();
  FairRuntimeDb *rtdb = ana->GetRuntimeDb();
  PndSdsPixelDigiPar *fDigiPar = (PndSdsPixelDigiPar *)(rtdb->getContainer("LMDPixelDigiPar"));
  PndSdsTotDigiPar *fTotDigiPar = (PndSdsTotDigiPar *)(rtdb->getContainer("LMDPixelTotDigiPar"));
  if (fDigiPar->GetChargeConvMethod() == 0) {
    if (fVerbose > 0)
      std::cout << "Info in <PndLmdSimplePixelClusterFinder>: ideal charge conversion" << std::endl;
    fChargeConverter = new PndSdsIdealChargeConversion(fDigiPar->GetNoise());
  } else if (fDigiPar->GetChargeConvMethod() == 1) {
    if (fVerbose > 0)
      std::cout << "Info in <PndLmdSimplePixelClusterFinder>: TOT charge conversion" << std::endl;
    fChargeConverter =
      new PndSdsTotChargeConversion(fTotDigiPar->GetChargingTime(), fTotDigiPar->GetConstCurrent(), fDigiPar->GetThreshold(), fTotDigiPar->GetClockFrequency(), fVerbose);
  } else {
    std::cout << "Error in <PndLmdSimplePixelClusterFinder>: charge conversion "
                 "method not defined, use ideal converter..."
              << std::endl;
    fChargeConverter = new PndSdsIdealChargeConversion(fDigiPar->GetNoise());
  }
  fcols = fDigiPar->GetFECols();
  frows = fDigiPar->GetFERows();
  fradius = fDigiPar->GetClustRadius();
};
