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

#include "BSEmcShashlikSensorNameIdMap.h"

#include <stdio.h>

#include "TString.h"

#include "fairlogger/Logger.h"

BSEmcShashlikSensorNameIdMap::BSEmcShashlikSensorNameIdMap() : PndSensorNameIdMap() {}

BSEmcShashlikSensorNameIdMap::~BSEmcShashlikSensorNameIdMap() {}

Int_t BSEmcShashlikSensorNameIdMap::CreateId(TString &t_sensName) const
{

  LOG(trace) << "BSEmcShashlikSensorNameIdMap::CreateId(" << t_sensName << ")";
  Int_t nMod = -1;
  Int_t nRow = -1;
  Int_t nCrys = -1;
  Int_t copyNo = -1;

  if (t_sensName.Contains("FscModuleVolume")) {
    Int_t SupModCopy = 0;
    Int_t LocCopy = 0;
    Int_t nSupCol = 0;
    Int_t nSupRow = 0;
    Int_t nModCol = 0;
    Int_t nModRow = 0;

    sscanf(t_sensName.Data(), "/cave_1/Emc%i_%i/FscSuperModuleVolume_%i/FscTyvekVolume_%*i/FscModuleVolume_%i/%*s", &nMod, &copyNo, &SupModCopy, &LocCopy);
    copyNo += 1;

    nSupCol = SupModCopy / 100;
    nSupRow = SupModCopy % 100;

    nModCol = LocCopy % 2;
    nModRow = LocCopy / 2;

    nCrys = (nSupCol - 1) * 2 + nModCol + 1;
    nRow = (nSupRow - 1) * 2 + nModRow + 1;
  }
  Int_t result = nMod * 100000000 + nRow * 1000000 + copyNo * 10000 + nCrys;
  size_t remove_position = t_sensName.Index("/FscLayerHolderVolume");
  t_sensName = t_sensName.Remove(remove_position);
  LOG(trace) << "BSEmcShashlikSensorNameIdMap::CreateId() returning " << result;
  return result;
}
