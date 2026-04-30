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
// -----                PndLmdIdealClusterTask source file             -----
// -------------------------------------------------------------------------

#include "PndLmdIdealClusterTask.h"

// -----   Default constructor   -------------------------------------------
PndLmdIdealClusterTask::PndLmdIdealClusterTask() {}
// -------------------------------------------------------------------------

PndLmdIdealClusterTask::PndLmdIdealClusterTask(Double_t radius, Int_t FEcolumns, Int_t FErows, TString geoFile) : PndSdsIdealClusterTask(radius, FEcolumns, FErows, geoFile) {}

// -----   Destructor   ----------------------------------------------------
PndLmdIdealClusterTask::~PndLmdIdealClusterTask() {}
// -------------------------------------------------------------------------

void PndLmdIdealClusterTask::SetBranchNames(TString inBranchname, TString outHitBranchname, TString outClustBranchname, TString folderName)
{
  fInBranchName = inBranchname;
  fOutBranchName = outHitBranchname;
  fClustBranchName = outClustBranchname;
  fFolderName = folderName;
}

void PndLmdIdealClusterTask::SetBranchNames()
{
  fInBranchName = "LMDPixelDigis";
  fOutBranchName = "LMDIdealClusterHit";
  fClustBranchName = "LMBCluster";
  fFolderName = "PndLmd";
}

ClassImp(PndLmdIdealClusterTask);
