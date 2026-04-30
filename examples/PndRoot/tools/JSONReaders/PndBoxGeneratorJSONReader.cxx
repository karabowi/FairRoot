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
 * PndBoxGeneratorJSONReader.cxx
 *
 *  Created on: 20.09.2022
 *      Author: tstockmanns
 */

#include "PndBoxGenerator.h"
#include "PndBoxGeneratorJSONReader.h"
#include "JSONReadersHelper.h"

#include <FairLogger.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <string>
#include <set>
#include <exception>
#include <iostream>
#include <utility>
#include <TObject.h>
#include <sstream>

PndBoxGenerator *PndBoxGeneratorJSONReader::readJSON(pt::ptree &tree)
{

  PndBoxGenerator *generator = new PndBoxGenerator();
  for (auto &val : tree) {
    if (val.first.front() == '_') {
      // this is a comment. So ignore it.
    } else if (val.first.compare("PID") == 0) {
      generator->SetPDGType(val.second.get_value<int>());
    } else if (val.first.compare("nParticles") == 0) {
      generator->SetMultiplicity(val.second.get_value<int>());
    } else if (val.first.compare("p") == 0) {
      std::pair<double, double> minMax = JSONReaders::readPair(val.second);
      generator->SetPRange(minMax.first, minMax.second);
    } else if (val.first.compare("pt") == 0) {
      std::pair<double, double> minMax = JSONReaders::readPair(val.second);
      generator->SetPtRange(minMax.first, minMax.second);
    } else if (val.first.compare("phi") == 0) {
      std::pair<double, double> minMax = JSONReaders::readPair(val.second);
      generator->SetPhiRange(minMax.first, minMax.second);
    } else if (val.first.compare("eta") == 0) {
      std::pair<double, double> minMax = JSONReaders::readPair(val.second);
      generator->SetEtaRange(minMax.first, minMax.second);
    } else if (val.first.compare("Y") == 0) {
      std::pair<double, double> minMax = JSONReaders::readPair(val.second);
      generator->SetYRange(minMax.first, minMax.second);
    } else if (val.first.compare("theta") == 0) {
      std::pair<double, double> minMax = JSONReaders::readPair(val.second);
      generator->SetThetaRange(minMax.first, minMax.second);
    } else if (val.first.compare("cosTheta") == 0) {
      if (val.second.get_value<bool>() == true)
        generator->SetCosTheta();
    } else if (val.first.compare("inverseP") == 0) {
      if (val.second.get_value<bool>() == true)
        generator->SetInverseP();
    } else if (val.first.compare("xyz") == 0) {
      std::array<double, 3> xyz = JSONReaders::readTriplet(val.second);
      generator->SetXYZ(xyz[0], xyz[1], xyz[2]);
    } else if (val.first.compare("box_x1y1x2y2z") == 0) {
      std::vector<double> box_x1y1x2y2z = JSONReaders::readVector(val.second);
      if (box_x1y1x2y2z.size() != 5) {
        LOG(error) << "Number of box coordinates wrong. Expected 5 given " << box_x1y1x2y2z.size() << std::endl;
      }
      generator->SetBoxXYZ(box_x1y1x2y2z[0], box_x1y1x2y2z[1], box_x1y1x2y2z[2], box_x1y1x2y2z[3], box_x1y1x2y2z[4]);
    } else {
      LOG(warning) << "Unknown parameter: " << val.first << std::endl;
    }
  }
  std::stringstream ss;
  ss << *generator;
  LOG(info) << ss.str();

  return generator;
}

ClassImp(PndBoxGeneratorJSONReader);
