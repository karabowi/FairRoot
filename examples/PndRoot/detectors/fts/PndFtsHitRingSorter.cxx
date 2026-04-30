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
 * PndFtsHitRingSorter.cxx
 *
 *  Created on: Sep 9, 2011
 *      Author: stockman
 */

#include "PndFtsHitRingSorter.h"
#include "PndFtsHit.h"

ClassImp(PndFtsHitRingSorter);

PndFtsHitRingSorter::~PndFtsHitRingSorter()
{
  // TODO Auto-generated destructor stub
}

FairTimeStamp *PndFtsHitRingSorter::CreateElement(FairTimeStamp *data)
{
  return new PndFtsHit(*(PndFtsHit *)data);
}
