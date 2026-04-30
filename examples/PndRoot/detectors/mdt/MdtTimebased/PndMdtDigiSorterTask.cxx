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
 * PndMdtDigiRingSorterTask.cxx
 */

#include "PndMdtDigiSorterTask.h"
#include "PndMdtDigiRingSorter.h"
#include "TClonesArray.h"

#include "FairTimeStamp.h"
#include "FairRootManager.h"

ClassImp(PndMdtDigiSorterTask);

PndMdtDigiSorterTask::PndMdtDigiSorterTask() {}

PndMdtDigiSorterTask::~PndMdtDigiSorterTask() {}

void PndMdtDigiSorterTask::AddNewDataToTClonesArray(FairTimeStamp *data)
{
  FairRootManager *ioman = FairRootManager::Instance();
  TClonesArray *myArray = ioman->GetTClonesArray(fOutputBranch);
  if (fVerbose > 1) {
    std::cout << "-I- PndMdtDigiSorterTask::AddNewDataToTClonesArray Data: ";
    data->Print();
    std::cout << std::endl;
  }
  (*myArray)[myArray->GetEntries()] = data;
}

FairRingSorter *PndMdtDigiSorterTask::InitSorter(Int_t numberOfCells, Double_t widthOfCells) const
{
  return new PndMdtDigiRingSorter(numberOfCells, widthOfCells);
}
