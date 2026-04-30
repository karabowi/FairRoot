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
 * PndSdsDigiStripSorterTask.cxx
 *
 *  Created on: Sep 9, 2011
 *      Author: stockman
 */

#include "PndSdsDigiStripSorterTask.h"

#include "PndSdsDigiStrip.h"
#include "PndSdsDigiStripRingSorter.h"

#include <FairRootManager.h>

#include "TClonesArray.h"

ClassImp(PndSdsDigiStripSorterTask);

PndSdsDigiStripSorterTask::PndSdsDigiStripSorterTask()
{
  // TODO Auto-generated constructor stub
}

PndSdsDigiStripSorterTask::~PndSdsDigiStripSorterTask()
{
  // TODO Auto-generated destructor stub
}

void PndSdsDigiStripSorterTask::AddNewDataToTClonesArray(FairTimeStamp *data)
{
  FairRootManager *ioman = FairRootManager::Instance();
  TClonesArray *myArray = ioman->GetTClonesArray(fOutputBranch);
  if (fVerbose > 1) {
    std::cout << "-I- PndSdsDigiStripSorterTask::AddNewDataToTClonesArray Data: ";
    std::cout << *(PndSdsDigiStrip *)(data) << std::endl;
  }
  new ((*myArray)[myArray->GetEntries()]) PndSdsDigiStrip(*(PndSdsDigiStrip *)(data));
}

FairRingSorter *PndSdsDigiStripSorterTask::InitSorter(Int_t numberOfCells, Double_t widthOfCells) const
{
  return new PndSdsDigiStripRingSorter(numberOfCells, widthOfCells);
}
