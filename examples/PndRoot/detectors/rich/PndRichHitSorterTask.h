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
 *      Author: stockman
 * PndRichHitSorterTask.h
 *
 */
// -------------------------------------------------------------------------
// -----                   PndRichHitSorterTask header file            -----
// -----         HARPHOOL KUMAWAT h.kumawat@gsi.de                     -----
// -----                                                               -----
// -------------------------------------------------------------------------

#ifndef PNDRICHHITSORTERTASK_H_
#define PNDRICHHITSORTERTASK_H_

#include "TClonesArray.h"

#include <FairRingSorterTask.h>
#include <FairRingSorter.h>

class PndRichHitSorterTask : public FairRingSorterTask {
 public:
  PndRichHitSorterTask();
  PndRichHitSorterTask(const char *name) : FairRingSorterTask(name){};
  PndRichHitSorterTask(Int_t numberOfCells, Double_t widthOfCells, TString inputBranch, TString outputBranch, TString folderName)
    : FairRingSorterTask(numberOfCells, widthOfCells, inputBranch, outputBranch, folderName){};

  virtual ~PndRichHitSorterTask();

  virtual void AddNewDataToTClonesArray(FairTimeStamp *data);
  virtual FairRingSorter *InitSorter(Int_t numberOfCells, Double_t widthOfCells) const;

  ClassDef(PndRichHitSorterTask, 1);
};

#endif /* PNDRICHHITSORTERTASK_H_ */
