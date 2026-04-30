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
 * PndSciTHitRingSorter.cxx
 *
 *  Created on: May 13, 2015
 *      Author: Steinschaden Dominik
 *  Last update: 6.2015
 */

#include "PndSciTHitRingSorter.h"
#include "PndSciTHit.h"

ClassImp(PndSciTHitRingSorter);

PndSciTHitRingSorter::~PndSciTHitRingSorter()
{
  // TODO Auto-generated destructor stub
}

FairTimeStamp *PndSciTHitRingSorter::CreateElement(FairTimeStamp *data)
{
  return new PndSciTHit(*(PndSciTHit *)data);
}
