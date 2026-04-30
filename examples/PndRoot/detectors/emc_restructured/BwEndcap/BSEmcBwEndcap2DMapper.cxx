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


#include "BSEmcBwEndcap2DMapper.h"

#include "BSEmcDetectorID.h"

std::pair<Int_t, Int_t> BSEmcBwEndcap2DMapper::GetXYIndices(Int_t t_detectorId) const /*override*/
{
  BSEmcDetectorID id{t_detectorId};
  Int_t crystal = id.GetColumn();
  Int_t row = id.GetRow();
  Int_t copy = id.GetCopy();

  Int_t iXrel = 0, iYrel = 0, iX = 0, iY = 0, rowc = 0;
  if (row < 6) { // side submodules

    iXrel = 4 * (row % 3) + (crystal % 4) - 6;
    iYrel = -4 * (row / 3) - (crystal / 4) + 14;

  } else { // corner submodules
    rowc = row - 6;

    iXrel = 4 * (rowc % 2) + (crystal % 4) + 6;
    iYrel = -4 * (rowc / 2) - (crystal / 4) + 13;
  }

  if (copy == 0) {
    iX = iXrel;
    iY = iYrel;
  }
  if (copy == 1) {
    iX = -iYrel - 1;
    iY = iXrel;
  }
  if (copy == 2) {
    iX = -iXrel - 1;
    iY = -iYrel - 1;
  }
  if (copy == 3) {
    iX = iYrel;
    iY = -iXrel - 1;
  }

  return std::pair<Int_t, Int_t>{(iX), (iY)};
}
