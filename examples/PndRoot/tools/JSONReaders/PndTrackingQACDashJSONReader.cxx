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
 * PndTrackingQACDashJSONReader.cxx
 *
 *  Created on: 16.05.2023
 *      Author: tstockmanns
 */

#include <PndTrackingQACDashJSONReader.h>
#include "JSONReadersHelper.h"

ClassImp(PndTrackingQACDashJSONReader);

PndTrackingQACDashJSONReader::PndTrackingQACDashJSONReader()
{
  // TODO Auto-generated constructor stub
}

PndTrackingQACDashJSONReader::~PndTrackingQACDashJSONReader()
{
  // TODO Auto-generated destructor stub
}

PndTrackingQACDash PndTrackingQACDashJSONReader::createTrackingQACDash(pt::ptree &tree)
{

  PndTrackingQACDash result;
  auto child = tree.get_child("CDashTrackingQA");
  for (pt::ptree::value_type &val : tree.get_child("CDashTrackingQA")) {
    if (val.first.front() == '_') {
      // this is a comment. So ignore it.
    } else if (val.first.compare("QAFileName") == 0) {
      result.SetQAFileName(val.second.data());
    } else if (val.first.compare("PicturePrefix") == 0) {
      result.SetPicturePrefix(val.second.data());
    } else if (val.first.compare("Workdir") == 0) {
      result.SetWorkdir(val.second.data());
    }
    //		  else if (val.first.compare("CDashMeasurementBoundaries") == 0) {
    //		  PndMeasurementBoundariesJSONReader mbReader;
    //		  auto mb = mbReader.createMeasurementBoundaries(val);
    //		  result.SetMeasurementBoundaries(mb);
    //		}
  }
  PndMeasurementBoundariesJSONReader mbReader;
  auto mb = mbReader.createMeasurementBoundaries(child);
  result.SetMeasurementBoundaries(mb);

  return result;
}
