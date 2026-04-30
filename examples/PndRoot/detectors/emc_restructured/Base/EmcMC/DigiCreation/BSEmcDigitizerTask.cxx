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

#include "BSEmcDigitizerTask.h"

// -------------------------------------------------------------------------
BSEmcDigitizerTask::BSEmcDigitizerTask(const std::string &t_detectorName, Bool_t t_storedigis)
  : PndProcessTask{t_detectorName}, fMCDepositBranchname{BSEmcDataBranchNames::fgMCDepositBranchName + t_detectorName},
    fDigiBranchname{BSEmcDataBranchNames::fgDigiBranchName + t_detectorName}, fWaveformBranchname{BSEmcDataBranchNames::fgMultiWaveformBranchName + t_detectorName}
{
  this->SetPersistency(t_storedigis);
}

// -------------------------------------------------------------------------
BSEmcDigitizerTask::~BSEmcDigitizerTask() {}

// -------------------------------------------------------------------------
void BSEmcDigitizerTask::SetupDataArrays()
{
  LOG(debug) << "BSEmcDigitizerTask::SetupDataArrays() - " << this->fDetectorName << " parameters: ";

  PndContainerRegister *containerRegister = fContainerRegister.get();
  for (const auto &entry : fContainerRegister->GetRequests()) {
    if (DispatchRequest<BSEmcMCDeposit>(entry.second, containerRegister, fMCDepositBranchname)) {
    } else if (DispatchRequest<BSEmcMultiWaveform>(entry.second, containerRegister, fWaveformBranchname)) {
    } else if (DispatchRequest<BSEmcDigi>(entry.second, containerRegister, fDigiBranchname)) {
    } else {
      const TString &classname = entry.second.fClassType;
      const TString &branchname = entry.second.fBranchname;
      LOG(fatal) << "BSEmcDigitizerTask::SetupDataArrays(): " << this->fDetectorName << " - Can not initialize/provide container " << branchname << " for " << classname
                 << ". Aborting!";
      exit(-1);
      // throw std::exception(); // Exception is catched in PndProcessTask::SetupDataArrays(), which will return kERROR InitStatus
    }
  }

  LOG(debug) << "BSEmcDigitizerTask::SetupDataArrays() - " << this->fDetectorName << " parameters: "
             << "Intialization successfull.";
}

// -------------------------------------------------------------------------
void BSEmcDigitizerTask::PreProcessing(Option_t * /*unused*/) {}

// -------------------------------------------------------------------------
void BSEmcDigitizerTask::PostProcessing(Option_t * /*unused*/) {}

// -------------------------------------------------------------------------
