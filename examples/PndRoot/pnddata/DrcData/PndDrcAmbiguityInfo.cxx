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

// -----------------------------------------
// PndDrcAmbiguityInfo.h
//
// Created on: 18.10.2013
// Author  : R.Dzhygadlo at gsi.de
// -----------------------------------------

#include "PndDrcAmbiguityInfo.h"

ClassImp(PndDrcAmbiguityInfo)

  // -----   Default constructor   -------------------------------------------
  PndDrcAmbiguityInfo::PndDrcAmbiguityInfo()
  : TObject(), fCherenkov(0), fBarTime(0), fEvTime(0)
{
}
