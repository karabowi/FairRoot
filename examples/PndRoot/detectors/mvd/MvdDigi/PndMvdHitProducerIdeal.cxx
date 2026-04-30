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
// -----               PndMvdHitProducerIdeal source file             -----
// -------------------------------------------------------------------------

#include "PndMvdHitProducerIdeal.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"

// -----   Default constructor   -------------------------------------------
PndMvdHitProducerIdeal::PndMvdHitProducerIdeal() : PndSdsHitProducerIdeal("Ideal MVD Hit Producer")
{
  fInBranchName = "MVDPoint";
  SetPersistency(kTRUE);
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMvdHitProducerIdeal::~PndMvdHitProducerIdeal() {}

void PndMvdHitProducerIdeal::SetBranchNames(TString inBranchname, TString outBranchname, TString folderName)
{
  fInBranchName = inBranchname;
  fOutBranchName = outBranchname;
  fFolderName = folderName;
  SetInBranchId();
}

void PndMvdHitProducerIdeal::SetBranchNames()
{
  fInBranchName = "MVDPoint";
  fOutBranchName = "MVDHit";
  fFolderName = "PndMvd";
  SetInBranchId();
}

ClassImp(PndMvdHitProducerIdeal)
