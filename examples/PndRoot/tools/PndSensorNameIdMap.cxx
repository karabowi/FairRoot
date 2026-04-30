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

#include "PndSensorNameIdMap.h"

PndSensorNameIdMap::PndSensorNameIdMap() : TObject() {}

PndSensorNameIdMap::~PndSensorNameIdMap() {}

Int_t PndSensorNameIdMap::GetId(TString &sensName) const
{
  Int_t id = CreateId(sensName);
  if (id > 0) {
    return id;
  }
  return -1;
}

ClassImp(PndSensorNameIdMap);
