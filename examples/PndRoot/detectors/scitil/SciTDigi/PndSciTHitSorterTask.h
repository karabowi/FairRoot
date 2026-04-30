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
 * PndSciTHitSorterTask.h
 *
 *  Created on: May 13, 2015
 *      Author: Steinschaden Dominik
 *  Last update: 6.2015
 */

#ifndef PNDSCITHITSORTERTASK_H
#define PNDSCITHITSORTERTASK_H

#include "FairRingSorterTask.h"
#include "FairRingSorter.h"

class PndSciTHitSorterTask : public FairRingSorterTask {
 public:
  PndSciTHitSorterTask();
  PndSciTHitSorterTask(const char *name) : FairRingSorterTask(name){};
  PndSciTHitSorterTask(Int_t numberOfCells, Double_t widthOfCells, TString inputBranch, TString outputBranch, TString folderName)
    : FairRingSorterTask(numberOfCells, widthOfCells, inputBranch, outputBranch, folderName){};

  virtual ~PndSciTHitSorterTask();

  virtual void AddNewDataToTClonesArray(FairTimeStamp *data);
  virtual FairRingSorter *InitSorter(Int_t numberOfCells, Double_t widthOfCells) const;

  ClassDef(PndSciTHitSorterTask, 1);
};

#endif /* PNDSCITHITSORTERTASK_H_ */
