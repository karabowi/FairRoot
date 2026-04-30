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
 * PndSttTubeMap.cxx
 *
 *  Created on: 02.08.2018
 *      Author: tstockmanns
 */

#include "PndSttTubeMap.h"
#include "PndSttTube.h"

ClassImp(PndSttTubeMap);

PndSttTubeMap *PndSttTubeMap::fInstance = nullptr;

PndSttTubeMap::PndSttTubeMap()
{
  // TODO Auto-generated constructor stub
}

PndSttTubeMap::~PndSttTubeMap()
{
  // TODO Auto-generated destructor stub
}

PndSttTubeMap *PndSttTubeMap::Instance()
{
  if (!fInstance) {
    fInstance = new PndSttTubeMap();
  }
  return fInstance;
}
