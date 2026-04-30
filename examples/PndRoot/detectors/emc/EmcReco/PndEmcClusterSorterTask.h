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
 * PndEmcClusterSorterTask.h
 */

#ifndef PNDEMCCLUSTERSORTERTASK_H_
#define PNDEMCCLUSTERSORTERTASK_H_

#include <FairRingSorterTask.h>
#include <FairRingSorter.h>
#include <FairRootManager.h>

class PndEmcClusterSorterTask : public FairRingSorterTask {
 public:
  PndEmcClusterSorterTask();
  PndEmcClusterSorterTask(const char *name) : FairRingSorterTask(name){};
  PndEmcClusterSorterTask(Int_t numberOfCells, Double_t widthOfCells, TString inputBranch, TString outputBranch, TString folderName)
    : FairRingSorterTask(numberOfCells, widthOfCells, inputBranch, outputBranch, folderName)
  {
    FairRootManager::Instance()->GetObject(inputBranch);
  };

  virtual ~PndEmcClusterSorterTask();

  virtual void AddNewDataToTClonesArray(FairTimeStamp *data);
  virtual FairRingSorter *InitSorter(Int_t numberOfCells, Double_t widthOfCells) const;

  void SetClusterType(Int_t type = 0) { fClusterType = type; };

 private:
  Int_t fClusterType = 0;

  ClassDef(PndEmcClusterSorterTask, 1);
};

#endif /* PNDEMCCLUSTERSORTERTASK_H_ */
