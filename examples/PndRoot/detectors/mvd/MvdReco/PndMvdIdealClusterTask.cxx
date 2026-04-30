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
// -----                PndMvdIdealClusterTask source file             -----
// -------------------------------------------------------------------------

#include "PndMvdIdealClusterTask.h"

// -----   Default constructor   -------------------------------------------
PndMvdIdealClusterTask::PndMvdIdealClusterTask() : PndSdsIdealClusterTask() {}

PndMvdIdealClusterTask::PndMvdIdealClusterTask(Double_t radius, Int_t FEcolumns, Int_t FErows, TString geoFile) : PndSdsIdealClusterTask(radius, FEcolumns, FErows, geoFile) {}

// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMvdIdealClusterTask::~PndMvdIdealClusterTask() {}
// -------------------------------------------------------------------------

// -----   Initialization  of Parameter Containers -------------------------
void PndMvdIdealClusterTask::SetParContainers() {}
// -----   Manula I/O folders/branches   ----------------------------------------------------
void PndMvdIdealClusterTask::SetBranchNames(TString inBranchname, TString outHitBranchname, TString outClustBranchname, TString folderName)
{
  fInBranchName = inBranchname;
  fOutBranchName = outHitBranchname;
  fClustBranchName = outClustBranchname;
  fFolderName = folderName;
}

// -----   Default I/O folder/branches   ----------------------------------------------------
void PndMvdIdealClusterTask::SetBranchNames()
{
  fInBranchName = "MVDPixelDigis";
  fOutBranchName = "MVDHitsPixelIdeal";
  fClustBranchName = "MVDPixelClusterCandIdeal";
  fFolderName = "PndMvd";
}

ClassImp(PndMvdIdealClusterTask);
