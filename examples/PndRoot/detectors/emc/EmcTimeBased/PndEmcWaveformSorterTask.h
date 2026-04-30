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
 * PndEmcWaveformSorterTask.h
 */

#ifndef PNDEMCWAVEFORMSORTERTASK_H_
#define PNDEMCWAVEFORMSORTERTASK_H_

#include <FairRingSorterTask.h>
#include <FairRingSorter.h>

class PndEmcWaveformSorterTask : public FairRingSorterTask {
 public:
  PndEmcWaveformSorterTask();
  PndEmcWaveformSorterTask(const char *name) : FairRingSorterTask(name){};
  PndEmcWaveformSorterTask(Int_t numberOfCells, Double_t widthOfCells, TString inputBranch, TString outputBranch, TString folderName)
    : FairRingSorterTask(numberOfCells, widthOfCells, inputBranch, outputBranch, folderName){};

  virtual ~PndEmcWaveformSorterTask();

  virtual void AddNewDataToTClonesArray(FairTimeStamp *data);
  virtual FairRingSorter *InitSorter(Int_t numberOfCells, Double_t widthOfCells) const;

  ClassDef(PndEmcWaveformSorterTask, 1);
};

#endif /* PNDEMCWAVEFORMSORTERTASK_H_ */
