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


/** PndMvdPixelDigiSorterTask.h
 **
 **/

#ifndef PndMvdPixelDigiSorterTask_H
#define PndMvdPixelDigiSorterTask_H 1

#include "PndSdsPixelDigiSorterTask.h"
#include "TString.h"

class PndMvdPixelDigiSorterTask : public PndSdsPixelDigiSorterTask {
 public:
  /** Default constructor **/
  PndMvdPixelDigiSorterTask();

  PndMvdPixelDigiSorterTask(Int_t numberOfCells, Double_t widthOfCells);

  /** Destructor **/
  virtual ~PndMvdPixelDigiSorterTask();

  /** Specific Parameter set loading **/
  virtual void SetParContainers();

  virtual InitStatus Init();

  /** Specify the branch & folder names for the I/O **/
  void SetBranchNames(TString inBranchname, TString outBranchname, TString folderName);

  /** Take default naming **/
  virtual void SetBranchNames();

  ClassDef(PndMvdPixelDigiSorterTask, 1);
};

#endif
