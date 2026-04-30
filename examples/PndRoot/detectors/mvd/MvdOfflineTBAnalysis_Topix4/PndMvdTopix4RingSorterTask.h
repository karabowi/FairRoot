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
 * PndMvdTopix4RingSorterTask.h
 *
 *  Created on: Sep 9, 2011
 *      Author: stockman
 */

#ifndef PndMvdTopix4RingSorterTask_H_
#define PndMvdTopix4RingSorterTask_H_

#include <FairRingSorterTask.h>
#include <FairRingSorter.h>

class PndMvdTopix4RingSorterTask : public FairRingSorterTask {
 public:
  PndMvdTopix4RingSorterTask();
  PndMvdTopix4RingSorterTask(const char *name) : FairRingSorterTask(name){};
  PndMvdTopix4RingSorterTask(Int_t numberOfCells, Double_t widthOfCells, TString inputBranch, TString outputBranch, TString folderName)
    : FairRingSorterTask(numberOfCells, widthOfCells, inputBranch, outputBranch, folderName){};

  virtual ~PndMvdTopix4RingSorterTask();

  virtual void AddNewDataToTClonesArray(FairTimeStamp *data);
  virtual FairRingSorter *InitSorter(Int_t numberOfCells, Double_t widthOfCells) const;

  ClassDef(PndMvdTopix4RingSorterTask, 1);
};

#endif /* PndMvdTopix4RingSorterTask_H_ */
