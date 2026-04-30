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

#include "CmpPndBarrelSondersliceSensorNameIdMap.h"

#include <stdio.h>

#include "TString.h"
#include "TObjArray.h"
#include "TObjString.h"

#include "fairlogger/Logger.h"

CmpPndBarrelSondersliceSensorNameIdMap::CmpPndBarrelSondersliceSensorNameIdMap() : PndSensorNameIdMap() {}

CmpPndBarrelSondersliceSensorNameIdMap::~CmpPndBarrelSondersliceSensorNameIdMap() {}

Int_t CmpPndBarrelSondersliceSensorNameIdMap::CreateId(TString &t_sensName) const
{

  LOG(trace) << "CmpPndBarrelSondersliceSensorNameIdMap::CreateId(" << t_sensName << ")";

  if (t_sensName.Contains("vol_shape_Sonderslice_Crystal")) {
    TString name = t_sensName;

    // TODO:/FIXME: I failed to implement a working sscanf patter matching and had to 
    // break the string up into smaller sets, to patternmatch there. 
    TObjArray* pathComponents = name.Tokenize("/");
    LOG(trace) << "CmpPndBarrelSondersliceSensorNameIdMap::CreateId() reducing to: " << name;
    Int_t sonderslice = 0;
    TString sondersliceComponent = dynamic_cast<TObjString*>(pathComponents->At(2))->GetString();
    int success = sscanf(sondersliceComponent.Data(), "Sonderslice_%d_%*d", &sonderslice);
    if (success != 1) {
      LOG(error) << "CmpPndBarrelSondersliceSensorNameIdMap::CreateId() could not match pattern Sonderslice_%d_%*d with: " << sondersliceComponent;
      pathComponents->Delete();
      delete pathComponents;
      exit(-1);
    }
    Int_t crystal = 10;
    TString crystalComponent = dynamic_cast<TObjString*>(pathComponents->At(4))->GetString();
    success = sscanf(crystalComponent.Data(), "Sonderslice_Crystal_%d_%*s", &crystal);
    if (success != 1) {
      LOG(error) << "CmpPndBarrelSondersliceSensorNameIdMap::CreateId() could not match pattern Sonderslice_Crystal_%d_%*s with: " << crystalComponent;
      pathComponents->Delete();
      delete pathComponents;
      exit(-1);
    }
    pathComponents->Delete();
    delete pathComponents;
    const Int_t nMod = 1; //for Barrel region

    crystal -= 1; //offset, as counting starts at 1, causing problems with / and % to get row and column.
    Int_t thetarow = crystal / 4;
    Int_t phirow = (crystal % 4 );
    Int_t result = nMod * 100000000 + thetarow * 1000000 + (sonderslice+16) * 10000 + phirow;
    LOG(trace) << "CmpPndBarrelSondersliceSensorNameIdMap::CreateId() Sonderslice " << sonderslice << " crystal " << crystal << " thetarow (crystal / 4) = " << thetarow
               << " phirow (crystal % 4) = " << phirow;

    LOG(trace) << "CmpPndBarrelSondersliceSensorNameIdMap::CreateId() returning " << result;
    return result;
  }
  return -1;
}
