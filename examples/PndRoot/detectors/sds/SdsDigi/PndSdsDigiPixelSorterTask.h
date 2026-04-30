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
 * PndSdsDigiPixelSorterTask.h
 *
 *  Created on: Sep 9, 2011
 *      Author: stockman
 */

#ifndef PNDSDSDIGIPIXELSORTERTASK_H_
#define PNDSDSDIGIPIXELSORTERTASK_H_

#include <FairRingSorterTask.h>
#include <FairRingSorter.h>

class PndSdsDigiPixelSorterTask : public FairRingSorterTask {
 public:
  PndSdsDigiPixelSorterTask();
  PndSdsDigiPixelSorterTask(const char *name) : FairRingSorterTask(name){};
  PndSdsDigiPixelSorterTask(Int_t numberOfCells, Double_t widthOfCells, TString inputBranch, TString outputBranch, TString folderName)
    : FairRingSorterTask(numberOfCells, widthOfCells, inputBranch, outputBranch, folderName){};

  virtual ~PndSdsDigiPixelSorterTask();

  virtual void AddNewDataToTClonesArray(FairTimeStamp *data);
  virtual FairRingSorter *InitSorter(Int_t numberOfCells, Double_t widthOfCells) const;

  ClassDef(PndSdsDigiPixelSorterTask, 1);
};

#endif /* PNDSDSDIGIPIXELSORTERTASK_H_ */
