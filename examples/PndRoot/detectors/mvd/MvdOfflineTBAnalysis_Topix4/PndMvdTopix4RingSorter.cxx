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
 * PndMvdTopix4RingSorter.cxx
 *
 *  Created on: Sep 9, 2011
 *      Author: stockman
 */

#include "PndSdsDigiTopix4.h"
#include "PndMvdTopix4RingSorter.h"

ClassImp(PndMvdTopix4RingSorter);

PndMvdTopix4RingSorter::~PndMvdTopix4RingSorter()
{
  // TODO Auto-generated destructor stub
}

FairTimeStamp *PndMvdTopix4RingSorter::CreateElement(FairTimeStamp *data)
{
  return new PndSdsDigiTopix4(*(PndSdsDigiTopix4 *)data);
}
