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
 * PndMeasurementBoundariesJSONReader.cxx
 *
 *  Created on: 16.05.2023
 *      Author: tstockmanns
 */

#include "PndMeasurementBoundariesJSONReader.h"
#include "JSONReadersHelper.h"

PndMeasurementBoundariesJSONReader::PndMeasurementBoundariesJSONReader()
{
  // TODO Auto-generated constructor stub
}

PndMeasurementBoundariesJSONReader::~PndMeasurementBoundariesJSONReader()
{
  // TODO Auto-generated destructor stub
}

PndMeasurementBoundaries PndMeasurementBoundariesJSONReader::createMeasurementBoundaries(pt::ptree &tree)
{
  PndMeasurementBoundaries result;
  for (pt::ptree::value_type &property : tree.get_child("CDashMeasurementBoundaries")) {
    pt::ptree &boundaries = property.second;

    for (pt::ptree::value_type &boundary : boundaries) {
      std::pair<double, double> values = JSONReaders::readPair(boundary.second);
      result.SetBoundaries(boundary.first.data(), values.first, values.second);
    }
  }
  return result;
}
