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
// Versions of the correction:
// 1 - "emc_module12.dat","emc_module3_2011_new.root","emc_module4_StraightGeo24.4.root","emc_module5_fsc.root" (+ full PANDA geometry) TGeant3
//
// Author List:
//      D.Melnychuk
//      PndEmcClusterHistCalibrator class is based on code PndEmcMakeCorr.cxx
//      (A. Biegun, M. Babai)
//      PndEmcClusterSimpleCalibrator class is based on EmcPhotonSimpleCalib class in Babar framework (Jan Zhong)
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "PndEmcDigiCalibrator.h"
#include "PndEmcDigi.h"
#include "PndEmcBump.h"

#include <algorithm>
#include <iostream>

using std::cout;
using std::endl;

//----------------
// Constructors --
//----------------
Double_t PndEmcDigiCalibrator::fTimeWindowOfDigi[5][17] = {
  //(x-2)<0.5,.6-.8,.8-1,1-2, 2-3, 3-4, 4-5, 5-6, 6-7, 7-8, 8-9, 9-10, 10-20,20-30,30-40,40-50,50+
  14., 14., 9.4, 4.7, 3.0, 2.1, 1.6, 1.3, 1.1, 0.9,  0.8,  0.7,  0.5,  0.3,  0.2,  0.16, 0.14, /*mod=1*/
  14., 14., 9.4, 4.7, 3.0, 2.1, 1.6, 1.3, 1.1, 0.9,  0.8,  0.7,  0.5,  0.3,  0.2,  0.16, 0.14, /*mod=2*/
  7.0, 5.0, 3.6, 2.2, 1.2, 0.8, 0.7, 0.6, 0.5, 0.45, 0.38, 0.36, 0.27, 0.2,  0.20, 0.20, 0.20, /*mod=3*/
  14., 14., 9.4, 4.7, 3.0, 2.1, 1.6, 1.3, 1.0, 0.9,  0.8,  0.7,  0.5,  0.3,  0.2,  0.16, 0.14, /*mod=4*/
  2.3, 1.8, 1.3, 0.9, 0.6, 0.4, 0.3, 0.3, 0.3, 0.3,  0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25  /*mod=5*/
};
Double_t PndEmcDigiCalibrator::fTimeWindowOfShower[5][20] = {
  //(x-2)<0.5,.6-.8,.8-1, 1-2, 2-3, 3-4, 4-5, 5-6, 6-7, 7-8, 8-9, 9-10, 10-20,20-30,30-40,40-50,50+
  24., 18., 12.5, 9.5, 4.7,  3.2,  2.3,  2.0,  1.7,  1.4,  1.2,  1.1,  0.7,  0.4,  0.3,  0.23, 0.19, 0.16, 0.14, 0.12, /*mod=1*/
  24., 18., 12.5, 9.5, 4.7,  3.2,  2.3,  2.0,  1.7,  1.4,  1.2,  1.1,  0.7,  0.4,  0.3,  0.23, 0.19, 0.16, 0.14, 0.12, /*mod=1*/
  7,   5.5, 3.7,  2.9, 2.0,  1.7,  1.7,  1.6,  1.5,  1.3,  1.3,  1.1,  0.86, 0.28, 0.18, 0.14, 0.15, 0.14, 0.14, 0.13, /*mod=3*/
  24., 18., 12.5, 9.5, 4.7,  3.2,  2.3,  2.0,  1.7,  1.4,  1.2,  1.1,  0.7,  0.4,  0.3,  0.23, 0.19, 0.16, 0.14, 0.12, /*mod=1*/
  7,   5.5, 1.4,  1.1, 0.67, 0.44, 0.29, 0.23, 0.24, 0.19, 0.14, 0.18, 0.14, 0.12, 0.13, 0.14, 0.12, 0.13, 0.13, 0.12  /*mod=5*/
};

PndEmcDigiCalibrator::PndEmcDigiCalibrator()
{
  // digi coefficients
  // shashylik
  CoeffMod5.ResizeTo(6);
  CoeffMod5[0] = 41.636;
  CoeffMod5[1] = 0.0532;
  CoeffMod5[2] = 0.0277;
  CoeffMod5[3] = 0.0097;
  CoeffMod5[4] = -7.576e-5;
  CoeffMod5[5] = 1.6053e-7;
  // forward
  CoeffMod3.ResizeTo(6);
  CoeffMod3[0] = 130.996;
  CoeffMod3[1] = 0.00135;
  CoeffMod3[2] = 0.32456;
  CoeffMod3[3] = -0.0295;
  CoeffMod3[4] = -3.0491e-6;
  CoeffMod3[5] = 1.93008e-7;
  // other parts
  // barrel mod == 1, 2, backward mod = 4
  CoeffModo.ResizeTo(6);
  CoeffModo[0] = 246.223;
  CoeffModo[1] = 0.03520;
  CoeffModo[2] = -0.1207;
  CoeffModo[3] = 0.04283;
  CoeffModo[4] = -0.0002;
  CoeffModo[5] = 1.25472e-6;
  // bump coefficients, for numofdigi>3
  CoeffMod5A.ResizeTo(6);
  CoeffMod5A[0] = 52.221;
  CoeffMod5A[1] = 0.0408;
  CoeffMod5A[2] = -0.5354;
  CoeffMod5A[3] = 0.1821;
  CoeffMod5A[4] = -9.337e-3;
  CoeffMod5A[5] = 2.8338e-4;
  // forward
  CoeffMod3A.ResizeTo(6);
  CoeffMod3A[0] = 131.428;
  CoeffMod3A[1] = 0.01073;
  CoeffMod3A[2] = -3.2207;
  CoeffMod3A[3] = 1.2828;
  CoeffMod3A[4] = -3.3757e-2;
  CoeffMod3A[5] = 3.33785e-4;
  // other parts
  // barrel mod == 1, 2, backward mod = 4
  CoeffModoA.ResizeTo(6);
  CoeffModoA[0] = 246.536;
  CoeffModoA[1] = 0.03316;
  CoeffModoA[2] = -0.6604;
  CoeffModoA[3] = 0.29184;
  CoeffModoA[4] = -0.0076;
  CoeffModoA[5] = 1.52179e-4;
  // for bumps numofdigi<=3
  CoeffMod5B.ResizeTo(6);
  CoeffMod5B[0] = 44.206;
  CoeffMod5B[1] = 0.0504;
  CoeffMod5B[2] = -0.140;
  CoeffMod5B[3] = 0.0225;
  CoeffMod5B[4] = -9.603e-7;
  CoeffMod5B[5] = -3.906e-7;
  // forward
  CoeffMod3B.ResizeTo(6);
  CoeffMod3B[0] = 136.73;
  CoeffMod3B[1] = -0.0359;
  CoeffMod3B[2] = 1.9219;
  CoeffMod3B[3] = -0.2218;
  CoeffMod3B[4] = 7.4442e-4;
  CoeffMod3B[5] = -1.16553e-6;
  // other parts
  // barrel mod == 1, 2, backward mod = 4
  CoeffModoB.ResizeTo(6);
  CoeffModoB[0] = 244.578;
  CoeffModoB[1] = 0.04145;
  CoeffModoB[2] = 0.34314;
  CoeffModoB[3] = 0.02705;
  CoeffModoB[4] = -2.6097e-4;
  CoeffModoB[5] = 1.42555e-6;
}

//--------------
// Destructor --
//--------------
PndEmcDigiCalibrator::~PndEmcDigiCalibrator() {}

Double_t PndEmcDigiCalibrator::CalibrationEvtTimeByDigi(PndEmcDigi *theDigi, bool PrintOut) const
{
  Double_t digiT = theDigi->GetTimeStamp();
  Double_t digiE = theDigi->GetEnergy();
  Int_t mod = theDigi->GetModule();
  // Int_t detID = theDigi->GetDetectorId();
  // PndEmcTwoCoordIndex* _tci = PndEmcMapper::Instance()->GetTCI(detID);
  // PndEmcTciXtalMap& map = const_cast<PndEmcTciXtalMap&> ( PndEmcStructure::Instance()->GetTciXtalMap());
  // TVector3 Where = PndEmcDigi::algPointer()(map.find(_tci)->second);
  Double_t path = theDigi->where().Mag();
  // Double_t path = map[_tci]->frontCentre().Mag();

  TVectorD vFunc(6);
  vFunc[0] = 1.;
  vFunc[1] = path;
  vFunc[2] = 1. / TMath::Sqrt(digiE);
  vFunc[3] = 1. / digiE;
  vFunc[4] = vFunc[3] * vFunc[3];
  vFunc[5] = vFunc[4] * vFunc[3];

  Double_t corrT(0.);
  // Int_t mod = detID/100000000;

  if (mod == 3) {
    corrT = CoeffMod3 * vFunc;
  } else if (mod == 5) {
    corrT = CoeffMod5 * vFunc;
  } else {
    corrT = CoeffModo * vFunc;
  }
  if (PrintOut) {
    cout << "path = " << path << ", mod = " << mod << ", corrT = " << corrT << endl;
  }
  return digiT - corrT;
}

Double_t PndEmcDigiCalibrator::CalibrationEvtTimeByBump(PndEmcBump *theBump, bool PrintOut) const
{
  Double_t bumpT = theBump->GetTimeStamp();
  Double_t bumpE = theBump->GetEnergy();
  Int_t mod = theBump->GetModule();
  Int_t nDigi = theBump->NumberOfDigis();
  // Int_t detID = theDigi->GetDetectorId();
  // PndEmcTwoCoordIndex* _tci = PndEmcMapper::Instance()->GetTCI(detID);
  // PndEmcTciXtalMap& map = const_cast<PndEmcTciXtalMap&> ( PndEmcStructure::Instance()->GetTciXtalMap());
  // TVector3 Where = PndEmcDigi::algPointer()(map.find(_tci)->second);
  Double_t path = theBump->where().Mag();
  // Double_t path = map[_tci]->frontCentre().Mag();

  TVectorD vFunc(6);
  vFunc[0] = 1.;
  vFunc[1] = path;
  vFunc[2] = 1. / TMath::Sqrt(bumpE);
  vFunc[3] = 1. / bumpE;
  vFunc[4] = vFunc[3] * vFunc[3];
  vFunc[5] = vFunc[4] * vFunc[3];

  Double_t corrT(0.);
  // Int_t mod = detID/100000000;

  if (mod == 3) {
    if (nDigi > 3)
      corrT = CoeffMod3A * vFunc;
    else
      corrT = CoeffMod3B * vFunc;
  } else if (mod == 5) {
    if (nDigi > 3)
      corrT = CoeffMod5A * vFunc;
    else
      corrT = CoeffMod5B * vFunc;
  } else {
    if (nDigi > 3)
      corrT = CoeffModoA * vFunc;
    else
      corrT = CoeffModoB * vFunc;
  }
  if (PrintOut) {
    cout << "path = " << path << ", mod = " << mod << ", corrT = " << corrT << endl;
  }
  return bumpT - corrT;
}

Double_t PndEmcDigiCalibrator::GetTimeResolutionOfDigi(PndEmcDigi *theDigi) const
{
  Int_t TheIndexOfEnergy = GetIdxByEnergy(theDigi->GetEnergy());
  Int_t TheIndexOfModule = theDigi->GetModule();
  return PndEmcDigiCalibrator::fTimeWindowOfDigi[TheIndexOfModule - 1][TheIndexOfEnergy];
}

Double_t PndEmcDigiCalibrator::GetTimeResolutionOfShower(PndEmcBump *theBump) const
{
  Int_t TheIndexOfEnergy = GetIdxByEnergyForBump(theBump->GetEnergy());
  Int_t TheIndexOfModule = theBump->GetModule();
  return PndEmcDigiCalibrator::fTimeWindowOfShower[TheIndexOfModule - 1][TheIndexOfEnergy];
}
Int_t PndEmcDigiCalibrator::GetIdxByEnergy(Double_t energy) const
{
  if (energy < 0.006)
    return 0;
  if (energy < 0.008)
    return 1;
  if (energy < 0.010)
    return 2;
  if (energy < 0.1)
    return 2 + Int_t(energy / 0.01); // energy step 0.01GeV
  if (energy >= 0.1) {
    Int_t idx = 11 + Int_t(energy / 0.1);
    if (idx > 16)
      idx = 16;
    return idx;
  }
  return -1;
}
Int_t PndEmcDigiCalibrator::GetIdxByEnergyForBump(Double_t energy) const
{
  if (energy < 0.006)
    return 0;
  if (energy < 0.008)
    return 1;
  if (energy < 0.010)
    return 2;
  if (energy < 0.1)
    return 2 + Int_t(energy / 0.01); // energy step 0.01GeV
  if (energy >= 0.1) {
    Int_t idx = 11 + Int_t(energy / 0.1);
    if (idx > 19)
      idx = 19;
    return idx;
  }
  return -1;
}
