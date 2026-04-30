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
 * PndTrackingQAJSONReader.cxx
 *
 *  Created on: 20.09.2022
 *      Author: tstockmanns
 */

#include "PndTrackingQATask.h"
#include "PndTrackingQAJSONReader.h"
#include "JSONReadersHelper.h"

#include "FairRun.h"

#include <FairLogger.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>
#include <PndTrackingQATimebased.h>

FairTask *PndTrackingQAJSONReader::createTask(pt::ptree &tree)
{

  PndTrackingQATask *trackingQA = new PndTrackingQATask("", "");

  for (auto &val : tree) {
    if (val.first.front() == '_') {
      // this is a comment. So ignore it.
    } else if (val.first.compare("IdealTrackBranchName") == 0) {
      trackingQA->SetIdealTrackBranchName(val.second.data());
    } else if (val.first.compare("TrackBranchName") == 0) {
      trackingQA->SetTrackBranchName(val.second.data());
    } else if (val.first.compare("FunctorName") == 0) {
      trackingQA->SetFunctorName(val.second.data());
    } else if (val.first.compare("OutputMCInfoBranchName") == 0) {
      trackingQA->SetOutputMCInfoBranchName(val.second.data());
    } else if (val.first.compare("OutputRecoInfoBranchName") == 0) {
      trackingQA->SetOutputRecoInfoBranchName(val.second.data());
    } else if (val.first.compare("HitBranchNames") == 0) {
      pt::ptree &hitBranches = val.second;
      for (pt::ptree::value_type &hitBranch : hitBranches) {
        trackingQA->AddHitsBranchName(hitBranch.second.data().c_str());
      }
      //    } else if (val.first.compare("SaveHistosAsPictures") == 0) {
      //      trackingQA->SetSaveHistosAsPictures(val.second.get_value<bool>());
      //    } else if (val.first.compare("PicturePrefix") == 0) {
      //      trackingQA->SetPicturePrefix(val.second.data());
      //    } else if (val.first.compare("PictureFormat") == 0) {
      //      trackingQA->SetPictureFormat(val.second.data());
      //    } else if (val.first.compare("WriteCDashOutput") == 0) {
      //      trackingQA->SetWriteCDashOutput(val.second.get_value<bool>());
    } else {
      LOG(warning) << "Unknown parameter: " << val.first << std::endl;
    }
  }

  return trackingQA;
}

void PndTrackingQAJSONReader::AddTask(FairRun *run, pt::ptree &tree)
{
  std::vector<FairTask *> result;
  for (pt::ptree::value_type &trackingQA : tree) {
    FairTask *task = createTask(trackingQA.second);
    run->AddTask(task);
  }
}

ClassImp(PndTrackingQAJSONReader);
