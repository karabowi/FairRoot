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
 * PndGemDigiSorterTask.h
 *
 *  Created on: Sep 9, 2011
 *      Author: stockman
 */

#ifndef PNDGEMDIGISORTERTASK_H_
#define PNDGEMDIGISORTERTASK_H_

#include <FairRingSorterTask.h>
#include <FairRingSorter.h>

class PndGemDigiSorterTask : public FairRingSorterTask {
 public:
  PndGemDigiSorterTask();
  PndGemDigiSorterTask(const char *name) : FairRingSorterTask(name){};
  PndGemDigiSorterTask(Int_t numberOfCells, Double_t widthOfCells, TString inputBranch, TString outputBranch, TString folderName)
    : FairRingSorterTask(numberOfCells, widthOfCells, inputBranch, outputBranch, folderName){};

  virtual ~PndGemDigiSorterTask();

  virtual void AddNewDataToTClonesArray(FairTimeStamp *data);
  virtual FairRingSorter *InitSorter(Int_t numberOfCells, Double_t widthOfCells) const;

  ClassDef(PndGemDigiSorterTask, 1);
};

#endif /* PNDGEMDIGISORTERTASK_H_ */
