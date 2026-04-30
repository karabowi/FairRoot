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

/**
 * @class PndJSONReader
 * @brief Class to generate a PndMasterRunSim object from a configuration JSON file
 * @date 20.09.2022
 * @author Tobias Stockmanns <t.stockmanns@fz-juelich.de>
 **/

#include "PndMasterRunSim.h"
#include "PndJSONReader.h"
#include "PndBoxGeneratorJSONReader.h"
#include "PndFixStepParticleGunJSONReader.h"
#include "PndBoxGenerator.h"
#include "PndFixStepParticleGun.h"

#include "FairGenerator.h"
#include "FairLogger.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <string>
#include <set>
#include <exception>
#include <iostream>
#include <TObject.h>
#include <TRandom.h>

namespace pt = boost::property_tree;

pt::ptree PndJSONReader::getPtreeFromJSON(const std::string &fileName)
{
  pt::ptree tree;
  pt::read_json(fileName, tree);
  std::string prefix;

  return tree;
}

ClassImp(PndJSONReader);
