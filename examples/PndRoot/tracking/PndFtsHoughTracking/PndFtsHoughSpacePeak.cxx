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

#include "PndFtsHoughSpacePeak.h"

#include <iostream>

ClassImp(PndFtsHoughSpacePeak);

PndFtsHoughSpacePeak::PndFtsHoughSpacePeak(Int_t height, Int_t firstBin, Int_t firstHitIdx) : fHeight(height), fFinished(kFALSE)
{
  // warn if not all values were set
  if (noVal == firstHitIdx)
    std::cout << "WARNING FROM PndFtsHoughSpacePeak, not all values set in initialisation!\n";
  addBin(firstBin, firstHitIdx);
}

PndFtsHoughSpacePeak::~PndFtsHoughSpacePeak() {}
