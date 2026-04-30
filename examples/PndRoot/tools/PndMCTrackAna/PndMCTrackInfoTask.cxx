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
// -----                PndMCTrackInfoTask source file             -----
// -------------------------------------------------------------------------

#include "PndMCTrackInfoTask.h"

#include "FairRun.h"
#include "FairRootManager.h"
#include "FairMultiLinkedData_Interface.h"

#include "PndTrack.h"
#include "PndPidCandidate.h"

#include "TObjString.h"

// -----   Default constructor   -------------------------------------------
PndMCTrackInfoTask::PndMCTrackInfoTask()
  : PndPersistencyTask("MCTrackInfo Task"), fMaxStage(3), fMaxNeutralMomDiff(0.1), fMaxStageNeutralCheck(1), fUseKalmanTracks(false), fUsePidInfo(false),
    fUseChargeCands(false)
{
  SetPersistency(kTRUE);
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMCTrackInfoTask::~PndMCTrackInfoTask() {}
// -------------------------------------------------------------------------

// -----   Initialization  of Parameter Containers -------------------------
void PndMCTrackInfoTask::SetParContainers()
{
  return;
}

InitStatus PndMCTrackInfoTask::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMCTrackInfoTask::Init()
{

  // FairRun* ana = FairRun::Instance(); //[R.K. unused]
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    std::cout << "-E- PndMCTrackInfoTask::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  TList *branchList = ioman->GetBranchNameList();

  if (fPointBranches.size() == 0) {
    // Use hits of all tracking subsystems if nothing is given
    AddPointBranchName("STTPoint");
    AddPointBranchName("MVDPoint");
    AddPointBranchName("GEMPoint");
    AddPointBranchName("FTSPoint");
  }

  AssignTClonesArrays(fPointBranches);

  if (fTrackingBranches.size() == 0) {
    AddTrackingBranchName("BarrelTrack");
    AddTrackingBranchName("FtsIdealTrack");
    fUseKalmanTracks = true;
  }

  if (fUseKalmanTracks == true) {
    for (int i = 0; i < branchList->GetEntries(); i++) {
      TObjString *branchName = (TObjString *)branchList->At(i);
      if (branchName->String().Contains("GenTrack")) {
        AddTrackingBranchName(branchName->String().Data());
      }
    }
  }

  AssignTClonesArrays(fTrackingBranches);

  if (fCandidateBranches.size() == 0) {
    AddCandidateBranchName("PidNeutralCand");
    fUseChargeCands = true;
  }

  if (fUseChargeCands == true) {
    for (int i = 0; i < branchList->GetEntries(); i++) {
      TObjString *branchName = (TObjString *)branchList->At(i);
      if (branchName->String().Contains("ChargedCand")) {
        AddCandidateBranchName(branchName->String().Data());
      }
    }
  }

  AssignTClonesArrays(fCandidateBranches);

  if (fPidBranches.size() == 0) {
    fUsePidInfo = true;
  }

  if (fUsePidInfo == true) {
    for (int i = 0; i < branchList->GetEntries(); i++) {
      TObjString *branchName = (TObjString *)branchList->At(i);
      if (branchName->String().Contains("PidAlgo")) {
        if (branchName->String().Contains("PidAlgoIdeal")) { // todo: fix bug in branch list (there are two branches for ideal case with _1, _2
          continue;
        }
        AddPidBranchName(branchName->String().Data());
      }
    }
  }

  AssignTClonesArrays(fPidBranches);

//  if (fBranches.size() == 0) {
//  }
//
//  AssignTClonesArrays(fBranches);

  fMCTracks = (TClonesArray *)ioman->GetObject("MCTrack");
  if (!fMCTracks) {
    std::cout << "-W- PndMCTrackInfoTask::Init: "
              << "No MCTrack array!" << std::endl;
    return kERROR;
  }

  fMCTrackInfo = new TClonesArray("PndMCTrackInfo");
  ioman->Register("MCTrackInfo", "MC", fMCTrackInfo, GetPersistency());

  return kSUCCESS;
}
// -------------------------------------------------------------------------

void PndMCTrackInfoTask::AssignTClonesArrays(std::map<TString, TClonesArray *> &map)
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

// -----   Public method Exec   --------------------------------------------
void PndMCTrackInfoTask::Exec(Option_t *)
{
  fTrackInfo.clear();
  fMCIdPhoton.clear();
  fMCTrackInfo->Clear();

  std::cout << "Event: " << FairRootManager::Instance()->GetEntryNr() << std::endl;

  for (int i = 0; i < fMCTracks->GetEntriesFast(); i++) {
    PndMCTrack *myTrack = (PndMCTrack *)fMCTracks->At(i);
    std::vector<int> pidMothers = GetPIDMothers(myTrack->GetMotherID());
    if (fMaxStage > -1 && (int)pidMothers.size() > fMaxStage) {
      fTrackInfo[i]; // create empty MCTrackInfo object to keep the index in-line with the MCTracks
      continue;
    }
    fTrackInfo[i].SetPIDMothers(pidMothers);
    fTrackInfo[i].SetPID(myTrack->GetPdgCode());
    if (myTrack->GetPdgCode() == 22 && fTrackInfo[i].GetStage() <= fMaxStageNeutralCheck) {
      fMCIdPhoton.push_back(i);
    }

    int motherID = myTrack->GetMotherID();
    if (motherID > -1) {
      fTrackInfo[motherID].AddDaughter(FairLink(-1, FairRootManager::Instance()->GetEntryNr(), FairRootManager::Instance()->GetBranchId("MCTrack"), i));
      fTrackInfo[motherID].AddPIDDaughter(myTrack->GetPdgCode());
      if (fTrackInfo[motherID].GetDecayTime() == 0) {
        fTrackInfo[motherID].SetDecayTime(myTrack->GetStartTime());
        fTrackInfo[motherID].SetDecayVertex(myTrack->GetStartVertex());
      } else {
        if (fTrackInfo[motherID].GetDecayTime() != myTrack->GetStartTime()) {
          //          std::cout << "-W- PndMCTrackInfoTask::Exec DecayTimes not equal: " << fTrackInfo[motherID].GetDecayTime() << " != " << myTrack->GetStartTime() << std::endl;
        }
        if (fTrackInfo[motherID].GetDecayVertex() != myTrack->GetStartVertex()) {
          //          std::cout << "-W- PndMCTrackInfoTask::Exec DecayVertices not equal: " << fTrackInfo[motherID].GetDecayVertex().x() << "/" <<
          //          fTrackInfo[motherID].GetDecayVertex().y()
          //                    << "/" << fTrackInfo[motherID].GetDecayVertex().z() << " != " << myTrack->GetStartVertex().x() << "/" << myTrack->GetStartVertex().y() << "/"
          //                    << myTrack->GetStartVertex().z() << std::endl;
        }
      }
    }
  }

  AssignPoints();
  FillPointInfo();
  AssignTrackingBranches();
  AssignCandidateBranches();
//  AssignBranches();

  FillOutputArray();

  if (fVerbose > 0)
    PrintPrimariesWithDaughters(2);

  //    if (fVerbose > 0) {
  //        std::cout << "-- Event " << FairRootManager::Instance()->GetEntryNr() << "---" << std::endl;
  //        for (int i = 0; i < fMCTracks->GetEntriesFast(); i++) {
  //            PndMCTrack *myTrack = (PndMCTrack *)fMCTracks->At(i);
  //            std::cout << i << " : " << *myTrack;
  //            std::cout << fTrackInfo[i] << std::endl;
  //        }
  //    }
}
// -------------------------------------------------------------------------

void PndMCTrackInfoTask::PrintPrimariesWithDaughters(int stage)
{
  for (int i = 0; i < fMCTracks->GetEntriesFast(); i++) {
    PndMCTrack *myTrack = (PndMCTrack *)fMCTracks->At(i);
    if (myTrack->GetMotherID() < 0) {
      std::cout << "Primary: " << i << " : " << *myTrack;
      std::cout << fTrackInfo[i] << std::endl;
      PrintDaughters(fTrackInfo[i], stage);
      std::cout << "******* End Primary *******" << std::endl << std::endl;
    }
  }
}

void PndMCTrackInfoTask::PrintDaughters(PndMCTrackInfo mother, int stage)
{
  std::vector<int> daughters = mother.GetDaughters();
  for (int i = 0; i < daughters.size(); i++) {
    PndMCTrack *daughterTrack = (PndMCTrack *)fMCTracks->At(daughters[i]);
    std::cout << "Stage: " << fTrackInfo[daughters[i]].GetStage() << " Daughter " << i << " : " << *daughterTrack;
    std::cout << fTrackInfo[daughters[i]] << std::endl;
    if (fTrackInfo[daughters[i]].GetStage() < stage) {
      PrintDaughters(fTrackInfo[daughters[i]], stage);
    }
  }
}

std::vector<int> PndMCTrackInfoTask::GetPIDMothers(int motherID)
{
  std::vector<int> result;
  if (motherID == -1)
    return result;

  result = fTrackInfo[motherID].GetPIDMothers();
  PndMCTrack *myTrack = (PndMCTrack *)fMCTracks->At(motherID);
  result.push_back(myTrack->GetPdgCode());

  return result;
}

void PndMCTrackInfoTask::AssignPoints()
{
  FairRootManager *ioman = FairRootManager::Instance();
  for (auto branch : fPointBranches) {
    for (int i = 0; i < branch.second->GetEntriesFast(); i++) {
      FairMultiLinkedData_Interface *links = (FairMultiLinkedData_Interface *)branch.second->At(i);
      std::vector<FairLink> mcLinks = links->GetSortedMCTracks();
      for (auto mclink : mcLinks) {
        fTrackInfo[mclink.GetIndex()].AddLink(
          FairLink(-1, ioman->GetEntryNr(), ioman->GetBranchId(branch.first), i)); // at the moment all MCTrackInfos get this information independent of stage
      }
    }
  }
}

void PndMCTrackInfoTask::AssignTrackingBranches()
{
  FairRootManager *ioman = FairRootManager::Instance();
  for (auto branch : fTrackingBranches) {
    //    std::cout << branch.first << " " << branch.second << std::endl;
    for (int i = 0; i < branch.second->GetEntriesFast(); i++) {
      FairMultiLinkedData_Interface *links = (FairMultiLinkedData_Interface *)branch.second->At(i);
      std::vector<FairLink> mcLinks = links->GetSortedMCTracks();
      if (mcLinks.size() > 0) {                                     // only the MCTrack with the largest weight gets a link
        fTrackInfo[mcLinks[0].GetIndex()].SetInsertHistory(kFALSE); // at the moment all MCTrackInfos get this information independent of stage
        fTrackInfo[mcLinks[0].GetIndex()].AddLink(FairLink(-1, ioman->GetEntryNr(), ioman->GetBranchId(branch.first), i));
        PndMCTrack *mcTrack = (PndMCTrack *)fMCTracks->At(mcLinks[0].GetIndex());
        PndTrack *track = (PndTrack *)branch.second->At(i);
        TVector3 momentum = track->GetParamFirst().GetMomentum();
        fTrackInfo[mcLinks[0].GetIndex()].SetRecoMomentum(FairRootManager::Instance()->GetBranchId(branch.first), momentum);
        TVector3 mcmomentum = mcTrack->GetMomentum();
        fTrackInfo[mcLinks[0].GetIndex()].SetDeltaMomentum(FairRootManager::Instance()->GetBranchId(branch.first), mcmomentum.Pt() - momentum.Pt(), mcmomentum.Z() - momentum.Z());
      }
    }
  }
}

void PndMCTrackInfoTask::AssignCandidateBranches()
{
  FairRootManager *ioman = FairRootManager::Instance();
  for (auto branch : fCandidateBranches) {
    //    std::cout << branch.first << " " << branch.second << std::endl;
    for (int i = 0; i < branch.second->GetEntriesFast(); i++) {
      FairMultiLinkedData_Interface *links = (FairMultiLinkedData_Interface *)branch.second->At(i);
      std::vector<FairLink> mcLinks = links->GetSortedMCTracks();
      if (mcLinks.size() > 0) {                                     // only the MCTrack with the largest weight gets a link
        fTrackInfo[mcLinks[0].GetIndex()].SetInsertHistory(kFALSE); // at the moment all MCTrackInfos get this information independent of stage
        fTrackInfo[mcLinks[0].GetIndex()].AddLink(FairLink(-1, ioman->GetEntryNr(), ioman->GetBranchId(branch.first), i));
        PndMCTrack *mcTrack = (PndMCTrack *)fMCTracks->At(mcLinks[0].GetIndex());
        PndPidCandidate *cand = (PndPidCandidate *)branch.second->At(i);
        TVector3 momCand = cand->GetMomentum();
        fTrackInfo[mcLinks[0].GetIndex()].SetRecoMomentum(FairRootManager::Instance()->GetBranchId(branch.first), momCand);
        TVector3 mommc = mcTrack->GetMomentum();
        fTrackInfo[mcLinks[0].GetIndex()].SetDeltaMomentum(FairRootManager::Instance()->GetBranchId(branch.first), mommc.Pt() - momCand.Pt(), mommc.Z() - momCand.Z());
        if (branch.first.Contains("ChargedCand") && fPidBranches.size() > 0) {
          AssignPidBranches(mcLinks[0].GetIndex(), i);
        }
      }
      if (branch.first.Contains("NeutralCand")) {
        AssignNeutralCand(branch.second);
      }
    }
  }
}

void PndMCTrackInfoTask::AssignNeutralCand(TClonesArray *neutralbranch)
{
  FairRootManager *ioman = FairRootManager::Instance();
  for (auto mcPhoton : fMCIdPhoton) {
    PndMCTrack *mcTrack = (PndMCTrack *)fMCTracks->At(mcPhoton);
    if (fTrackInfo[mcPhoton].GetLinksWithType(ioman->GetBranchId("PidNeutralCand")).GetNLinks() > 0) // check if you already have a neutral cand
      continue;
    if (fTrackInfo[mcPhoton].GetStage() > fMaxStageNeutralCheck)
      continue;
    //    std::cout << "PndMCTrackInfoTask::AssignNeutralCand check photon " << mcPhoton << " : " << *mcTrack << std::endl;
    TVector3 mommc(mcTrack->GetMomentum());
    for (int i = 0; i < neutralbranch->GetEntriesFast(); i++) {
      PndPidCandidate *cand = (PndPidCandidate *)neutralbranch->At(i);
      TVector3 momCand(cand->GetMomentum());
      //      std::cout << i << " : " << momCand.X() << "/" << momCand.Y() << "/" << momCand.Z() << " diff " << (mommc - momCand).Mag() / mommc.Mag() << std::endl;
      if ((mommc - momCand).Mag() / mommc.Mag() < fMaxNeutralMomDiff) {
        //        std::cout << "Match" << std::endl;
        fTrackInfo[mcPhoton].SetInsertHistory(kFALSE); // at the moment all MCTrackInfos get this information independent of stage
        fTrackInfo[mcPhoton].AddLink(FairLink(-1, ioman->GetEntryNr(), ioman->GetBranchId("PidNeutralCand"), i));
        fTrackInfo[mcPhoton].SetRecoMomentum(FairRootManager::Instance()->GetBranchId("PidNeutralCand"), momCand);
        fTrackInfo[mcPhoton].SetDeltaMomentum(FairRootManager::Instance()->GetBranchId("PidNeutralCand"), mommc.Pt() - momCand.Pt(), mommc.Z() - momCand.Z());
      }
    }
  }
}

void PndMCTrackInfoTask::AssignPidBranches(int mcIndex, int chargedCandIndex)
{
  for (auto pidBranch : fPidBranches) {
    //    std::cout << pidBranch.first << " " << pidBranch.second << std::endl;
    PndPidProbability *prob = (PndPidProbability *)pidBranch.second->At(chargedCandIndex);
    fTrackInfo[mcIndex].SetPidProbability(FairRootManager::Instance()->GetBranchId(pidBranch.first), *prob);
  }
}

// double PndMCTrackInfoTask::GetPidProbability(PndPidProbability *prob, int pdgCode)
//{
//  int absPdg = abs(pdgCode);
//  switch (absPdg) {
//  case 11: return prob->GetElectronPidProb(); break;
//  case 13: return prob->GetMuonPidProb(); break;
//  case 211: return prob->GetPionPidProb(); break;
//  case 321: return prob->GetKaonPidProb(); break;
//  case 2212: return prob->GetProtonPidProb(); break;
//  default: return -1;
//  }
//}

//void PndMCTrackInfoTask::AssignBranches()
//{
//  FairRootManager *ioman = FairRootManager::Instance();
//  for (auto branch : fBranches) {
//    //    std::cout << branch.first << " " << branch.second << std::endl;
//    for (int i = 0; i < branch.second->GetEntriesFast(); i++) {
//      FairMultiLinkedData_Interface *links = (FairMultiLinkedData_Interface *)branch.second->At(i);
//      std::vector<FairLink> mcLinks = links->GetSortedMCTracks();
//      if (mcLinks.size() > 0) {                                     // only the MCTrack with the largest weight gets a link
//        fTrackInfo[mcLinks[0].GetIndex()].SetInsertHistory(kFALSE); // at the moment all MCTrackInfos get this information independent of stage
//        fTrackInfo[mcLinks[0].GetIndex()].AddLink(FairLink(-1, ioman->GetEntryNr(), ioman->GetBranchId(branch.first), i));
//      }
//    }
//  }
//}

void PndMCTrackInfoTask::FillPointInfo()
{
  for (auto &trackInfo : fTrackInfo) {
    trackInfo.second.FillPoints();
  }
}

void PndMCTrackInfoTask::FillOutputArray()
{
  //  for (auto track : fTrackInfo) {                                                       //this is how it should be done, after the bug in the copy constructor of
  //  FairMultiLinkedData_Interface is fixed
  //    new ((*fMCTrackInfo)[fMCTrackInfo->GetEntriesFast()]) PndMCTrackInfo(track.second);
  //  }
  for (int i = 0; i < fTrackInfo.size(); i++) {
    PndMCTrackInfo *myInfo = new ((*fMCTrackInfo)[fMCTrackInfo->GetEntriesFast()]) PndMCTrackInfo(fTrackInfo[i]);
    myInfo->SetInsertHistory(kFALSE);
    myInfo->SetPointerToLinks(fTrackInfo[i].GetPointerToLinks());
  }
}

void PndMCTrackInfoTask::FinishEvent()
{

  FinishEvents();
}
// -------------------------------------------------------------------------

void PndMCTrackInfoTask::FinishTask() {}

ClassImp(PndMCTrackInfoTask);
