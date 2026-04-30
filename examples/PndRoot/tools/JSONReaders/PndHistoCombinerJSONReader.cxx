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
 * PndHistoCombinerJSONReader.cxx
 *
 *  Created on: 20.09.2022
 *      Author: tstockmanns
 */

#include "PndHistoCombinerJSONReader.h"
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

PndHistoCombiner *PndHistoCombinerJSONReader::readJSON(pt::ptree &tree)
{

  PndHistoCombiner *combiner = new PndHistoCombiner();
  for (pt::ptree::value_type &val : tree.get_child("histoCombinerOptions")) {
      if (val.first.front() == '_') {
        // this is a comment. So ignore it.
      } else if(val.first.compare("PicPerCan") == 0){
    	  combiner->SetPicPerCan(val.second.get_value<int>());
      }
      else if (val.first.compare("CanvasPrefix") == 0){
    	  combiner->SetCanvasPrefix(val.second.get_value<std::string>().c_str());
      } else if (val.first.compare("CanvasOutputDir") == 0){
    	  combiner->SetCanvasOutputDir(val.second.data());
      } else if (val.first.compare("CanvasOutputFormat") == 0){
    	  combiner->SetCanvasOutputFormat(val.second.data());
      } else if (val.first.compare("CanvasDimensions") == 0){
    	  std::pair<double, double> dim = JSONReaders::readPair(val.second);
    	  combiner->SetDimensions(dim.first, dim.second);
      } else if (val.first.compare("Pulls") == 0){
    	  combiner->SetPulls(val.second.get_value<bool>());
      } else if (val.first.compare("CDashOutput") == 0){
    	  combiner->SetCDashOutput(val.second.get_value<bool>());
      } else if (val.first.compare("FileNames") == 0){
    	  pt::ptree &fileNames = val.second;
    	  for (pt::ptree::value_type &file : fileNames) {
    		  std::pair<std::string, std::string> names = JSONReaders::readStringPair(file.second);
    		  combiner->AddFileNamesAndShortName(names.first.c_str(), names.second.c_str());
    	  }

      } else if (val.first.compare("histoThresholds") == 0){
    	  pt::ptree &threshvals = val.second;
    	  for (pt::ptree::value_type &threshval : threshvals) {
    		  TString histoName = threshval.first;
    		  std::array<double, 3> values = JSONReaders::readTriplet(threshval.second);
    		  combiner->AddHistoThresholds(histoName, values[0], values[1], values[2]);
    	  }
      } else {
          LOG(warning) << "Unknown parameter: " << val.first << std::endl;
        }
  }
  return combiner;
}

ClassImp(PndHistoCombinerJSONReader);
