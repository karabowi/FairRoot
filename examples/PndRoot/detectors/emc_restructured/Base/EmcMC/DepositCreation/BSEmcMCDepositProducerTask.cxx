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

#include "BSEmcMCDepositProducerTask.h"

BSEmcMCDepositProducerTask::BSEmcMCDepositProducerTask(const std::string &t_detectorName, Bool_t t_storeDeposits)
  : PndProcessTask{t_detectorName}, fPointBranchname{BSEmcDataBranchNames::fgMCPointBranchName + t_detectorName}, fMCDepositBranchname{BSEmcDataBranchNames::fgMCDepositBranchName +
                                                                                                                                       t_detectorName}
{
  this->SetPersistency(t_storeDeposits);
}

BSEmcMCDepositProducerTask::~BSEmcMCDepositProducerTask() {}

void BSEmcMCDepositProducerTask::SetupDataArrays()
{
  PndContainerRegister *containerRegister = fContainerRegister.get();
  for (const auto &entry : fContainerRegister->GetRequests()) {

    if (DispatchRequest<PndMCTrack>(entry.second, containerRegister, "MCTrack")) {
    } else if (DispatchRequest<BSEmcMCPoint>(entry.second, containerRegister, fPointBranchname)) {
    } else if (DispatchRequest<BSEmcMCDeposit>(entry.second, containerRegister, fMCDepositBranchname)) {
    } else {
      const TString &classname = entry.second.fClassType;
      const TString &branchname = entry.second.fBranchname;
      LOG(error) << "BSEmcMCDepositProducerTask::SetupDataArrays(): " << this->fDetectorName << " - Can not initialize/provide container " << branchname << " for " << classname
                 << ". Aborting.";
      throw std::exception(); // Exception is catched in PndProcessTask::Init(), which will return kERROR InitStatus
    }
  }

  LOG(debug) << "BSEmcMCDepositProducerTask::SetupDataArrays() - " << this->fDetectorName << " - "
             << "Intialization successfull: Taking Points from " << fPointBranchname << " and writting it to " << fMCDepositBranchname;
}

void BSEmcMCDepositProducerTask::PreProcessing(Option_t * /*unused*/) {}

void BSEmcMCDepositProducerTask::PostProcessing(Option_t * /*unused*/) {}
