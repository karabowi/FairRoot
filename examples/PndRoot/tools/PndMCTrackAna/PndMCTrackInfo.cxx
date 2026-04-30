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
// -----                      PndMCTrackInfo source file               -----
// -----                  Created 16/09/19  by T.Stockmanns            -----
// -------------------------------------------------------------------------

#include <FairRootManager.h>

#include <iostream>

#include "PndMCTrackInfo.h"
using namespace std;

// -----   Default constructor   -------------------------------------------
PndMCTrackInfo::PndMCTrackInfo()
  : FairMultiLinkedData_Interface(), fDecayVertex(), fDecayTime(), fStage(-1), fMVDPoints(0), fSTTPoints(0), fGEMPoints(0), fFTSPoints(0), fTotalPoints(0), fPID(0)
{
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMCTrackInfo::~PndMCTrackInfo() {}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndMCTrackInfo::Print(std::ostream &out)
{
  if (fStage > -1) {
    out << "PID: " << fPID << std::endl;
    out << "DecayTime: " << fDecayTime << " DecayVertex: " << fDecayVertex.x() << "/" << fDecayVertex.y() << "/" << fDecayVertex.z() << endl;
    std::vector<int> motherPID = GetPIDMothers();
    if (motherPID.size() > 0) {
      out << "Mothers: ";
      for (auto mpid : motherPID) {
        out << mpid << " : ";
      }
      out << std::endl;
    }

    std::vector<int> daughterPID = GetPIDDaughters();
    if (daughterPID.size() > 0) {
      out << "Daughter PIDs: ";
      for (auto dpid : daughterPID)
        out << dpid << " ";
      out << std::endl;
    }
    out << "Links to Daughters, Points, etc. : " << *((FairMultiLinkedData_Interface *)this) << std::endl;

    out << "TotalPoints(Total/MVD/STT/GEM/FTS) : " << GetTotalPoints() << " = " << GetMVDPoints() << " : " << GetSTTPoints() << " : " << GetGEMPoints() << " : "
              << GetFTSPoints() << std::endl;

    FillTypeCounts();
    if (fTypeCount.size() > 0) {
      out << "Link types: " << std::endl;
      for (auto type : fTypeCount) {
        out << type.first << " : " << FairRootManager::Instance()->GetBranchName(type.first) << " : " << type.second << std::endl;
      }
    }
    if (fMomentumReco.size() > 0) {
      out << "Reco Momentum: " << std::endl;
      for (auto recoMom : fMomentumReco) {
        out << FairRootManager::Instance()->GetBranchName(recoMom.first) << " : " << recoMom.second.X() << "/" << recoMom.second.Y() << "/" << recoMom.second.Z() << " GeV/c "
                  << std::endl;
      }
    }
    if (fMomentumDifference.size() > 0) {
      out << "Delta Momentum (pt, pl):" << std::endl;
      for (auto deltaMom : fMomentumDifference) {
        out << FairRootManager::Instance()->GetBranchName(deltaMom.first) << " : " << deltaMom.second.X() << "/" << deltaMom.second.Y() << " GeV/c " << std::endl;
      }
    }
    if (fPidProb.size() > 0) {
      out << "PidProbabilities for PID " << GetPID() << std::endl;
      for (auto pid : fPidProb) {
        out << FairRootManager::Instance()->GetBranchName(pid.first) << " : " << pid.second.GetPidProb(GetPID()) << " %" << std::endl;
      }
    }
  } else {
    out << "Stage of particle above maxStage" << std::endl;
  }
}
// -------------------------------------------------------------------------

void PndMCTrackInfo::AddDaughter(FairLink daughter)
{
  SetInsertHistory(kFALSE);
  AddLink(daughter);
  fDaughters.push_back(daughter.GetIndex());
}

std::vector<int> PndMCTrackInfo::GetDaughters()
{
  return fDaughters;
}

Int_t PndMCTrackInfo::GetNHits(TString branchName)
{
  return GetLinksWithType(FairRootManager::Instance()->GetBranchId(branchName)).GetNLinks();
}

void PndMCTrackInfo::FillPoints()
{
  fMVDPoints = GetNHits("MVDPoint");
  fSTTPoints = GetNHits("STTPoint");
  fGEMPoints = GetNHits("GEMPoint");
  fFTSPoints = GetNHits("FTSPoint");
  fTotalPoints = fMVDPoints + fSTTPoints + fGEMPoints + fFTSPoints;
}

void PndMCTrackInfo::FillTypeCounts()
{
  fTypeCount.clear();
  if (fTypeCount.size() == 0) {
    std::set<FairLink> links = GetLinks();
    for (auto link : links) {
      fTypeCount[link.GetType()]++;
    }
  }
}

ClassImp(PndMCTrackInfo);
