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
 * PndSttHitSorterTask.cxx
 *
 *  Created on: Sep 9, 2011
 *      Author: stockman
 */

#include "PndSttHitSorterTask.h"

#include "PndSttHit.h"
#include "PndSttHitRingSorter.h"
#include "FairRootManager.h"

#include "TClonesArray.h"

ClassImp(PndSttHitSorterTask);

PndSttHitSorterTask::PndSttHitSorterTask()
{
  // TODO Auto-generated constructor stub
}

PndSttHitSorterTask::~PndSttHitSorterTask()
{
  // TODO Auto-generated destructor stub
}

void PndSttHitSorterTask::AddNewDataToTClonesArray(FairTimeStamp *data)
{
  FairRootManager *ioman = FairRootManager::Instance();
  TClonesArray *myArray = ioman->GetTClonesArray(fOutputBranch);
  if (fVerbose > 1) {
    std::cout << "-I- PndSttHitSorterTask::AddNewDataToTClonesArray Data: ";
    std::cout << *(PndSttHit *)(data) << std::endl;
  }
  new ((*myArray)[myArray->GetEntries()]) PndSttHit(*(PndSttHit *)(data));
}

FairRingSorter *PndSttHitSorterTask::InitSorter(Int_t numberOfCells, Double_t widthOfCells) const
{
  return new PndSttHitRingSorter(numberOfCells, widthOfCells);
}
