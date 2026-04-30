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
// -----                    PndMvdDigiTask source file                 -----
// -----                  Created 07/10/08  by R. Kliemt               -----
// -----                  Wrapper for Mvd digitizing tasks             -----
// -------------------------------------------------------------------------

#include "PndMvdDigiTask.h"
#include "PndMvdHybridHitProducer.h"
#include "PndMvdStripHitProducer.h"
#include "PndMvdNoiseProducer.h"
#include "PndSdsDigiPixelSorterTask.h"
#include "PndSdsDigiStripSorterTask.h"

#include "PndMvdTimeWalkCorrTask.h"

// -----   Default constructor   -------------------------------------------
PndMvdDigiTask::PndMvdDigiTask() : PndPersistencyTask("MVD Digitization BlacBox Task"), fPersistance(kTRUE)
{
  this->Add(new PndMvdHybridHitProducer());
  this->Add(new PndMvdStripHitProducer());
  this->Add(new PndMvdNoiseProducer());
  SetPersistance(fPersistance);
}
// -------------------------------------------------------------------------

void PndMvdDigiTask::SetPersistance(Bool_t pers)
{
  SetPersistency(pers);
  TList *thistasks = this->GetListOfTasks();
  ((PndMvdHybridHitProducer *)thistasks->At(0))->SetPersistency(GetPersistency());
  ((PndMvdStripHitProducer *)thistasks->At(1))->SetPersistency(GetPersistency());
  ((PndMvdNoiseProducer *)thistasks->At(2))->SetPersistency(GetPersistency());
  return;
}

void PndMvdDigiTask::RunTimeBased()
{
  TList *thistasks = this->GetListOfTasks();
  ((PndMvdHybridHitProducer *)thistasks->At(0))->RunTimeBased();
  ((PndMvdStripHitProducer *)thistasks->At(1))->RunTimeBased();
  ((PndMvdNoiseProducer *)thistasks->At(2))->RunTimeBased();

  this->Add(new PndSdsDigiPixelSorterTask(10000, 10, "MVDPixelDigis", "MVDSortedPixelDigis", "PndMvd")); //"MVDDigisCorr"
  this->Add(new PndSdsDigiStripSorterTask(10000, 10, "MVDStripDigis", "MVDSortedStripDigis", "PndMvd"));

  std::cout << "PndMvdDigiTask: PndMvdPixelDigiSorterTask added" << std::endl;
}

// -----   Destructor   ----------------------------------------------------
PndMvdDigiTask::~PndMvdDigiTask() {}
// -------------------------------------------------------------------------

ClassImp(PndMvdDigiTask);
