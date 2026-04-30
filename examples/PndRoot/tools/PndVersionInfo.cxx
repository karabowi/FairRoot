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

#include "PndVersionInfo.h"
#include <Rtypes.h>
#include <TObject.h>
#include <iostream>
#include "PandaVersion.h"
#include "FairLogger.h"

// ClassImp(PndVersionInfo);

PndVersionInfo::PndVersionInfo()
{
  fGitRev = Panda::GIT_REV;
  fGitTag = Panda::GIT_TAG;
  fGitBranch = Panda::GIT_BRANCH;

  // Always print on loading the library
  Print();
}

void PndVersionInfo::Print()
{
  LOG(info) << "git commit hash:" << fGitRev;
  LOG(info) << "git tag        :" << fGitTag;
  LOG(info) << "git branch     :" << fGitBranch;
}
