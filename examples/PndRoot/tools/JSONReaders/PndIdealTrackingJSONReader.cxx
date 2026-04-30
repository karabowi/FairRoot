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
 * PndIdealTrackingJSONReader.cxx
 *
 *  Created on: 20.09.2022
 *      Author: tstockmanns
 */

#include "PndIdealTrackingJSONReader.h"
#include "PndIdealTrackFinder.h"
#include "JSONReadersHelper.h"

#include "FairRun.h"

#include <FairLogger.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>
#include "JSONReadersHelper.h"

FairTask *PndIdealTrackingJSONReader::createTask(pt::ptree &tree)
{

  PndIdealTrackFinder *idealTracking = new PndIdealTrackFinder();

  for (auto &val : tree) {
    if (val.first.front() == '_') {
      // this is a comment. So ignore it.
    } else if (val.first.compare("OutputBranchName") == 0) {
      idealTracking->SetOutputBranchName(val.second.data());
    } else if (val.first.compare("TrackSelector") == 0) {
      idealTracking->SetTrackSelector(val.second.data());
    } else if (val.first.compare("HitBranchName") == 0) {
      pt::ptree &hitBranches = val.second;
      for (pt::ptree::value_type &hitBranch : hitBranches) {
        idealTracking->AddBranchName(hitBranch.second.data().c_str());
      }
    } else if (val.first.compare("MomentumSmearing") == 0) {
      {
        std::array<double, 3> params = JSONReaders::readTriplet(val.second);
        idealTracking->SetMomentumSmearing(params[0], params[1], params[2]);
      }
    } else if (val.first.compare("RelativeMomentumSmearing") == 0) {
      idealTracking->SetRelativeMomentumSmearing(val.second.get_value<double>());
    } else if (val.first.compare("VertexSmearing") == 0) {
      {
        std::array<double, 3> params = JSONReaders::readTriplet(val.second);
        idealTracking->SetVertexSmearing(params[0], params[1], params[2]);
      }
    } else if (val.first.compare("TrackingEfficiency") == 0) {
      idealTracking->SetTrackingEfficiency(val.second.get_value<double>());
    } else {
      LOG(warning) << "Unknown parameter: " << val.first;
    }
  }
  return idealTracking;
}

void PndIdealTrackingJSONReader::AddTask(FairRun *run, pt::ptree &tree)
{
  std::vector<FairTask *> result;
  for (pt::ptree::value_type &trackingQA : tree) {
    FairTask *task = createTask(trackingQA.second);
    run->AddTask(task);
  }
}

ClassImp(PndIdealTrackingJSONReader);
