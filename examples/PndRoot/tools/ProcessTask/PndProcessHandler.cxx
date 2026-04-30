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

#include "PndProcessHandler.h"

PndProcessHandler::~PndProcessHandler()
{
  for (PndProcess *process : this->fProcesses) {
    delete process;
  }
  this->fProcesses.clear();
}

void PndProcessHandler::AddProcess(PndProcess *t_process)
{
  t_process->SetDetectorName(fDetectorName);
  fParameterRegister->AddRequestList(t_process->GetListOfRequiredParameters());
  this->fProcesses.push_back(t_process);
}

void PndProcessHandler::GetRequiredDataContainerInfo()
{
  for (PndProcess *process : this->fProcesses) {
    process->RequestDataContainer(fContainerRegister.get());
  }
}

void PndProcessHandler::PassDataContainersToProcesses()
{
  for (PndProcess *process : this->fProcesses) {
    process->GetDataContainer(fContainerRegister.get());
  }
}

void PndProcessHandler::SetupProcesses()
{
  for (PndProcess *process : this->fProcesses) {
    this->SetupProcess(process);
  }
}

void PndProcessHandler::SetupProcess(PndProcess *t_process)
{
  t_process->SetupParameters(this->fParameterRegister.get());
}
