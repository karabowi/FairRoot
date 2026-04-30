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
 * PndFtsHitSorterTask.cxx
 *
 *  Created on: Sep 9, 2011
 *      Author: stockman
 */

#include "PndFtsHitSorterTask.h"

#include "PndFtsHit.h"
#include "PndFtsHitRingSorter.h"

#include "FairRootManager.h"

#include "TClonesArray.h"

ClassImp(PndFtsHitSorterTask);

PndFtsHitSorterTask::PndFtsHitSorterTask()
{
  // TODO Auto-generated constructor stub
}

PndFtsHitSorterTask::~PndFtsHitSorterTask()
{
  // TODO Auto-generated destructor stub
}

void PndFtsHitSorterTask::AddNewDataToTClonesArray(FairTimeStamp *data)
{
  FairRootManager *ioman = FairRootManager::Instance();
  TClonesArray *myArray = ioman->GetTClonesArray(fOutputBranch);
  if (fVerbose > 1) {
    std::cout << "-I- PndFtsHitSorterTask::AddNewDataToTClonesArray Data: ";
    std::cout << *(PndFtsHit *)(data) << std::endl;
  }
  new ((*myArray)[myArray->GetEntries()]) PndFtsHit(*(PndFtsHit *)(data));
}

FairRingSorter *PndFtsHitSorterTask::InitSorter(Int_t numberOfCells, Double_t widthOfCells) const
{
  return new PndFtsHitRingSorter(numberOfCells, widthOfCells);
}
