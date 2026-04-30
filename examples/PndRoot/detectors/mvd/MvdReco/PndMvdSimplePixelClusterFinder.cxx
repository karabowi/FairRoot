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
#include "PndMvdSimplePixelClusterFinder.h"
#include "PndSdsIdealChargeConversion.h"
#include "PndSdsTotChargeConversion.h"
#include "FairLogger.h"

PndMvdSimplePixelClusterFinder::PndMvdSimplePixelClusterFinder(TString parName, TString totParName, Int_t verbose)
  : PndSdsSimplePixelClusterFinder(), fParName(parName), fTotParName(totParName), fDigiPar(nullptr), fTotDigiPar(nullptr)
{
  fVerbose = verbose;
  FairRun *ana = FairRun::Instance();
  FairRuntimeDb *rtdb = ana->GetRuntimeDb();
  fDigiPar = (PndSdsPixelDigiPar *)(rtdb->getContainer(fParName.Data()));
  fTotDigiPar = (PndSdsTotDigiPar *)(rtdb->getContainer(fTotParName.Data()));
  SetParameters();
};

PndMvdSimplePixelClusterFinder::PndMvdSimplePixelClusterFinder(PndSdsPixelDigiPar *digiPar, PndSdsTotDigiPar *totPar)
  : PndSdsSimplePixelClusterFinder(), fParName(""), fTotParName(""), fDigiPar(digiPar), fTotDigiPar(totPar)
{
  SetParameters();
}

void PndMvdSimplePixelClusterFinder::SetParameters()
{
  if (fDigiPar->GetChargeConvMethod() == 0) {
    if (fVerbose > 0)
      std::cout << "Info in <PndMvdSimplePixelClusterFinder>: ideal charge conversion" << std::endl;
    fChargeConverter = new PndSdsIdealChargeConversion(fDigiPar->GetNoise());
  } else if (fDigiPar->GetChargeConvMethod() == 1) {
    if (fVerbose > 0)
      std::cout << "Info in <PndMvdSimplePixelClusterFinder>: TOT charge conversion" << std::endl;
    fChargeConverter =
      new PndSdsTotChargeConversion(fTotDigiPar->GetChargingTime(), fTotDigiPar->GetConstCurrent(), fDigiPar->GetThreshold(), fTotDigiPar->GetClockFrequency(), fVerbose);
  } else {
    std::cout << "Error in <PndMvdSimplePixelClusterFinder>: charge conversion method not defined, use ideal converter..." << std::endl;
    fChargeConverter = new PndSdsIdealChargeConversion(fDigiPar->GetNoise());
  }
  fcols = fDigiPar->GetFECols();
  frows = fDigiPar->GetFERows();
  fradius = fDigiPar->GetClustRadius();

  if (fVerbose > 1)
    std::cout << "PndMvdSimplePixelClusterFinder::SetParameters() Cols/Rows " << fcols << "/" << frows << " Radius: " << fradius << std::endl;
}
