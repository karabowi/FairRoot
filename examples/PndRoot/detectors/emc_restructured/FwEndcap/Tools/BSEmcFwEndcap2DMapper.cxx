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


#include "BSEmcFwEndcap2DMapper.h"

#include "BSEmcDetectorID.h"

std::pair<Int_t, Int_t> BSEmcFwEndcap2DMapper::GetXYIndices(Int_t t_detectorId) const /*override*/
{
  BSEmcDetectorID id{t_detectorId};
  Int_t crystal = id.GetColumn();
  Int_t row = id.GetRow();
  if (crystal > 36) {
    --crystal;
  }
  if (row > 37) {
    --row;
  }
  return std::pair<Int_t, Int_t>{crystal, row};
}
