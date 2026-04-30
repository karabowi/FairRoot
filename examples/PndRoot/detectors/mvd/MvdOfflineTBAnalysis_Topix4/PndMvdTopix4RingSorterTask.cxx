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
 * PndMvdTopix4RingSorterTask.cxx
 *
 *  Created on: Sep 9, 2011
 *      Author: stockman
 */

#include <MvdOfflineTBAnalysis_Topix4/PndMvdTopix4RingSorter.h>
#include <MvdOfflineTBAnalysis_Topix4/PndMvdTopix4RingSorterTask.h>

#include "PndSdsDigiTopix4.h"
#include "TClonesArray.h"

ClassImp(PndMvdTopix4RingSorterTask);

PndMvdTopix4RingSorterTask::PndMvdTopix4RingSorterTask()
{
  // TODO Auto-generated constructor stub
}

PndMvdTopix4RingSorterTask::~PndMvdTopix4RingSorterTask()
{
  // TODO Auto-generated destructor stub
}

void PndMvdTopix4RingSorterTask::AddNewDataToTClonesArray(FairTimeStamp *data)
{
  FairRootManager *ioman = FairRootManager::Instance();
  TClonesArray *myArray = ioman->GetTClonesArray(fOutputBranch);
  if (fVerbose > 1) {
    std::cout << "-I- PndMvdTopix4RingSorterTask::AddNewDataToTClonesArray Data: ";
    std::cout << *(PndSdsDigiTopix4 *)(data) << std::endl;
  }
  new ((*myArray)[myArray->GetEntries()]) PndSdsDigiTopix4(*(PndSdsDigiTopix4 *)(data));
}

FairRingSorter *PndMvdTopix4RingSorterTask::InitSorter(Int_t numberOfCells, Double_t widthOfCells) const
{
  return new PndMvdTopix4RingSorter(numberOfCells, widthOfCells);
}
