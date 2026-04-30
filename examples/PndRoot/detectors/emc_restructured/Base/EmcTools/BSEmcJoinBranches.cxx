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

#include "BSEmcJoinBranches.h"

#include "TClonesArray.h"
#include "TObject.h"
#include "TString.h"

#include "FairRootManager.h"
#include "fairlogger/Logger.h"

BSEmcJoinBranches::BSEmcJoinBranches() : PndPersistencyTask(), fSourceBranchNames({}), fTargetBranchName(""), fSourceArrays({}), fTargetArray()
{
  SetPersistency(kTRUE);
}

BSEmcJoinBranches::BSEmcJoinBranches(const std::vector<TString> &t_srcbranchnames, const TString &t_targetbranchname, Bool_t t_storeTarget)
  : PndPersistencyTask(), fSourceBranchNames(t_srcbranchnames), fTargetBranchName(t_targetbranchname), fSourceArrays({}), fTargetArray(nullptr)
{
  SetPersistency(t_storeTarget);
}

BSEmcJoinBranches::~BSEmcJoinBranches() {}

InitStatus BSEmcJoinBranches::Init()
{
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (ioman == nullptr) {
    LOG(error) << "BSEmcJoinBranches::Init: "
               << "RootManager not instantiated!";
    return kFATAL;
  }
  for (const TString &branchname : fSourceBranchNames) {
    TClonesArray *tmp = dynamic_cast<TClonesArray *>(ioman->GetObject(branchname));
    if (tmp == nullptr) {
      LOG(error) << "BSEmcJoinBranches::Init: " << branchname << "fInputArray not instantiated!";
      return kFATAL;
    }
    fSourceArrays.push_back(tmp);
  }
  if (fSourceArrays[0] != nullptr) {
    fTargetArray = new TClonesArray(fSourceArrays[0]->GetClass());
    ioman->Register(fTargetBranchName, "Emc", fTargetArray, GetPersistency());
  }
  return kSUCCESS;
}

void BSEmcJoinBranches::Exec(Option_t * /*unused*/)
{
  // Reset output array
  if (fTargetArray == nullptr) {
    Fatal("Exec", "No fTargetArray");
  }

  fTargetArray->Delete();

  for (TClonesArray *array : fSourceArrays) {
    fTargetArray->AbsorbObjects(array);
  }
}
