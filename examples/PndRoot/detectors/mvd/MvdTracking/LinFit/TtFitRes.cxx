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

// Simone Bianco
// 15.07.2010

#include "stdlib.h"
#include <iostream>
#include <vector>
#include "TtFitRes.h"

// -----   Default constructor   -------------------------------------------
TtFitRes::TtFitRes() : fP0(0.), fP1(0.), fP2(0.), fP3(0.), fEloss(0.), fChiSqX(0.), fChiSqY(0.), fNsens(0) {}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
TtFitRes::TtFitRes(Double_t p0, Double_t p1, Double_t p2, Double_t p3, Double_t eloss, Double_t chiX, Double_t chiY, Int_t numSens)
  : fP0(p0), fP1(p1), fP2(p2), fP3(p3), fEloss(eloss), fChiSqX(chiX), fChiSqY(chiY), fNsens(numSens)
{
}

// -----   Destructor   ----------------------------------------------------
TtFitRes::~TtFitRes() {}

// -------------------------------------------------------------------------
ClassImp(TtFitRes);
