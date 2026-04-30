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

#include <cmath>
#include <fstream>

#include "FairLogger.h"

#include "BSEmcLUTBasedPulseshape.h"

BSEmcLUTBasedPulseshape::BSEmcLUTBasedPulseshape() {}

Double_t BSEmcLUTBasedPulseshape::operator()(const Double_t t_time, const Double_t t_amp, const Double_t t_timeOffset) const
{
  Int_t bin = (t_time - t_timeOffset) / 12.5; // divide the relative time by 12.5 ns to get the sample value

  if (bin < 0) { // if we are still before the peak starts, return 0
    return 0;
  } else if ((UInt_t)bin >= fLUT.size()) {
    return 0;
  }

  return t_amp * fLUT[bin];
}

void BSEmcLUTBasedPulseshape::ReadLUT(const std::string &t_lutfile)
{
  fLUT.clear();
  std::ifstream txtfile;
  txtfile.open(t_lutfile);
  Double_t value;
  while (txtfile >> value) {
    fLUT.push_back(value);
  }
}

ClassImp(BSEmcLUTBasedPulseshape);
