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
// -----                     PndMvdClusterTask source file             -----
// -----                  Created 07/10/08  by R. Kliemt               -----
// -------------------------------------------------------------------------

#include "PndMvdClusterTask.h"
#include "PndMvdPixelClusterTask.h"
#include "PndMvdStripClusterTask.h"
// #include "PndMvdTimeWalkCorrTask.h"

PndMvdClusterTask::PndMvdClusterTask() : PndPersistencyTask("Mvd Hit Reconstruction BlackBox Task")
{
  this->Add(new PndMvdPixelClusterTask());
  this->Add(new PndMvdStripClusterTask());
  // this->Add(new PndMvdTimeWalkCorrTask());   // Task to correct Digi-TimeStamp from TimeWalk-Effect. Corrected digis will be saved in output-file
  SetPersistance(kTRUE);
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMvdClusterTask::~PndMvdClusterTask() {}
// -------------------------------------------------------------------------

void PndMvdClusterTask::SetPersistance(Bool_t pers)
{
  // TODO: This is not recursive
  SetPersistency(pers);
  TList *thistasks = this->GetListOfTasks();
  ((PndMvdPixelClusterTask *)thistasks->At(0))->SetPersistency(GetPersistency());
  ((PndMvdStripClusterTask *)thistasks->At(1))->SetPersistency(GetPersistency());
  return;
}

ClassImp(PndMvdClusterTask);
