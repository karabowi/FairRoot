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

// -------------------------------------------------------------------------
// -----                PndMvdPixelDigiSorterTask source file             -----
// -------------------------------------------------------------------------

#include "PndMvdPixelDigiSorterTask.h"
#include "PndSdsPixelDigiPar.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"

// -----   Default constructor   -------------------------------------------
PndMvdPixelDigiSorterTask::PndMvdPixelDigiSorterTask() : PndSdsPixelDigiSorterTask("MVD Pixel Digi Sorter")
{

  fOverwriteParams = kFALSE;
  fPersistance = kTRUE;
  if (fVerbose > 2)
    std::cout << "MVD Pixel Digi Sorter created, Parameters will be taken from RTDB" << std::endl;
}

PndMvdPixelDigiSorterTask::PndMvdPixelDigiSorterTask(Int_t numberOfCells, Double_t widthOfCells) : PndSdsPixelDigiSorterTask(numberOfCells, widthOfCells)
{

  fOverwriteParams = kTRUE;
  fPersistance = kTRUE;
  if (fVerbose > 2)
    std::cout << "MVD Pixel Digi Sorter created, Parameters will be taken from RTDB" << std::endl;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMvdPixelDigiSorterTask::~PndMvdPixelDigiSorterTask() {}

// -----   Public method Init   --------------------------------------------
InitStatus PndMvdPixelDigiSorterTask::Init()
{
  InitStatus status = PndSdsPixelDigiSorterTask::Init();
  return status;
}

// -----   Initialization  of Parameter Containers -------------------------
void PndMvdPixelDigiSorterTask::SetParContainers()
{
  // called before Init()
  // Get Base Container
  FairRun *ana = FairRun::Instance();
  FairRuntimeDb *rtdb = ana->GetRuntimeDb();
  fDigiPar = (PndSdsPixelDigiPar *)(rtdb->getContainer("MVDPixelDigiPar"));
  PndSdsPixelDigiSorterTask::SetParContainers();
}

// -------------------------------------------------------------------------
void PndMvdPixelDigiSorterTask::SetBranchNames(TString inBranchname, TString outBranchname, TString folderName)
{
  fInBranchName = inBranchname;
  fOutBranchName = outBranchname;
  fFolderName = folderName;
  SetInBranchId();
}

// -------------------------------------------------------------------------
void PndMvdPixelDigiSorterTask::SetBranchNames()
{
  if (fVerbose > 1)
    Info("SetBranchNames", "Set Mvd Pixel names.");
  fInBranchName = "MVDPixelDigis";
  fOutBranchName = "MVDSortedPixelDigis";
  fFolderName = "PndMvd";
  SetInBranchId();
}

ClassImp(PndMvdPixelDigiSorterTask);
