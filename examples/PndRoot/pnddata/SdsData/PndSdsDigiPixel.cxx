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

#include "PndSdsDigiPixel.h"

PndSdsDigiPixel::PndSdsDigiPixel() : PndSdsDigi(), fCol(-1), fRow(-1) {}

PndSdsDigiPixel::PndSdsDigiPixel(std::vector<Int_t> index, Int_t detID, Int_t sensorID, Int_t fe, Int_t col, Int_t row, Double_t charge, Double_t timeStamp)
  : PndSdsDigi(index, detID, sensorID, fe, charge, timeStamp), fCol(col), fRow(row)
{
}

ClassImp(PndSdsDigiPixel);
