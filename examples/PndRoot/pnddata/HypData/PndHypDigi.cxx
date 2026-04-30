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

#include "PndHypDigi.h"

PndHypDigi::PndHypDigi() : fIndex(), fDetID(-1), fDetName(""), fCharge(-1), fFE(0) {}

PndHypDigi::PndHypDigi(Int_t index, Int_t detID, TString detName, Int_t fe, Double_t charge) : fIndex(), fDetID(detID), fDetName(detName), fCharge(charge), fFE(fe)
{
  fIndex.push_back(index);
}

ClassImp(PndHypDigi)
