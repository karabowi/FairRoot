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

// -------------------------------------------------------------------------
// -----                     PndEmcReader header file                  -----
// -----               Created 14/08/06  by S.Spataro                  -----
// -------------------------------------------------------------------------
//#pragma once
#ifndef PNDEMCREADER_H
#define PNDEMCREADER_H

#include <vector>
#include "TString.h"

class DataG4 {

 public:
  DataG4()
    : crystal(0), row(0), module(0), theta(0), phi(0), tau(0), posX(0), posY(0), posZ(0), pDz(0), pTheta(0), pPhi(0), pDy1(0), pDx1(0), pDx2(0), pAlp1(0), pDy2(0), pDx3(0),
      pDx4(0), pAlp2(0)
  {
  }
  ~DataG4() {}

  int crystal, row, module;
  double theta, phi, tau;
  double posX, posY, posZ;
  double pDz, pTheta, pPhi, pDy1, pDx1, pDx2, pAlp1, pDy2, pDx3, pDx4, pAlp2;
};

class PndEmcReader {

 public:
  PndEmcReader(TString name);
  ~PndEmcReader();

  std::vector<DataG4> g4data;

  TString sName;
  void fill_vector();
  int GetMaxModules();
  int GetMaxRows(int module);
  int GetMaxCrystals(int module, int row);
  int GetMinModules();
  int GetMinRows(int module);
  int GetMinCrystals(int module, int row);
  DataG4 GetData(int module, int row, int crystal);
};
#endif // PNDEMCREADER_H
