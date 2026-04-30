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
 * PndTrackingQACDashJSONReader.h
 *
 *  Created on: 16.05.2023
 *      Author: tstockmanns
 */

#pragma once

#include "PndMeasurementBoundariesJSONReader.h"
#include "PndTrackingQACDash.h"

#include <boost/property_tree/ptree.hpp>

namespace pt = boost::property_tree;

class PndTrackingQACDashJSONReader : public TObject {
 public:
  PndTrackingQACDashJSONReader();
  virtual ~PndTrackingQACDashJSONReader();

  PndTrackingQACDash createTrackingQACDash(pt::ptree &tree);

  ClassDef(PndTrackingQACDashJSONReader, 1);
};
