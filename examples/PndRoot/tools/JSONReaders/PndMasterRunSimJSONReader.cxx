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
 * @class PndMasterRunSimJSONReader
 * @brief Class to generate a PndMasterRunSim object from a configuration JSON file
 * @date 20.09.2022
 * @author Tobias Stockmanns <t.stockmanns@fz-juelich.de>
 **/

#include "PndMasterRunSim.h"
#include "PndMasterRunSimJSONReader.h"
#include "PndBoxGeneratorJSONReader.h"
#include "PndFixStepParticleGunJSONReader.h"
#include "PndBoxGenerator.h"
#include "PndFixStepParticleGun.h"

#include "FairGenerator.h"
#include "FairPrimaryGenerator.h"
#include "FairLogger.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <string>
#include <set>
#include <exception>
#include <iostream>
#include <TObject.h>
#include <TRandom.h>

PndMasterRunSim *PndMasterRunSimJSONReader::createRunSim(pt::ptree &tree)
{

  std::string prefix;

  PndMasterRunSim *run = new PndMasterRunSim();

  for (pt::ptree::value_type &property : tree.get_child("simOptions")) {
    LOG(info) << property.first.data() << " : " << property.second.data();
    if (property.first.front() == '_') {
      // this is a comment. So ignore it.
    } else if (property.first.compare("prefix") == 0) {
      prefix = property.second.get_value<std::string>();
    } else if (property.first.compare("seed") == 0) {
      gRandom->SetSeed(property.second.get_value<int>());
    } else if (property.first.compare("nEvents") == 0) {
      fNEvents = property.second.get_value<int>();
      run->SetNumberOfEvents(fNEvents);
    } else if (property.first.compare("simEngine") == 0) {
      run->SetName(property.second.data().c_str());
    } else if (property.first.compare("parAsciiFile") == 0) {
      run->SetParamAsciiFile(property.second.data());
    } else if (property.first.compare("parRootFile") == 0) {
      run->SetParamRootFile(property.second.data());
    } else if (property.first.compare("beamMomentum") == 0) {
      run->SetBeamMom(std::stod(property.second.data()));
    } else if (property.first.compare("targetMode") == 0) {
      run->SetTargetMode(property.second.get_value<int>());
    } else if (property.first.compare("storeTrajectories") == 0) {
      if (property.second.data().compare("true") == 0)
        run->SetStoreTraj(true);
      else
        run->SetStoreTraj(false);
    } else if (property.first.compare("options") == 0) {
      run->SetOptions(property.second.data());

    } else if (property.first.compare("boxGenerators") == 0) {
      pt::ptree &boxGenerators = property.second;
      for (pt::ptree::value_type &generator : boxGenerators) {
        PndBoxGenerator *boxGen = PndBoxGeneratorJSONReader::readJSON(generator.second);
        run->AddGenerator(dynamic_cast<FairGenerator *>(boxGen));
      }
    } else if (property.first.compare("boxGenerator") == 0) {
      PndBoxGenerator *boxGen = PndBoxGeneratorJSONReader::readJSON(property.second);
      run->AddGenerator(dynamic_cast<FairGenerator *>(boxGen));
    } else if (property.first.compare("fixStepGenerators") == 0) {
      pt::ptree &generators = property.second;
      for (pt::ptree::value_type &generator : generators) {
        PndFixStepParticleGun *gen = PndFixStepParticleGunJSONReader::readJSON(generator.second);
        run->AddGenerator(dynamic_cast<FairGenerator *>(gen));
      }
    } else if (property.first.compare("fixStepGenerator") == 0) {
      PndFixStepParticleGun *gen = PndFixStepParticleGunJSONReader::readJSON(property.second);
      run->AddGenerator(dynamic_cast<FairGenerator *>(gen));
    } else if (property.first.compare("ftfGenerator") == 0) {
      pt::ptree &options = property.second;
      std::string fileName;
      int flag = -1;
      for (pt::ptree::value_type &option : options) {
        if (option.first.compare("ftfRootFile") == 0) {
          fileName = option.second.data();
        } else if (option.first.compare("ftfFlag") == 0) {
          flag = option.second.get_value<int>();
        }
      }
      if (flag > -1)
        run->SetFtfFlag(flag);
      //      run->UseFtfGenerator(fileName);
      run->SetInput("ftf");

    } else if (property.first.compare("dpmGenerator") == 0) {
      pt::ptree &options = property.second;
      std::string fileName;
      int flag = -1;
      for (pt::ptree::value_type &option : options) {
        if (option.first.compare("dpmFlag") == 0) {
          flag = option.second.get_value<int>();
        }
      }
      if (flag > -1)
        run->SetDpmFlag(flag);
      run->UseDpmGenerator();

    } else if (property.first.compare("evtGenerator") == 0) {
      pt::ptree &options = property.second;
      std::string decFileName;
      for (pt::ptree::value_type &option : options) {
        if (option.first.compare("decFileName") == 0) {
          decFileName = option.second.data();
        }
        run->SetInput(decFileName.c_str());
      }
    } else {
      LOG(warning) << "Option not known " << property.first.data();
    }
  }
  run->Setup(prefix.c_str());
  return run;
}

ClassImp(PndMasterRunSimJSONReader);
