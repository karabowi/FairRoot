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
// -----                      PndParticleRequirements source file               -----
// -----                  Created 16/09/19  by T.Stockmanns            -----
// -------------------------------------------------------------------------

#include <FairRootManager.h>
#include "FairLogger.h"
#include <PndParticleRequirements.h>
#include "PndStringSeparator.h"

#include "TDatabasePDG.h"

#include <sstream>

using namespace std;

// -----   Default constructor   -------------------------------------------
PndParticleRequirements::PndParticleRequirements()
  : fMinDecayTime(), fMaxDecayTime(), fStage(-1), fPID(-1), fFound(false), fCheckTracking(false), fCheckBranchChargedCand(-1), fCheckNeutral(false), fCheckPoints(false),
    fMinTrackingPoints(4), fMinPidProb(100.), fCheckPid(false)
{
  fMomentumResolution.push_back(TVector2(0.5, 0.5)); // TrackFinding
  fMomentumResolution.push_back(TVector2(0.1, 0.1)); // Kalman
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndParticleRequirements::~PndParticleRequirements() {}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndParticleRequirements::Print(std::ostream &out)
{
  out << "Required is a particle of type: " << fPID;
  if (fPIDMothers.size() > 0) {
	std::stringstream ss;
    ss << " from Mothers: ";
    for (auto mpid : fPIDMothers) {
      ss << mpid << " : ";
    }
    out << ss.str() << std::endl;
  }

  if (fPIDDaughters.size() > 0) {
	std::stringstream ss;
    ss << " with Daughters: ";
    for (auto mpid : fPIDDaughters) {
      ss << mpid << " : ";
    }
    out << ss.str() << std::endl;
  }

  if (fMinDecayTime > 0 || fMaxDecayTime > 0) {
    out << "DecayTime: " << fMinDecayTime << " < " << fMaxDecayTime;
  }

  if (fStage > -1)
    out << "Stage: " << fStage << std::endl;

  if (fCheckTracking == true) {
	out << "Check Tracking:";
	out << "Minimum tracking points: " << fMinTrackingPoints;
    int i = 0;
    for (auto branchType : fCheckBranchesBarrel) {
      out << FairRootManager::Instance()->GetBranchName(branchType) << " alternative " << FairRootManager::Instance()->GetBranchName(fCheckBranchesForward[i])
                << " resolution: " << fMomentumResolution[i].X() << "/" << fMomentumResolution[i].Y() << std::endl;
      i++;
    }
    out << FairRootManager::Instance()->GetBranchName(fCheckBranchChargedCand) << " resolution: " << fMomentumResolution[1].X() << "/" << fMomentumResolution[1].Y();
  }
  if (fTypeCount.size() > 0)
	out << "Links Required: " << std::endl;
  for (auto type : fTypeCount) {
	out << FairRootManager::Instance()->GetBranchName(type.first) << " : " << type.second << std::endl;
  }

  if (fMomentumDifference.size() > 0) {
	out << "Max Delta Momentum (pt, pl):" << std::endl;
    for (auto deltaMom : fMomentumDifference) {
      out << deltaMom.first << " : " << deltaMom.second.X() << "/" << deltaMom.second.Y() << " GeV/c " << std::endl;
    }
  }
  if (fPidBranches.size() > 0) {
	out << "PID algos: " << std::endl;
	std::stringstream ss;
    for (auto algo : fPidBranches) {
      ss << FairRootManager::Instance()->GetBranchName(algo) << " ";
    }
    ss << "> " << fMinPidProb;
    out << ss.str() << std::endl;
  }
}

int PndParticleRequirements::GetNRequirements()
{
  int req = 4; // PID/Mothers/Daughters/Stage
  if (fCheckTracking == true) {
    req++; // Enough points
    req += fCheckBranchesBarrel.size() * 2;
    if (fCheckBranchChargedCand > 0)
      req += 2;
  }
  if (fCheckNeutral == true) {
    req += 2;
  }
  if (fCheckPid == true) {
    req += 1;
  }
  return req;
}

void PndParticleRequirements::SetCheckTracking(TString barrelPrefix, TString forwardPrefix, TString particlePropagatorName)
{
  fBarrelTrackerPrefix = barrelPrefix;
  fForwardTrackerPrefix = forwardPrefix;
  fPropagatorParticle = particlePropagatorName;

  fCheckTracking = true;
  fCheckPoints = true;
}

void PndParticleRequirements::InitTracking()
{
  FairRootManager *ioman = FairRootManager::Instance();
  TString barrelTrack = fBarrelTrackerPrefix + "Track";
  TString barrelKalman = fBarrelTrackerPrefix + "GenTrack" + fPropagatorParticle;
  TString forwardTrack = fForwardTrackerPrefix + "Track";
  TString forwardKalman = fForwardTrackerPrefix + "GenTrack" + fPropagatorParticle;
  TString pidName = "PidChargedCand" + fPropagatorParticle;

  fCheckBranchesBarrel.push_back(ioman->GetBranchId(barrelTrack));
  if (ioman->GetBranchId(barrelTrack) < 0) {
    LOG(error) << "PndParticleRequirements::InitTracking: " << barrelTrack << " not found in list of branches: " << ioman->GetBranchId(barrelTrack);
  } else {
	  LOG(debug) << "PndParticleRequirements::InitTracking: BarrelTrack " << barrelTrack << " : " << ioman->GetBranchId(barrelTrack);
  }
  fCheckBranchesBarrel.push_back(ioman->GetBranchId(barrelKalman));
  if (ioman->GetBranchId(barrelKalman) < 0) {
	LOG(error) << "PndParticleRequirements::InitTracking: " << barrelKalman << " not found in list of branches: " << ioman->GetBranchId(barrelKalman);
  } else {
	  LOG(debug) << "PndParticleRequirements::InitTracking: BarrelKalman " << barrelKalman << " : " << ioman->GetBranchId(barrelKalman);
  }

  fCheckBranchesForward.push_back(ioman->GetBranchId(forwardTrack));
  if (ioman->GetBranchId(forwardTrack) < 0) {
	LOG(error) << "PndParticleRequirements::InitTracking: " << forwardTrack << " not found in list of branches: " << ioman->GetBranchId(forwardTrack);
  } else {
	  LOG(debug) << "PndParticleRequirements::InitTracking: ForwardTrack " << forwardTrack << " : " << ioman->GetBranchId(forwardTrack);
  }

  fCheckBranchesForward.push_back(ioman->GetBranchId(forwardKalman));
  if (ioman->GetBranchId(forwardKalman) < 0) {
	LOG(error) << "PndParticleRequirements::InitTracking: " << forwardKalman << " not found in list of branches: " << ioman->GetBranchId(forwardKalman);
  } else {
	  LOG(debug) << "PndParticleRequirements::InitTracking: ForwardKalman " << forwardKalman << " : " << ioman->GetBranchId(forwardKalman);
  }

  fCheckBranchChargedCand = ioman->GetBranchId(pidName);
  if (ioman->GetBranchId(pidName) < 0) {
	LOG(warning) << "PndParticleRequirements::InitTracking: " << pidName << " not found in list of branches: " << ioman->GetBranchId(pidName);
  } else {
	  LOG(debug) << "PndParticleRequirements::InitTracking: ChargeCand " << pidName << " : " << ioman->GetBranchId(pidName);
  }
}

std::vector<int> PndParticleRequirements::CheckTracking(PndMCTrackInfo &info)
{

  std::vector<int> result;
  info.FillTypeCounts();
  FairRootManager *ioman = FairRootManager::Instance();

  int enoughPoints = 0;

  if (fCheckPoints == true) {
    if (info.GetTotalPoints() > fMinTrackingPoints)
      enoughPoints = 1;
    else
      enoughPoints = -1;
  }
  result.push_back(enoughPoints);

  bool barrelTrackFound = false;
  int i = 0;

  for (auto branchType : fCheckBranchesBarrel) {
    if (info.GetTypeCount(branchType) < 0) {
      result.push_back(0); // type does not exist
      result.push_back(0); // no momentum comparison
    } else if (info.GetTypeCount(branchType) == 1) {
      result.push_back(1); // type exist
      if (info.GetDeltaMomentum(branchType).X() < fMomentumResolution[i].X() && info.GetDeltaMomentum(branchType).Y() < fMomentumResolution[i].Y())
        result.push_back(1); // momentum matches
      else
        result.push_back(-1); // does not match
      barrelTrackFound = true;
    } else {
      result.push_back(-1); // type count larger than one???
      result.push_back(-1);
    }
  }
  i = 0;
  if (barrelTrackFound == false) {
    result.clear();
    result.push_back(enoughPoints);
    for (auto branchType : fCheckBranchesForward) {
      if (info.GetTypeCount(branchType) < 0) {
        result.push_back(0);
        result.push_back(0);
      } else if (info.GetTypeCount(branchType) == 1) {
        result.push_back(2);
        if (info.GetDeltaMomentum(branchType).X() < fMomentumResolution[++i].X() && info.GetDeltaMomentum(branchType).Y() < fMomentumResolution[i].Y())
          result.push_back(2);
        else
          result.push_back(-1);
      } else {
        result.push_back(-1);
        result.push_back(-1);
      }
    }
  }
  if (fCheckBranchChargedCand > 0){
	  if (info.GetTypeCount(fCheckBranchChargedCand) > 0) {
		result.push_back(1);
		if (info.GetDeltaMomentum(fCheckBranchChargedCand).X() < fMomentumResolution[1].X() && info.GetDeltaMomentum(fCheckBranchChargedCand).Y() < fMomentumResolution[1].Y())
		  result.push_back(1);
		else
		  result.push_back(-1);
	  } else {
		result.push_back(-1); // no PidChargedCand
		result.push_back(-1); // therefore no matching momentum resolution
	  }
  } else {
	  result.push_back(0); // no ChargedCand branches
	  result.push_back(0); // therefore no matching momentum resolution
  }
  return result;
}

void PndParticleRequirements::SetCheckNeutral()
{
  fCheckNeutral = true;
}

void PndParticleRequirements::InitNeutral()
{
  int neutralCandType = FairRootManager::Instance()->GetBranchId("PidNeutralCand");
  AddTypeRequirements(neutralCandType, 1);
  AddMaxMomentumDifference(neutralCandType, TVector2(0.3, 0.3));
}

void PndParticleRequirements::InitPid()
{
  PndStringSeparator sep(fPidAlgorithms.Data(), ";");
  std::vector<std::string> algorithmNames = sep.GetStringVector();
  for (auto algo : algorithmNames) {
    int branchId = FairRootManager::Instance()->GetBranchId(algo.c_str());
    if (branchId < 0) {
      LOG(error) << " PndParticleRequirements::InitPid " << algo << " not a valid branch name!";
      return;
    }
    fPidBranches.push_back(branchId);
  }
}

void PndParticleRequirements::Init()
{
  if (fCheckTracking == true)
    InitTracking();
  if (fCheckNeutral == true)
    InitNeutral();
  if (fCheckPid == true)
    InitPid();
}

std::string PndParticleRequirements::ShortRequirementsAsString()
{

  std::stringstream ss;
  for (auto mother : fPIDMothers) {
    ss << mother << ":";
  }
  ss << fPID << " : ";
  ss << " PID/Mothers/Daughters/Stage/";
  if (fCheckTracking == true) {
    ss << "Points/";
    for (auto type : fCheckBranchesBarrel) {
      ss << FairRootManager::Instance()->GetBranchName(type) << "/"
                << "MomRes/";
    }
    ss << "ChCand/MomRes/";
  }
  for (auto type : fTypeCount) {
    ss << FairRootManager::Instance()->GetBranchName(type.first) << "=" << type.second << "/";
  }
  for (auto type : fMomentumDifference) {
    ss << FairRootManager::Instance()->GetBranchName(type.first) << "=" << type.second.X() << " " << type.second.Y() << "/";
  }
  if (fCheckPid == true) {
    ss << "Pid/";
  }
  ss << "Total";
  return ss.str();
}

std::vector<int> PndParticleRequirements::CheckTrack(PndMCTrackInfo &info)
{
  std::vector<int> result;
  result.push_back(1); // check of PID has to happen before;
  int val = CheckMothers(info.GetPIDMothers());
  result.push_back(val);

  if (fPIDDaughters.size() > 0) {
    val = CheckDaughters(info.GetPIDDaughters());
    result.push_back(val);
  } else
    result.push_back(0);

  if (fStage > -1) {
    val = CheckStage(info.GetStage());
    result.push_back(val);
  } else
    result.push_back(0);

  if (fCheckTracking > 0) {
    std::vector<int> type = CheckTracking(info);
    result.insert(result.end(), type.begin(), type.end());
  }

  if (fTypeCount.size() > 0) {
    std::vector<int> type = CheckTypeCount(info);
    result.insert(result.end(), type.begin(), type.end());
  }
  if (fMomentumDifference.size() > 0) {
    std::vector<int> mom = CheckMomentum(info);
    result.insert(result.end(), mom.begin(), mom.end());
  }

  if (fPidBranches.size() > 0) {
    result.push_back(CheckPid(info));
  }

  result.push_back(CheckOverall(result));
  if (result.back() == -1) {
	std::stringstream ss;
    LOG(debug) << "Particle not fully found: ";
    for (auto res : result) {
      ss << res << "/";
    }
    LOG(debug) << ss.str();
    LOG(debug) << info;
    if (fCheckPid == true)
      LOG(debug) << "OverallPIDProb: " << GetPidProbability(info).GetPidProb(info.GetPID()) << " required: " << fMinPidProb;
    LOG(debug) << std::endl;
  }
  return result;
}

int PndParticleRequirements::CheckMothers(std::vector<int> mothers)
{
  return mothers == fPIDMothers;
}

int PndParticleRequirements::CheckDaughters(std::vector<int> daughters)
{
  return daughters == fPIDDaughters;
}

int PndParticleRequirements::CheckStage(int stage)
{
  return stage == fStage;
}

std::vector<int> PndParticleRequirements::CheckTypeCount(PndMCTrackInfo &info)
{
  info.FillTypeCounts();
  std::vector<int> result;
  for (auto type : fTypeCount) {
    if (info.GetTypeCount(type.first) < 0) {
      result.push_back(-1);
    } else if (info.GetTypeCount(type.first) == type.second)
      result.push_back(1);
    else
      result.push_back(-1);
  }
  return result;
}
std::vector<int> PndParticleRequirements::CheckMomentum(PndMCTrackInfo &info)
{
  std::vector<int> result;
  for (auto momdiff : fMomentumDifference) {
    TVector2 measuredMomDiff = info.GetDeltaMomentum(momdiff.first);
    if (measuredMomDiff.X() == 0 && measuredMomDiff.Y() == 0)
      result.push_back(-1);
    else if (TMath::Abs(measuredMomDiff.X()) < momdiff.second.X() && TMath::Abs(measuredMomDiff.Y()) < momdiff.second.Y())
      result.push_back(1);
    else
      result.push_back(-1);
  }
  return result;
}

int PndParticleRequirements::CheckPid(PndMCTrackInfo &info)
{
  int result = -1;
  PndPidProbability prob = GetPidProbability(info);

  if (prob.GetPidProb(GetPdgCode()) > fMinPidProb)
    return 1;
  else
    return -1;
}

PndPidProbability PndParticleRequirements::GetPidProbability(PndMCTrackInfo &info)
{
  PndPidProbability prob;
  if (fPidBranches.size() == 0) {
    LOG(error) << "PndParticleRequirements::CheckPid no pidBranches";
    return prob;
  }
  for (auto branchId : fPidBranches) {
    prob *= info.GetPidProbability(branchId);
  }
  return prob;
}

int PndParticleRequirements::CheckOverall(std::vector<int> results)
{
  for (auto result : results) {
    if (result < 0)
      return -1;
  }
  return 1;
}
// -------------------------------------------------------------------------

ClassImp(PndParticleRequirements);
