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
 * PndEmcDigiSorterTask.cxx
 */

#include "PndEmcDigiSorterTask.h"

#include "PndEmcDigi.h"
#include "PndEmcDigiRingSorter.h"

#include "FairRootManager.h"

#include "TClonesArray.h"

ClassImp(PndEmcDigiSorterTask);

PndEmcDigiSorterTask::PndEmcDigiSorterTask() {}

PndEmcDigiSorterTask::~PndEmcDigiSorterTask() {}

void PndEmcDigiSorterTask::AddNewDataToTClonesArray(FairTimeStamp *data)
{
  FairRootManager *ioman = FairRootManager::Instance();
  TClonesArray *myArray = ioman->GetTClonesArray(fOutputBranch);
  if (fVerbose > 1) {
    std::cout << "-I- PndEmcDigiSorterTask::AddNewDataToTClonesArray Data: ";
    std::cout << *(PndEmcDigi *)(data) << std::endl;
  }
  new ((*myArray)[myArray->GetEntries()]) PndEmcDigi(*(PndEmcDigi *)(data));
}

FairRingSorter *PndEmcDigiSorterTask::InitSorter(Int_t numberOfCells, Double_t widthOfCells) const
{
  return new PndEmcDigiRingSorter(numberOfCells, widthOfCells);
}
