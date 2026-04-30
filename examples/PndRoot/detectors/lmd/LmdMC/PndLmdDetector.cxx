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

#include "PndLmdDetector.h"

#include "TClonesArray.h"
#include "TGeoPhysicalNode.h"
#include "TVirtualMC.h"

#include <iostream>

// -----   Default constructor   -------------------------------------------
PndLmdDetector::PndLmdDetector()
{
  SetDetectorID(DetectorId::kLUMI);
}

// -----   Standard constructor   ------------------------------------------
PndLmdDetector::PndLmdDetector(const char *name, Bool_t active) : PndSdsDetector(name, active)
{
  SetDetectorID(DetectorId::kLUMI);
}

// -----   Destructor   ----------------------------------------------------
PndLmdDetector::~PndLmdDetector() {}

void PndLmdDetector::SetBranchNames(char *outBranchname, char *folderName)
{
  fOutBranchName = outBranchname;
  fFolderName = folderName;
}

void PndLmdDetector::SetBranchNames()
{
  fOutBranchName = "LMDPoint";
  fFolderName = "PndLmd";
}

void PndLmdDetector::SetDefaultSensorNames()
{
  fListOfSensitives.push_back("LumActive");
}

ClassImp(PndLmdDetector);
