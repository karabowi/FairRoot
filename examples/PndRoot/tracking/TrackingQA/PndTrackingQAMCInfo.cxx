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
 * PndTrackingQAMCInfo.cxx
 *
 *  Created on: 20.06.2023
 *      Author: tstockmanns
 */

#include "PndTrackingQAMCInfo.h"
#include "PndTrackingQARecoInfo.h"

#include "FairRootManager.h"

#include <functional>

ClassImp(PndTrackingQAMCInfo);

PndTrackingQAMCInfo::PndTrackingQAMCInfo()
{
  // TODO Auto-generated constructor stub
}

PndTrackingQAMCInfo::~PndTrackingQAMCInfo()
{
  // TODO Auto-generated destructor stub
}

void PndTrackingQAMCInfo::FindBestRecoTrack(TClonesArray *recoInfo, std::function<bool(FairLink &, FairLink &)> function)
{
  std::sort(fRecoTrackIDs.begin(), fRecoTrackIDs.end(), function);

  SetBestRecoTrack(recoInfo);
  SetRecoTrackIsClone(recoInfo);
}

void PndTrackingQAMCInfo::FindBestRecoTrack(TClonesArray *recoInfo)
{
  std::sort(fRecoTrackIDs.begin(), fRecoTrackIDs.end(), [&](FairLink &a, FairLink &b) {
    PndTrackingQARecoInfo *aVal = dynamic_cast<PndTrackingQARecoInfo *>(recoInfo->At(a.GetIndex()));
    PndTrackingQARecoInfo *bVal = dynamic_cast<PndTrackingQARecoInfo *>(recoInfo->At(b.GetIndex()));
    double resA = aVal->GetEfficiency();
    double resB = bVal->GetEfficiency();
    return resA > resB;
  });

  SetBestRecoTrack(recoInfo);
  SetRecoTrackIsClone(recoInfo);
}

void PndTrackingQAMCInfo::SetBestRecoTrack(TClonesArray *recoInfo)
{
  if (fRecoTrackIDs.size() > 0) {
    fAssoRecoTrackID = fRecoTrackIDs[0];
    PndTrackingQARecoInfo *reco = dynamic_cast<PndTrackingQARecoInfo *>(recoInfo->At(fRecoTrackIDs[0].GetIndex()));
    if (reco == nullptr) {
      LOG(error) << "No QARecoInfo for " << fRecoTrackIDs[0].GetIndex() << std::endl;
      return;
    }
    if (reco->GetQuality() > 0)
      fQuality = reco->GetQuality();
  }
  //  if (fRecoTrackIDs.size() > 1) {
  //    std::cout << "RecoTrackIDs > " << fRecoTrackIDs.size() << std::endl;
  //  }
}

void PndTrackingQAMCInfo::SetRecoTrackIsClone(TClonesArray *recoInfo)
{
  int i = 0;
  for (auto recoLink : fRecoTrackIDs) {
    PndTrackingQARecoInfo *reco = dynamic_cast<PndTrackingQARecoInfo *>(recoInfo->At(recoLink.GetIndex()));
    reco->SetIsClone(i != 0);
    i++;
  }
}
