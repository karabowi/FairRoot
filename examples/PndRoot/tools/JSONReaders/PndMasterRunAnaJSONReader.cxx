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
 * @class PndMasterRunAnaJSONReader
 * @brief Class to generate a PndMasterRunAna object from a configuration JSON file
 * @date 20.09.2022
 * @author Tobias Stockmanns <t.stockmanns@fz-juelich.de>
 **/

#include "PndMasterRunAna.h"
#include "PndMasterRunAnaJSONReader.h"
#include "PndBoxGeneratorJSONReader.h"
#include "PndBoxGenerator.h"

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

PndMasterRunAna *PndMasterRunAnaJSONReader::createRunAna(pt::ptree &tree, const std::string &extPrefix)
{
  std::string prefix = extPrefix;

  PndMasterRunAna *run = new PndMasterRunAna();

  for (pt::ptree::value_type &property : tree.get_child("anaOptions")) {
    LOG(info) << property.first.data() << " : " << property.second.data();
    if (property.first.compare("prefix") == 0) {
      prefix = property.second.get_value<std::string>();
    } else if (property.first.compare("seed") == 0) {
      gRandom->SetSeed(property.second.get_value<int>());
    } else if (property.first.compare("nEvents") == 0) {
      fNEvents = property.second.get_value<int>();
    } else if (property.first.compare("parAsciiFile") == 0) {
      run->SetParamAsciiFile(property.second.data());
    } else if (property.first.compare("parRootFile") == 0) {
      run->SetParamRootFile(property.second.data());
    } else if (property.first.compare("outputFile") == 0) {
      run->SetOutput(property.second.data());
    } else if (property.first.compare("options") == 0) {
      run->SetOptions(property.second.data());
    } else if (property.first.compare("useFairLinks") == 0) {
      run->UseFairLinks(property.second.get_value<bool>());
    } else if (property.first.compare("generateRunInfo") == 0) {
      run->GenerateRunInfo(property.second.get_value<bool>());
    } else if (property.first.compare("setEventCounterRate") == 0) {
      run->SetEventCounterRate(property.second.get_value<int>());
    } else if (property.first.compare("inputFiles") == 0) {
      pt::ptree &files = property.second;
      for (pt::ptree::value_type &file : files) {
        run->AddFriend(file.second.data().c_str());
      }
    } else {
      LOG(warning) << "Option not known " << property.first.data();
    }
  }
  run->Setup(prefix.c_str());
  return run;
}

ClassImp(PndMasterRunAnaJSONReader);
