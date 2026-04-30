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

#include "BSEmcClusteringTask.h"

#include "BSEmcDigi.h"
#include "BSEmcPrecluster.h"
#include "BSEmcCluster.h"
#include "BSEmcSubCluster.h"
#include "BSEmcRecoHit.h"

BSEmcClusteringTask::BSEmcClusteringTask(const std::string &t_detectorName, Bool_t t_storeclusters)
  : PndProcessTask{t_detectorName}, fMCDepositBranchname{BSEmcDataBranchNames::fgMCDepositBranchName + t_detectorName}, fDigiBranchname{BSEmcDataBranchNames::fgDigiBranchName +
                                                                                                                                        t_detectorName},
    fPreclusterBranchname{BSEmcDataBranchNames::fgPreclusterBranchName + t_detectorName}, fClusterBranchname{BSEmcDataBranchNames::fgClusterBranchName + t_detectorName},
    fSubClusterBranchname{BSEmcDataBranchNames::fgSubClusterBranchName + t_detectorName}, fRecoHitBranchname{BSEmcDataBranchNames::fgRecoHitBranchName + t_detectorName}
{
  this->SetPersistency(t_storeclusters);
}

//--------------
// Destructor --
//--------------
BSEmcClusteringTask::~BSEmcClusteringTask() {}

void BSEmcClusteringTask::SetupDataArrays()
{

  LOG(debug) << "BSEmcClusteringTask::SetupDataArrays() - " << this->fDetectorName << " - initialising";

  PndContainerRegister *containerRegister = fContainerRegister.get();
  for (const auto &entry : fContainerRegister->GetRequests()) {

    if (DispatchRequest<BSEmcMCDeposit>(entry.second, containerRegister, fMCDepositBranchname)) {
    } else if (DispatchRequest<BSEmcDigi>(entry.second, containerRegister, fDigiBranchname)) {
    } else if (DispatchRequest<BSEmcPrecluster>(entry.second, containerRegister, fPreclusterBranchname)) {
    } else if (DispatchRequest<BSEmcCluster>(entry.second, containerRegister, fClusterBranchname)) {
    } else if (DispatchRequest<BSEmcSubCluster>(entry.second, containerRegister, fSubClusterBranchname)) {
    } else if (DispatchRequest<BSEmcRecoHit>(entry.second, containerRegister, fRecoHitBranchname)) {
    } else {
      const TString &classname = entry.second.fClassType;
      const TString &branchname = entry.second.fBranchname;
      LOG(fatal) << "BSEmcClusteringTask::SetupDataArrays(): " << this->fDetectorName << " - Can not initialize/provide container " << branchname << " for " << classname
                 << ". Aborting!";
      exit(-1);
      // throw std::exception(); // Exception is catched in PndProcessTask::SetupDataArrays(), which will return kERROR InitStatus
    }
  }

  LOG(debug) << "BSEmcClusteringTask::SetupDataArrays: successfull";
}

void BSEmcClusteringTask::PreProcessing(Option_t * /*unused*/)
{
  LOG(trace) << "BSEmcClusteringTask: PreProcessing - " << this->fDetectorName;
}

void BSEmcClusteringTask::PostProcessing(Option_t * /*unused*/)
{
  LOG(trace) << "BSEmcClusteringTask: PostProcessing - " << this->fDetectorName;
}
