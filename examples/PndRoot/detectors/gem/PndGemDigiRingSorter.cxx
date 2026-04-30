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

/*
 * PndGemDigiRingSorter.cxx
 *
 *  Created on: Sep 9, 2011
 *      Author: stockman
 */

#include "PndGemDigiRingSorter.h"
#include "PndGemDigi.h"

ClassImp(PndGemDigiRingSorter);

PndGemDigiRingSorter::~PndGemDigiRingSorter()
{
  // TODO Auto-generated destructor stub
}

FairTimeStamp *PndGemDigiRingSorter::CreateElement(FairTimeStamp *data)
{
  return new PndGemDigi(*(PndGemDigi *)data);
}
