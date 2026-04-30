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
 * PndEmcWaveformSorterTask.cxx
 */

#include "PndEmcWaveformSorterTask.h"

#include "PndEmcWaveform.h"
#include "PndEmcWaveformRingSorter.h"

#include "FairRootManager.h"

#include "TClonesArray.h"

ClassImp(PndEmcWaveformSorterTask);

PndEmcWaveformSorterTask::PndEmcWaveformSorterTask() {}

PndEmcWaveformSorterTask::~PndEmcWaveformSorterTask() {}

void PndEmcWaveformSorterTask::AddNewDataToTClonesArray(FairTimeStamp *data)
{
  FairRootManager *ioman = FairRootManager::Instance();
  TClonesArray *myArray = ioman->GetTClonesArray(fOutputBranch);
  if (fVerbose > 1) {
    std::cout << "-I- PndEmcWaveformSorterTask::AddNewDataToTClonesArray Data: ";
    std::cout << *(PndEmcWaveform *)(data) << std::endl;
  }
  new ((*myArray)[myArray->GetEntries()]) PndEmcWaveform(*(PndEmcWaveform *)(data));
}

FairRingSorter *PndEmcWaveformSorterTask::InitSorter(Int_t numberOfCells, Double_t widthOfCells) const
{
  return new PndEmcWaveformRingSorter(numberOfCells, widthOfCells);
}
