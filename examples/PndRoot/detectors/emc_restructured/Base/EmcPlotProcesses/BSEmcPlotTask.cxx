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

#include "BSEmcPlotTask.h"

BSEmcPlotTask::BSEmcPlotTask(const std::string &detectorname, BSEmcHistogrammer *histogrammer)
  : PndProcessTask{detectorname},
    fHistogrammer(histogrammer), fMCPointBranchname{BSEmcDataBranchNames::fgMCPointBranchName + detectorname}, fMCDepositBranchname{BSEmcDataBranchNames::fgMCDepositBranchName +
                                                                                                                                    detectorname},
    fWaveformBranchname{BSEmcDataBranchNames::fgMultiWaveformBranchName + detectorname}, fDigiBranchname{BSEmcDataBranchNames::fgDigiBranchName + detectorname},
    fPreclusterBranchname{BSEmcDataBranchNames::fgPreclusterBranchName + detectorname}, fClusterBranchname{BSEmcDataBranchNames::fgClusterBranchName + detectorname},
    fSubClusterBranchname{BSEmcDataBranchNames::fgSubClusterBranchName + detectorname}, fRecoHitBranchname{BSEmcDataBranchNames::fgRecoHitBranchName + detectorname}
{
}

//--------------
// Destructor --
//--------------

BSEmcPlotTask::~BSEmcPlotTask() {}

void BSEmcPlotTask::SetupDataArrays()
{
  LOG(debug) << "BSEmcPlotTask::SetupDataArrays() - " << this->fDetectorName << " - initialising";

  PndContainerRegister *containerRegister = fContainerRegister.get();
  for (const auto &entry : fContainerRegister->GetRequests()) {
    if (DispatchRequest<PndMCTrack>(entry.second, containerRegister, "MCTrack")) {
    } else if (DispatchRequest<BSEmcMCPoint>(entry.second, containerRegister, fMCPointBranchname)) {
    } else if (DispatchRequest<BSEmcMCDeposit>(entry.second, containerRegister, fMCDepositBranchname)) {
    } else if (DispatchRequest<BSEmcMultiWaveform>(entry.second, containerRegister, fWaveformBranchname)) {
    } else if (DispatchRequest<BSEmcDigi>(entry.second, containerRegister, fDigiBranchname)) {
    } else if (DispatchRequest<BSEmcPrecluster>(entry.second, containerRegister, fPreclusterBranchname)) {
    } else if (DispatchRequest<BSEmcCluster>(entry.second, containerRegister, fClusterBranchname)) {
    } else if (DispatchRequest<BSEmcSubCluster>(entry.second, containerRegister, fSubClusterBranchname)) {
    } else if (DispatchRequest<BSEmcRecoHit>(entry.second, containerRegister, fRecoHitBranchname)) {
    } else {
      const TString &classname = entry.second.fClassType;
      const TString &branchname = entry.second.fBranchname;
      LOG(fatal) << "BSEmcPlottingTask::SetupDataArrays(): " << this->fDetectorName << " - Can not initialize/provide container " << branchname << " for " << classname
                 << ". Aborting!";
      exit(-1);
      // throw std::exception(); // Exception is catched in PndProcessTask::SetupDataArrays(), which will return kERROR InitStatus
    }
  }

  LOG(debug) << "BSEmcPlotTask::SetupDataArrays: successfull";
}

void BSEmcPlotTask::SetupProcess(PndProcess *process)
{
  // Setup Parameters (else done in non overriden version)
  PndProcessTask::SetupProcess(process);
  // process->SetupParameters(fBranchNamePar);
  if (fFolderPrefix == "") {
    fFolderPrefix = "Emc/" + this->fDetectorName;
  }
  if (dynamic_cast<BSEmcPlotProcess *>(process) != nullptr) {
    BSEmcPlotProcess *plotprocess = dynamic_cast<BSEmcPlotProcess *>(process);
    plotprocess->SetHistogrammer(fHistogrammer);
    plotprocess->InitHistos(fFolderPrefix);
  }
}

void BSEmcPlotTask::PreProcessing(Option_t * /*unused*/)
{
  LOG(trace) << "BSEmcPlotTask: PreProcessing - " << this->fDetectorName;
}

void BSEmcPlotTask::PostProcessing(Option_t * /*unused*/)
{
  LOG(trace) << "BSEmcPlotTask: PostProcessing - " << this->fDetectorName;
}

void BSEmcPlotTask::ExecuteTasks(Option_t *option)
{
  PndPersistencyTask::ExecuteTasks(option);
}
