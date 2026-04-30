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

#include "BSEmcBwEndcapSensorNameIdMap.h"

#include <stdio.h>

#include "TString.h"

#include "fairlogger/Logger.h"

BSEmcBwEndcapSensorNameIdMap::BSEmcBwEndcapSensorNameIdMap() : PndSensorNameIdMap() {}

BSEmcBwEndcapSensorNameIdMap::~BSEmcBwEndcapSensorNameIdMap() {}

Int_t BSEmcBwEndcapSensorNameIdMap::CreateId(TString &t_sensName) const
{
  LOG(trace) << "BSEmcBwEndcapSensorNameIdMap::CreateId(" << t_sensName << ")";
  Int_t module = 0;
  Int_t copy = 0;
  Int_t row = 0;
  Int_t crystal = 0;

  ///////////////////////////////////////////////////////////////////////////
  // Bwd Endcap geometry - 2017 version
  // copy: quarter no. (0-4), row: submodule no. (0-9), crystal: cryst. no. (0-15)
  ///////////////////////////////////////////////////////////////////////////
  if (t_sensName.Contains("BWECinnerVol")) {

    Int_t submodType = -1;
    const char pathform[] = "/cave_1/BWECouterVol_0/BWECinnerVol_0/BWECquarter_%i/BWECsubmodule%i_%i/PWOCrystal_%i";
    sscanf(t_sensName.Data(), pathform, &copy, &submodType, &row, &crystal);
    module = 4;
  }
  LOG(trace) << "module = " << module << ", row = " << row << ", copy = " << copy << ", crystal = " << crystal;
  LOG(trace) << "detectorId = " << module * 100000000 + row * 1000000 + copy * 10000 + crystal;
  return module * 100000000 + row * 1000000 + copy * 10000 + crystal;
}
