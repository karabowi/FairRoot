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

#include "PndMQSdsPixelBackMapping.h"
#include "FairLogger.h"

PndMQSdsPixelBackMapping::PndMQSdsPixelBackMapping()
{
  fGeoH = PndGeoHandling::Instance();
  fVerbose = 0;
}

PndMQSdsPixelBackMapping::PndMQSdsPixelBackMapping(PndGeoHandling *geo)
{
  fGeoH = geo;
  fVerbose = 0;
}

PndMQSdsPixelBackMapping::~PndMQSdsPixelBackMapping() {}
