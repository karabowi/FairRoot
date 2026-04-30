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
 * PndTrackingQARecoInfo.cxx
 *
 *  Created on: 20.06.2023
 *      Author: tstockmanns
 */

#include "PndTrackingQARecoInfo.h"
#include "PndTrackingQAMCInfo.h"

#include "PndTrackingQABranchEnum.h"

ClassImp(PndTrackingQARecoInfo);

PndTrackingQARecoInfo::PndTrackingQARecoInfo()
{
  // TODO Auto-generated constructor stub
}

PndTrackingQARecoInfo::~PndTrackingQARecoInfo()
{
  // TODO Auto-generated destructor stub
}

Int_t PndTrackingQARecoInfo::GetTrueHits()
{
  int hits = 0;
  std::for_each(fTrueHitsCollection.begin(), fTrueHitsCollection.end(), [&](std::pair<TrackingQA::branchEnum, int> val) { hits += val.second; });
  return hits;
}
Int_t PndTrackingQARecoInfo::GetFalseHits()
{
  int hits = 0;
  std::for_each(fFalseHitsCollection.begin(), fFalseHitsCollection.end(), [&](std::pair<TrackingQA::branchEnum, int> val) { hits += val.second; });
  return hits;
}

Int_t PndTrackingQARecoInfo::GetMCHits()
{
  int hits = 0;
  std::for_each(fMCHitsCollection.begin(), fMCHitsCollection.end(), [&](std::pair<TrackingQA::branchEnum, int> val) { hits += val.second; });
  return hits;
}

Int_t PndTrackingQARecoInfo::GetMCHits(TrackingQA::branchEnum branch)
{
  return GetMCHits(TrackingQA::BranchEnumToString(branch));
}

Int_t PndTrackingQARecoInfo::GetMissingHits()
{
  int hits = 0;
  std::for_each(fMCHitsCollection.begin(), fMCHitsCollection.end(), [&](std::pair<TrackingQA::branchEnum, int> val) { hits += GetMissingHits(val.first); });
  return hits;
}

Int_t PndTrackingQARecoInfo::GetMissingHits(TrackingQA::branchEnum branch)
{
  return GetMissingHits(TrackingQA::BranchEnumToString(branch));
}

void PndTrackingQARecoInfo::SetMCTrackInfo(PndTrackingQAMCInfo *info)
{
  //    fMCTrackInfo = *info;
  fMCMomFirst = info->GetMomentumFirst();
  fMCMomLast = info->GetMomentumLast();
  fMCPosFirst = info->GetPositionFirst();
  fMCPosLast = info->GetPositionLast();
  fMCVertex = info->GetVertex();
  fMCCharge = info->GetCharge();
  fIsPrimary = info->GetIsPrimary();
  fMCPdg = info->GetPDGCode();
  fQuality = info->GetQuality();
  fMCQuality = info->GetMCQuality();

  std::map<TrackingQA::branchEnum, Int_t> mcHits = info->GetMCHits();
  for (auto hits : mcHits) {
    fMCHitsCollection[hits.first] = hits.second;
  }
  if (info->GetNofSttPoints() > 0) {
    fMCHitsCollection[TrackingQA::StringToBranchEnum("STTHitParal")] = info->GetNofSttParalPoints();
    fMCHitsCollection[TrackingQA::StringToBranchEnum("STTHitSkew")] = info->GetNofSttSkewPoints();
  }
}

double PndTrackingQARecoInfo::GetEfficiency(TrackingQA::branchEnum branch)
{
  return (GetEfficiency(TrackingQA::BranchEnumToString(branch)));
}

double PndTrackingQARecoInfo::GetPurity(TrackingQA::branchEnum branch)
{
  return (GetPurity(TrackingQA::BranchEnumToString(branch)));
}
