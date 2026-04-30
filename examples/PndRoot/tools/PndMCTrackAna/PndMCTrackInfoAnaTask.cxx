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
// -----                PndMCTrackInfoAnaTask
// -------------------------------------------------------------------------

#include "PndParticleRequirements.h"
#include <PndParticleRequirements.h>
#include "PndMCTrackInfoAnaTask.h"

#include "FairRun.h"
#include "FairRootManager.h"
#include "FairLogger.h"
#include "PndTrack.h"
#include "PndPidCandidate.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

// -----   Default constructor   -------------------------------------------
PndMCTrackInfoAnaTask ::PndMCTrackInfoAnaTask() : PndPersistencyTask("MCTrackInfo Task")
{
  SetPersistency(kTRUE);
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMCTrackInfoAnaTask ::~PndMCTrackInfoAnaTask() {}
// -------------------------------------------------------------------------

// -----   Initialization  of Parameter Containers -------------------------
void PndMCTrackInfoAnaTask ::SetParContainers()
{
  return;
}

InitStatus PndMCTrackInfoAnaTask ::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMCTrackInfoAnaTask ::Init()
{

  // FairRun* ana = FairRun::Instance(); //[R.K. unused]
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    LOG(error) << "-E- PndMCTrackInfoAnaTask::Init : "
              << "RootManager not instantiated!";
    return kFATAL;
  }

  fMCTracks = (TClonesArray *)ioman->GetObject("MCTrack");
  if (!fMCTracks) {
	LOG(error) << "-W- PndMCTrackInfoAnaTask::Init : "
              << "No MCTrack array!";
    return kERROR;
  }

  fMCTrackInfo = (TClonesArray *)ioman->GetObject("MCTrackInfo");
  if (!fMCTrackInfo) {
	LOG(error) << "-W- PndMCTrackInfoAnaTask::Init : "
              << "No MCTrackInfo array!";
    return kERROR;
  }

  SetupEventRequirements();

  InitPointOfFailure();
  InitInterestingEvents();

  return kSUCCESS;
}
// -------------------------------------------------------------------------

void PndMCTrackInfoAnaTask::AssignTClonesArrays(std::map<TString, TClonesArray *> &map)
{
  FairRootManager *ioman = FairRootManager::Instance();
  for (auto branch : map) {
    map[branch.first] = (TClonesArray *)ioman->GetObject(branch.first);
  }
  for (auto it = map.cbegin(), next_it = it; it != map.cend(); it = next_it) {
    ++next_it;
    if (it->second == nullptr) {
      map.erase(it);
    }
  }
}

void PndMCTrackInfoAnaTask::SetupEventRequirements()
{
  fEventCheck.InitRequirements();
}

// -----   Public method Exec   --------------------------------------------
void PndMCTrackInfoAnaTask::Exec(Option_t *)
{
  LOG(debug) << "Event: " << FairRootManager::Instance()->GetEntryNr();
  std::vector<std::vector<int>> result = fEventCheck.CheckEvent(fMCTrackInfo);

  int i = 0;
  for (auto reqResult : result) {
	  std::stringstream ss;
    ss << i++ << " : ";
    for (auto details : reqResult) {
      ss << details << "/";
    }
    LOG(debug) << ss.str();
  }
  LOG(debug) << std::endl;
  AnalyseResult(fEventCheck);
}
// -------------------------------------------------------------------------

void PndMCTrackInfoAnaTask::AnalyseResult(PndEventCheck &eventCheck)
{
  std::vector<std::vector<int>> result = eventCheck.GetEventResult();
  if (fReqResult.size() == 0) {
    fReqResult.resize(result.size() + 3);
  }
  int i = 0;
  bool fullyReco = true;
  bool chargedReco = true;
  bool neutralReco = true;
  std::vector<int> chargedReq = eventCheck.GetEventRequirements().GetChargedRequirements();
  std::vector<int> neutralReq = eventCheck.GetEventRequirements().GetNeutralRequirements();
  //  std::cout << "PndMCTrackInfoAnaTask::AnalyseResult size result: " << result.size() << std::endl;
  for (auto res : result) {
    if (res.size() > 0) {
      if (res.back() > 0) {
        fReqResult[i]++;
      } else {
        fullyReco = false;
        if (std::count(chargedReq.begin(), chargedReq.end(), i) == 1)
          chargedReco = false;
        if (std::count(neutralReq.begin(), neutralReq.end(), i) == 1)
          neutralReco = false;
      }
    } else {
      fullyReco = false;
      if (std::count(chargedReq.begin(), chargedReq.end(), i) == 1)
        chargedReco = false;
      if (std::count(neutralReq.begin(), neutralReq.end(), i) == 1)
        neutralReco = false;
    }

    if (GetPointOfFailure(res) > 0) {
      AddPointOfFailure(i, GetPointOfFailure(res));
      if (fInterestingEvents[i].size() < GetPointOfFailure(res))
        fInterestingEvents[i].resize(GetPointOfFailure(res) + 1);
      fInterestingEvents[i][GetPointOfFailure(res)].push_back(FairRootManager::Instance()->GetEntryNr());
    }
    i++;
  }
  if (chargedReco == true) {
    auto riter = fReqResult.rbegin();
    riter += 2;
    (*riter)++;
  }

  if (neutralReco == true) {
    auto riter = fReqResult.rbegin();
    riter++;
    (*riter)++;
  }

  if (fullyReco == true) {
    fReqResult.back()++;
  }
}

int PndMCTrackInfoAnaTask::GetPointOfFailure(std::vector<int> result)
{
  if (result.size() == 0) { // this is a bit tricky: the req 0 is the proper particle ID. This is always met if the result is filled at all
    return 0;
  }
  int i = 0;
  for (auto value : result) {
    if (value < 0) {
      return i;
    }
    i++;
  }
  return -1; // returns last value in requirements which indicates that all requirements are matched
}

void PndMCTrackInfoAnaTask::InitPointOfFailure()
{
  PndEventRequirements eventRequirements = fEventCheck.GetEventRequirements();
  std::vector<PndParticleRequirements> listReq = eventRequirements.GetRequirements();
  fPointOfFailure.resize(listReq.size());
  int i = 0;
  for (auto req : listReq) {
    fPointOfFailure[i++].resize(req.GetNRequirements());
  }
}

void PndMCTrackInfoAnaTask::InitInterestingEvents()
{
  PndEventRequirements eventRequirements = fEventCheck.GetEventRequirements();
  std::vector<PndParticleRequirements> listReq = eventRequirements.GetRequirements();
  fInterestingEvents.resize(listReq.size());
  int i = 0;
  for (auto req : listReq) {
    fInterestingEvents[i++].resize(req.GetNRequirements());
  }
}

void PndMCTrackInfoAnaTask::AddPointOfFailure(int requirement, int position)
{
  if (fPointOfFailure.size() < requirement + 1) {
    fPointOfFailure.resize(requirement + 1);
  }
  if (fPointOfFailure[requirement].size() < position + 1) {
    fPointOfFailure[requirement].resize(position + 1);
  }
  fPointOfFailure[requirement][position]++;
}

void PndMCTrackInfoAnaTask::FinishTask()
{
  double totalEntries = FairRootManager::Instance()->GetEntryNr() + 1;

  LOG(info) << "Analysis finished! Result: ";
  int i = 0;
  for (auto values : fEventCheck.GetEventRequirements().GetRequirements()) {
    LOG(info) << std::setw(2) << i << " : " << std::left << std::setw(5) << (double)fReqResult[i++] / totalEntries * 100 << " % : "
    << values.ShortRequirementsAsString();
  }
  LOG(info) << std::setw(2) << i << " : " << std::left << std::setw(5) << (double)fReqResult[i++] / totalEntries * 100 << " % : Charged";
  LOG(info) << std::setw(2) << i << " : " << std::left << std::setw(5) << (double)fReqResult[i++] / totalEntries * 100 << " % : Neutral";
  LOG(info) << std::setw(2) << i << " : " << std::left << std::setw(5) << (double)fReqResult[i++] / totalEntries * 100 << " % : Total";
  LOG(info) << std::endl;

  LOG(info) << "Point of Failure: ";
  i = 0;


  for (auto values : fPointOfFailure) {
	std::stringstream ss;
    ss << i++ << " : ";
    for (auto val : values) {
      ss << val << "/";
    }
    LOG(info) << ss.str();
  }
  LOG(info) << std::endl;

  LOG(info) << "Interesting Events: ";
  for (int particle = 0; particle < fInterestingEvents.size(); particle++) {
	for (int req = 0; req < fInterestingEvents[particle].size(); req++) {
	  if (fInterestingEvents[particle][req].size() > 0){
	    std::stringstream ss;
	    ss << particle << "/" << req << " : ";
	    for (auto event : fInterestingEvents[particle][req]) {
          ss << event << ", ";
	    }
	    LOG(info) << ss.str();
	  }
    }
  }
}

ClassImp(PndMCTrackInfoAnaTask);
