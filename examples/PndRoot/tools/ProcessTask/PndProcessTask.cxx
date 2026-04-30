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

#include "PndProcessTask.h"
#include "PndParameterRegister.h"

PndProcessTask::PndProcessTask(const std::string &t_detectorName) : PndPersistencyTask(), PndProcessHandler(t_detectorName) {}

PndProcessTask::~PndProcessTask() {}

InitStatus PndProcessTask::Init()
{
  InitStatus result = kSUCCESS;
  try {
    this->SetupProcesses();
    this->GetRequiredDataContainerInfo();
    SetupDataArrays();
    this->PassDataContainersToProcesses();
  } catch (const std::exception &e) {
    result = kERROR;
  }
  return result;
}

void PndProcessTask::Exec(Option_t *t_option)
{
  FairRootManager *ioman = FairRootManager::Instance();
  if (ioman == nullptr) {
    LOG(error) << "PndProcessTask::Exec - FairRootManager is nullptr";
    exit(-1);
  }
  PreProcessing(t_option);
  for (PndProcess *process : this->fProcesses) {
    process->SetEventTime(ioman->GetEventTime());
    process->PreProcess();
    process->Process();
    process->PostProcess();
  }
  PostProcessing(t_option);
}

void PndProcessTask::Finish()
{
  LOG(info) << "PndProcessTask: Finish - " << this->fDetectorName;
  for (PndProcess *process : this->fProcesses) {
    process->TearDown();
  }
}

void PndProcessTask::PreProcessing(Option_t * /*unused*/)
{
  LOG(trace) << "PndProcessTask: PreProcessing - " << this->fDetectorName;
}

void PndProcessTask::PostProcessing(Option_t * /*unused*/)
{
  LOG(trace) << "PndProcessTask: PostProcessing - " << this->fDetectorName;
}

void PndProcessTask::SetParContainers()
{
  // Get run and runtime database
  FairRun *run = FairRun::Instance();
  if (run == nullptr) {
    Fatal("SetParContainers", "No analysis run");
  }
  FairRuntimeDb *db = run->GetRuntimeDb();
  if (db == nullptr) {
    Fatal("SetParContainers", "No runtime database");
  }

  LOG(debug) << "PndProcessTask - " << this->fDetectorName << " SetParContainers()";
  for (const std::string &parameterName : this->fParameterRegister->GetRequestList()) {
    LOG(debug) << parameterName;
    this->fParameterRegister->SetParameter(parameterName, db->getContainer(parameterName.c_str()));
  }
}