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

#include "PndLmdDigiQ.h"

PndLmdDigiQ::PndLmdDigiQ() : PndSdsDigiPixel()
{
  fSigfl = true;
  fPDG = -1;
}

PndLmdDigiQ::PndLmdDigiQ(const PndSdsDigiPixel &digiIn, bool sigfl) : PndSdsDigiPixel(digiIn)
{
  fSigfl = sigfl;
  fPDG = -1;
}

ClassImp(PndLmdDigiQ);
