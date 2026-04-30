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

// -------------------------------------------------------------------------
// -----                      PndEventCheck source file               -----
// -----                  Created 16/09/19  by T.Stockmanns            -----
// -------------------------------------------------------------------------

#include <FairRootManager.h>
#include <FairLogger.h>

#include <iostream>
#include <algorithm>
#include <sstream>

#include "PndEventCheck.h"
#include "PndMCTrackInfo.h"
using namespace std;

// -----   Default constructor   -------------------------------------------
PndEventCheck::PndEventCheck() {}
// -------------------------------------------------------------------------
PndEventCheck::PndEventCheck(PndEventRequirements &requirements) : fEventRequirements(requirements) {}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndEventCheck::~PndEventCheck() {}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndEventCheck::Print(std::ostream &out)
{
  out << "PndEventCheck Requirements: ";
  out << fEventRequirements;
}
// -------------------------------------------------------------------------

std::vector<std::vector<int>> PndEventCheck::CheckEvent(TClonesArray *mcTrackInfo)
{
  std::vector<std::vector<int>> result;
  std::vector<PndParticleRequirements> requirements = fEventRequirements.GetRequirements();
  std::vector<int> foundTracks;
  LOG(debug) << "Check Event: ";
  for (auto &req : requirements) {
    req.ClearFound();
    std::vector<int> reqResult;
    int countFound = 0;
    LOG(debug) << "Requirement: " << req.ShortRequirementsAsString();
    for (int i = 0; i < mcTrackInfo->GetEntries(); i++) {
      PndMCTrackInfo *mcInfo = (PndMCTrackInfo *)mcTrackInfo->At(i);
      if (mcInfo->GetPID() == req.GetPdgCode() && mcInfo->GetStage() > -1 && mcInfo->GetPIDMothers() == req.GetMothers() && req.GetFound() == false &&
          std::find(foundTracks.begin(), foundTracks.end(), i) == foundTracks.end()) {
        reqResult = req.CheckTrack(*mcInfo);
        if (req.GetPdgCode() == 22 && reqResult.back() == -1) { // a photon was not found then check if you have an e+ e- pair
          LOG(debug) << "Check for electron pair: ";
          int epResult = CheckElectronPair(mcInfo, mcTrackInfo);
          LOG(debug) << "Result: " << epResult;
          reqResult.back() = epResult;
          if (epResult > 0) {
            reqResult[reqResult.size() - 2] = epResult; // if the result is positive then the not found photon has to be corrected!
            reqResult[reqResult.size() - 3] = epResult;
          }
        }
        req.Found();
        countFound++;
        foundTracks.push_back(i);

        std::stringstream ss;
        mcInfo->Print(ss);
        LOG(debug) << i << " : " << ss.str();
        PndPidProbability prob = req.GetPidProbability(*mcInfo);
        LOG(debug) << "TotalProbability " << prob.GetPidProb(req.GetPdgCode());
        LOG(debug) << std::endl;

        continue;
      }
    }
    result.push_back(reqResult);
  }

  fEventResult = result;
  return result;
}

int PndEventCheck::CheckElectronPair(PndMCTrackInfo *trackInfo, TClonesArray *mcTrackInfos)
{
  FairRootManager *runManager = FairRootManager::Instance();
  LOG(debug) << "Photon Track Info: " << *trackInfo;
  std::vector<int> daughters = trackInfo->GetDaughters();
  if (daughters.size() != 2) {
	LOG(debug) << "Not two daughters: " << daughters.size();
    return -2;
  }
  PndMCTrackInfo *e1 = (PndMCTrackInfo *)mcTrackInfos->At(daughters[0]);
  LOG(debug) << "Daughter1 : " << *e1;
  if (e1->GetPID() != 11 && e1->GetPID() != -11) {
	LOG(debug) << "Daughter 1 not electron ";
    return -3;
  }
  PndMCTrackInfo *e2 = (PndMCTrackInfo *)mcTrackInfos->At(daughters[1]);
  LOG(debug) << "Daughter2 : " << *e2;
  if (e2->GetPID() != 11 && e2->GetPID() != -11) {
	LOG(debug) << "Daughter 2 not electron ";
    return -4;
  }
  e1->FillTypeCounts();
  e2->FillTypeCounts();

  // Check if both electrons have created an EMC cluster without leaving any track information and if both of the ended in the same cluster
  if (e1->GetTypeCount(runManager->GetBranchId("PidNeutralCand")) == 1 && e2->GetTypeCount(runManager->GetBranchId("PidNeutralCand"))) {
    FairMultiLinkedData e1Cluster = e1->GetLinksWithType(runManager->GetBranchId("EmcCluster"));
    FairMultiLinkedData e2Cluster = e2->GetLinksWithType(runManager->GetBranchId("EmcCluster"));
    if (e1Cluster.GetNLinks() != 1 || e2Cluster.GetNLinks() != 1) {
      return -5; // One of the electrons was attached to more than one neutral cluster (should not happen)
    }
    FairLink link1 = e1Cluster.GetLink(0);
    FairLink link2 = e2Cluster.GetLink(0);
    if (link1 == link2) {
      return 2; // The two electrons are in the same neutral candidate
    } else
      return -6; // The two electrons are in different neutral candidates
  }

  // Check if both electrons have left enough hit points in the detector that a track could be created
  if (e1->GetTypeCount(runManager->GetBranchId("PidChargedCandPion")) > 0 || e1->GetTypeCount(runManager->GetBranchId("PidChargedCand")) > 0) {
    if (e2->GetTypeCount(runManager->GetBranchId("PidChargedCandPion")) > 0 || e2->GetTypeCount(runManager->GetBranchId("PidChargedCand")) > 0) {
      return 3; // both electrons have a reconstructed track
    }
  }
  return -7;
}

ClassImp(PndEventCheck);
