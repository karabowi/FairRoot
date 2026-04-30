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

#include "BSEmcBarrelSensorNameIdMap.h"

#include <stdio.h>

#include "TString.h"

#include "fairlogger/Logger.h"

BSEmcBarrelSensorNameIdMap::BSEmcBarrelSensorNameIdMap() : PndSensorNameIdMap() {}

BSEmcBarrelSensorNameIdMap::~BSEmcBarrelSensorNameIdMap() {}

Int_t BSEmcBarrelSensorNameIdMap::CreateId(TString &t_sensName) const
{
  LOG(trace) << "BSEmcBarrelSensorNameIdMap::CreateId(" << t_sensName << ")";

  Int_t module = 0;
  Int_t copy = 0;
  Int_t row = 0;
  Int_t crystal = 0;

  ///////////////////////////////////////////////////////////////////////////
  // Case of new version of barrel section with hole
  ///////////////////////////////////////////////////////////////////////////
  if (t_sensName.Contains("Slice_target")) {
    Int_t iSM = 0, copySM = 0, iMod = 0, copyMod = 0, iAlv = 0, iCry = 0, copyCry = 0;
    char sgnMod = 0, sgnCry = 0, typeCry = 0;
    sscanf(t_sensName.Data(), "/cave_1/BarrelEMC_0/Slice_target_%d/SuperModule%d_Target_%d/Module%d%c_%d/Crystal-%d%c-%c%d_%d", &copy, &iSM, &copySM, &iMod, &sgnMod, &copyMod,
           &iAlv, &sgnCry, &typeCry, &iCry, &copyCry);
    // nMod
    if (sgnCry == 'p') {
      module = 1;
    } else if (sgnCry == 'm') {
      module = 2;
    } else {
      LOG(trace) << "BSEmcBarrelSensorNameIdMap::CreateId(" << t_sensName << ")"
                 << "Error!!!";
      return -1;
    }

    // nRow
    row = (iAlv - 1) * 4 + iCry;

    // nCrys
    if (module == 1) {
      if (typeCry == 'L') {
        crystal = (copyCry - 1) * 2 + 1;
      }
      if (typeCry == 'R') {
        crystal = (copyCry - 1) * 2 + 2;
      }
    } else if (module == 2) {
      if (typeCry == 'R') {
        crystal = (copyCry - 1) * 2 + 1;
      }
      if (typeCry == 'L') {
        crystal = (copyCry - 1) * 2 + 2;
      }
    } else {
      LOG(trace) << "BSEmcBarrelSensorNameIdMap::CreateId(" << t_sensName << ")"
                 << "Error!!!";
      return -1;
    }
    LOG(trace) << "module = " << module << ", row = " << row << ", copy = " << copy << ", crystal = " << crystal;
    LOG(trace) << "detectorId = " << module * 100000000 + row * 1000000 + copy * 10000 + crystal;

    return module * 100000000 + row * 1000000 + copy * 10000 + crystal;
  }

  ///////////////////////////////////////////////////////////////////////////
  // Case of new version of barrel
  ///////////////////////////////////////////////////////////////////////////
  else if (t_sensName.Contains("Slice")) {
    Int_t iSM = 0, copySM = 0, iMod = 0, copyMod = 0, iAlv = 0, iCry = 0, copyCry = 0;
    char sgnMod = 0, sgnCry = 0, typeCry = 0;
    sscanf(t_sensName.Data(), "/cave_1/BarrelEMC_0/Slice_%d/SuperModule%d_%d/Module%d%c_%d/Crystal-%d%c-%c%d_%d", &copy, &iSM, &copySM, &iMod, &sgnMod, &copyMod, &iAlv, &sgnCry,
           &typeCry, &iCry, &copyCry);

    // nMod
    if (sgnCry == 'p') {
      module = 1;
    } else if (sgnCry == 'm') {
      module = 2;
    } else {
      LOG(trace) << "BSEmcBarrelSensorNameIdMap::CreateId(" << t_sensName << ")"
                 << "Error!!!";
      return -1;
    }

    // nRow
    row = (iAlv - 1) * 4 + iCry;

    // nCrys
    if (module == 1) {
      if (typeCry == 'L') {
        crystal = (copyCry - 1) * 2 + 1;
      }
      if (typeCry == 'R') {
        crystal = (copyCry - 1) * 2 + 2;
      }
    } else if (module == 2) {
      if (typeCry == 'R') {
        crystal = (copyCry - 1) * 2 + 1;
      }
      if (typeCry == 'L') {
        crystal = (copyCry - 1) * 2 + 2;
      }
    } else {
      LOG(trace) << "BSEmcBarrelSensorNameIdMap::CreateId(" << t_sensName << ")"
                 << "Error!!!";
      return -1;
    }
    LOG(trace) << "module = " << module << ", row = " << row << ", copy = " << copy << ", crystal = " << crystal;
    LOG(trace) << "detectorId = " << module * 100000000 + row * 1000000 + copy * 10000 + crystal;

    return module * 100000000 + row * 1000000 + copy * 10000 + crystal;
  }
  LOG(trace) << "module = " << module << ", row = " << row << ", copy = " << copy << ", crystal = " << crystal;
  LOG(trace) << "detectorId = " << module * 100000000 + row * 1000000 + copy * 10000 + crystal;

  return module * 100000000 + row * 1000000 + copy * 10000 + crystal;
}
