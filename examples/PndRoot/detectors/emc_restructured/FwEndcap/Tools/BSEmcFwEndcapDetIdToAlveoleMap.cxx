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

#include "BSEmcFwEndcapDetIdToAlveoleMap.h"

#include <fstream>

#include "TSystem.h"

#include "FairLogger.h"

void BSEmcFwEndcapDetIdToAlveoleMap::LoadMap()
{
  fDetIdToAlveole.clear();
  TString filename = gSystem->Getenv("VMCWORKDIR");
  filename += "/input/" + fFilename;

  std::ifstream myStream(filename);
  Int_t detId{0}, x{0}, y{0}, crystal{0};
  if (myStream.is_open()) {
    while (myStream >> detId >> x >> y >> crystal) {
      if (300000000 < detId && detId < 400000000) {
        fDetIdToAlveole[detId] = {x, y, crystal};
        fAlveoleDetIds[ConvertToKey(x, y)].push_back(detId);
      }
    }
  } else {
    LOG(error) << " BSEmcFwEndcapDetIdToAlveoleMap::LoadMap() failed for " << fFilename;
  }
}

std::vector<Int_t> BSEmcFwEndcapDetIdToAlveoleMap::GetAlveoleCrystals(const Int_t t_detectorId) const
{
  auto alveole = GetAlveoleId(t_detectorId);
  const TString key = ConvertToKey(alveole.fX, alveole.fY);
  return GetAlveoleCrystals(key);
}
