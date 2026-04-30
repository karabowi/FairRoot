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

#include "BSEmcDetectorID.h"

#include "fairlogger/Logger.h"

BSEmcDetectorID::BSEmcDetectorID(Int_t t_detectorId) : fDetectorId(t_detectorId) {}

BSEmcDetectorID::~BSEmcDetectorID() {}

Short_t BSEmcDetectorID::GetXPad() const
{
  // Return the X pad value for clusterization (phi)

  // Barrel EMC
  if (GetModule() == 1 || GetModule() == 2) {
    return (11 - GetColumn() + (GetCopy() - 1) * 10);
  }
  // FwEndCap
  if (GetModule() == 3) {
    Int_t result = GetColumn();
    if (result > 36) {
      --result;
    }
    return -(result - 36);
    // return -(result - 36)+250; //the minus sign before the paranthesis is introduced since the geometry of FwEndCap gets rotated by 180 deg around the y-axis in PndEmc.cxx;
  } // this rotation was done in turn due to the way the geometry was defined in the geometry file of the forward end cap

  // BwEndCap and forward EMC
  if ((GetModule() == 4) || (GetModule() == 5)) {
    Int_t result = 0;
    // Int_t result = 350;
    if (GetCopy() == 1) {
      result = -GetRow() + 1;
    }
    if (GetCopy() == 2) {
      result = -GetRow() + 1;
    }
    if (GetCopy() == 3) {
      result = GetRow();
    }
    if (GetCopy() == 4) {
      result = GetRow();
    }

    if (GetModule() == 5) {
      // result += 100;
    }
    return result;
  }

  LOG(error) << "BSEmcDetectorID::GetXPad() - Failed to determine proper ypad!";
  return -1000; // failure
}

Short_t BSEmcDetectorID::GetYPad() const
{
  // Return the Y pad value for clusterization (Theta)

  // Barrel EMC
  if (GetModule() == 1) {
    return (GetRow() + 29);
  }
  if (GetModule() == 2) {
    return (-GetRow() + 30);
  }
  // FwEndCap
  if (GetModule() == 3) {
    Int_t result = GetRow();
    if (result > 37) {
      --result;
    }
    return result - 37;
    // return result-37+250;
  }

  // BwEndCap and forward EMC
  if ((GetModule() == 4) || (GetModule() == 5)) {
    // Int_t result = 350;
    Int_t result = 0;
    if (GetCopy() == 1) {
      result = GetColumn();
    }
    if (GetCopy() == 2) {
      result = -GetColumn() + 1;
    }
    if (GetCopy() == 3) {
      result = -GetColumn() + 1;
    }
    if (GetCopy() == 4) {
      result = GetColumn();
    }
    if (GetModule() == 5) {
      // result += 100;
    }
    return result;
  }

  LOG(error) << "BSEmcDetectorID::GetYPad() - Failed to determine proper ypad!";
  return -1000; // failure
}
