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
 *      Author: stockman
 * PndRichHitSorterTask.cxx
 *
 */
// -------------------------------------------------------------------------
// -----                   PndRichHitSorterTask source file            -----
// -----         HARPHOOL KUMAWAT h.kumawat@gsi.de                     -----
// -----                                                               -----
// -------------------------------------------------------------------------

#include "PndRichHitSorterTask.h"

#include "PndRichDigi.h"
#include "PndRichHitRingSorter.h"

#include "FairRootManager.h"

ClassImp(PndRichHitSorterTask);

PndRichHitSorterTask::PndRichHitSorterTask()
{
  // TODO Auto-generated constructor stub
}

PndRichHitSorterTask::~PndRichHitSorterTask()
{
  // TODO Auto-generated destructor stub
}

void PndRichHitSorterTask::AddNewDataToTClonesArray(FairTimeStamp *data)
{
  FairRootManager *ioman = FairRootManager::Instance();
  TClonesArray *myArray = ioman->GetTClonesArray(fOutputBranch);
  if (fVerbose > 1) {
    std::cout << "-I- PndRichHitSorterTask::AddNewDataToTClonesArray Data: ";
    std::cout << *(PndRichDigi *)(data) << std::endl;
  }
  new ((*myArray)[myArray->GetEntries()]) PndRichDigi(*(PndRichDigi *)(data));
}

FairRingSorter *PndRichHitSorterTask::InitSorter(Int_t numberOfCells, Double_t widthOfCells) const
{
  return new PndRichHitRingSorter(numberOfCells, widthOfCells);
}
