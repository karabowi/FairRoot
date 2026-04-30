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

/////////////////////////////////////////////////////////////
//
//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id:$
//
// Description:
//	EMC Digi.
//
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI
//
// Author List:
//	Xiaorong Shi            Lawrence Livermore National Lab
//	Stephen J. Gowdy        University of Edinburgh
// Dima Melnichuk - adaption for PANDA
//
// Copyright Information:
//	Copyright (C) 1996	Lawrence Livermore National Lab
//
///////////////////////////////////////////////////////////////
#include "BSEmcBarrel2DMapper.h"

#include "BSEmcDetectorID.h"

BSEmcBarrel2DMapper::BSEmcBarrel2DMapper() : BSEmc2DMapper() {}

BSEmcBarrel2DMapper::~BSEmcBarrel2DMapper() {}

std::pair<Int_t, Int_t> BSEmcBarrel2DMapper::GetXYIndices(Int_t t_detectorId) const
{
  BSEmcDetectorID id{t_detectorId};
  Int_t module = id.GetModule();
  Int_t crystal = id.GetColumn();
  Int_t row = id.GetRow();
  Int_t copy = id.GetCopy();
  Int_t iTheta{}, iPhi{};
  if (module == 1) {
    iPhi = (11 - crystal) + (copy - 1) * 10;
    iTheta = row + 28;
  } else if (module == 2) {
    iPhi = (11 - crystal) + (copy - 1) * 10;
    iTheta = -row + 29;
  }
  return std::pair<Int_t, Int_t>{iPhi, iTheta};
}