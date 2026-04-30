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
 * PndSdsDigiStripSorterTask.h
 *
 *  Created on: Sep 9, 2011
 *      Author: stockman
 */

#ifndef PNDSDSDigiStripSORTERTASK_H_
#define PNDSDSDigiStripSORTERTASK_H_

#include <FairRingSorterTask.h>

class PndSdsDigiStripSorterTask : public FairRingSorterTask {
 public:
  PndSdsDigiStripSorterTask();
  PndSdsDigiStripSorterTask(const char *name) : FairRingSorterTask(name){};
  PndSdsDigiStripSorterTask(Int_t numberOfCells, Double_t widthOfCells, TString inputBranch, TString outputBranch, TString folderName)
    : FairRingSorterTask(numberOfCells, widthOfCells, inputBranch, outputBranch, folderName){};

  virtual ~PndSdsDigiStripSorterTask();

  virtual void AddNewDataToTClonesArray(FairTimeStamp *data);
  virtual FairRingSorter *InitSorter(Int_t numberOfCells, Double_t widthOfCells) const;

  ClassDef(PndSdsDigiStripSorterTask, 1);
};

#endif /* PNDSDSDigiStripSORTERTASK_H_ */
