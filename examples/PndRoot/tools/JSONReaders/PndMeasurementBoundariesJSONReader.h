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
 * PndMeasurementBoundariesJSONReader.h
 *
 *  Created on: 16.05.2023
 *      Author: tstockmanns
 */

#pragma once

#include "PndMeasurementBoundaries.h"

#include <boost/property_tree/ptree.hpp>

namespace pt = boost::property_tree;

class PndMeasurementBoundariesJSONReader {
 public:
  PndMeasurementBoundariesJSONReader();
  virtual ~PndMeasurementBoundariesJSONReader();

  PndMeasurementBoundaries createMeasurementBoundaries(pt::ptree &tree);
};
