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

//--------------------------------------------------------------------------
// Description:
//	Class PndEmcDigiCalibrator
//      Do an energy and position corrections for EMC cluster
//      PndEmcDigiCalibrator is a factory which call specific method depending on the input parameter "method"
//      method=1 - PndEmcClusterHistCalibrator is called, which perform correction from the interpolation on 2-dimensional histogram on (Energy, theta)
//      method=2 - PndEmcClusterSimpleCalibrator is called, where energy correction is parametrised as a function of (Energy, theta)
//      PndEmcAbsClusterCalibrator - abstract interface class
//
// Author List:
//      D.Melnychuk
//      PndEmcClusterHistCalibrator class is based on code PndEmcMakeCorr.cxx
//      (A. Biegun, M. Babai)
//      PndEmcClusterSimpleCalibrator class is based on EmcPhotonSimpleCalib class in Babar framework (Jan Zhong)
//------------------------------------------------------------------------
//#pragma once
#ifndef PNDEMCDIGICALIBRATOR_H
#define PNDEMCDIGICALIBRATOR_H

#include "TObject.h"
#include "TVectorD.h"
#include <TFile.h>
#include <TString.h>
#include <cstdlib>

class PndEmcDigi;
class PndEmcBump;

class PndEmcDigiCalibrator {

 public:
  // Constructors
  PndEmcDigiCalibrator();
  // Destructor
  virtual ~PndEmcDigiCalibrator();

  Double_t CalibrationEvtTimeByDigi(PndEmcDigi *theDigi, bool PrintOut = kFALSE) const;
  Double_t CalibrationEvtTimeByBump(PndEmcBump *theBump, bool PrintOut = kFALSE) const;

  Double_t GetTimeResolutionOfDigi(PndEmcDigi *theDigi) const;
  Double_t GetTimeResolutionOfShower(PndEmcBump *theBump) const;

 private:
  Int_t GetIdxByEnergy(Double_t energy) const;
  Int_t GetIdxByEnergyForBump(Double_t energy) const;

  PndEmcDigiCalibrator(const PndEmcDigiCalibrator &L);
  PndEmcDigiCalibrator &operator=(const PndEmcDigiCalibrator &) { return *this; };

  static Double_t fTimeWindowOfDigi[5][17];
  static Double_t fTimeWindowOfShower[5][20];

  TVectorD CoeffMod3;
  TVectorD CoeffMod5;
  TVectorD CoeffModo;

  TVectorD CoeffMod3A;
  TVectorD CoeffMod5A;
  TVectorD CoeffModoA;

  TVectorD CoeffMod3B;
  TVectorD CoeffMod5B;
  TVectorD CoeffModoB;
};
#endif // PndEmcDigiCalibrator_HH
