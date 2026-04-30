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
 * PndTaskJSONReader.cxx
 *
 *  Created on: 20.09.2022
 *      Author: tstockmanns
 */

#include "PndTaskJSONReader.h"
#include "PndTrackingQAJSONReader.h"
#include "PndIdealTrackingJSONReader.h"

#include <FairLogger.h>

PndTaskJSONReader *PndTaskJSONReader::createTaskReader(std::string taskName)
{
  if (taskName == "trackingQAs")
    return new PndTrackingQAJSONReader();
  else if (taskName == "idealTracking")
    return new PndIdealTrackingJSONReader();
  else
    LOG(error) << "No valid task name was found: " << taskName << std::endl;
  return nullptr;
}

ClassImp(PndTaskJSONReader);
