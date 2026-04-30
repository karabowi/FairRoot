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
 * PndEmcPreclusterSorterTask.h
 */

#ifndef PNDEMCPRECLUSTERSORTERTASK_H_
#define PNDEMCPRECLUSTERSORTERTASK_H_

#include <FairRingSorterTask.h>
#include <FairRingSorter.h>
#include <FairRootManager.h>

class PndEmcPreclusterSorterTask : public FairRingSorterTask {
 public:
  PndEmcPreclusterSorterTask();
  PndEmcPreclusterSorterTask(const char *name) : FairRingSorterTask(name){};
  PndEmcPreclusterSorterTask(Int_t numberOfCells, Double_t widthOfCells, TString inputBranch, TString outputBranch, TString folderName)
    : FairRingSorterTask(numberOfCells, widthOfCells, inputBranch, outputBranch, folderName)
  {
    FairRootManager::Instance()->GetObject(inputBranch);
  };

  virtual ~PndEmcPreclusterSorterTask();

  virtual void AddNewDataToTClonesArray(FairTimeStamp *data);
  virtual FairRingSorter *InitSorter(Int_t numberOfCells, Double_t widthOfCells) const;

  void SetClusterType(Int_t type = 0) { fClusterType = type; };

 private:
  Int_t fClusterType = 0;

  ClassDef(PndEmcPreclusterSorterTask, 1);
};

#endif /* PNDEMCPRECLUSTERSORTERTASK_H_ */
