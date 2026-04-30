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

#include "CmpPndBarrelSonderslice2DMapper.h"

#include "BSEmcDetectorID.h"

CmpPndBarrelSonderslice2DMapper::CmpPndBarrelSonderslice2DMapper() : BSEmcBarrel2DMapper() {}

CmpPndBarrelSonderslice2DMapper::~CmpPndBarrelSonderslice2DMapper() {}

std::pair<Int_t, Int_t> CmpPndBarrelSonderslice2DMapper::GetXYIndices(Int_t t_detectorId) const
{
  BSEmcDetectorID id{t_detectorId};
  Int_t slice = id.GetCopy();

  if (slice != 17 || slice != 18) {
    return BSEmcBarrel2DMapper::GetXYIndices(t_detectorId);
  }
  Int_t row = id.GetRow();
  Int_t crystal = id.GetColumn();

  return std::pair<Int_t, Int_t>{row, crystal};
}
