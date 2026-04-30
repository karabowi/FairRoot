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

#include "BSEmcPreclusterTimeBuncherProcess.h"

#include <vector>

#include "TClonesArray.h"
#include "TObject.h"
#include "TString.h"

#include "FairParSet.h"
#include "FairRootManager.h"
#include "fairlogger/Logger.h"

#include "PndMutableContainerI.h"
#include "PndParameterRegister.h"

#include "BSEmcClusteringPar.h"
#include "BSEmcDataBranchNames.h"
#include "BSEmcPrecluster.h"

BSEmcPreclusterTimeBuncherProcess::BSEmcPreclusterTimeBuncherProcess(const TString &t_sourceBranchName)
  : PndProcess("BSEmcPreclusterTimeBuncherProcess"), fSourceBranchName(t_sourceBranchName)
{
}

BSEmcPreclusterTimeBuncherProcess::~BSEmcPreclusterTimeBuncherProcess() {}

void BSEmcPreclusterTimeBuncherProcess::SetDetectorName(const std::string &t_detectorName)
{
  fDetectorName = t_detectorName;
  fClusteringParName = BSEmcClusteringPar::fgParameterName + fDetectorName;

  fParameterList.push_back(fClusteringParName);
}

void BSEmcPreclusterTimeBuncherProcess::SetupParameters(const PndParameterRegister *t_paramRegister)
{
  BSEmcClusteringPar *clusteringPar = dynamic_cast<BSEmcClusteringPar *>(t_paramRegister->GetParameter(fClusteringParName));
  fTimebunchCutTime = clusteringPar->GetClusterActiveTime() * 1e9;
}

// -------------------------------------------------------------------------
void BSEmcPreclusterTimeBuncherProcess::RequestDataContainer(PndContainerRegister *t_register)
{
  // Lets use the defaults set by the task
  fSourceBranchName = (fSourceBranchName == "") ? "Sorted" + BSEmcDataBranchNames::fgPreclusterBranchName + this->fDetectorName : fSourceBranchName;
  fTargetBranchName = (fTargetBranchName == "") ? "Timebunched" + BSEmcDataBranchNames::fgPreclusterBranchName + this->fDetectorName : fSourceBranchName;

  t_register->Request({fSourceBranchName, "BSEmcPrecluster", kFALSE, kFALSE}); // Only to fail, if not existent
  t_register->Request({fTargetBranchName, "BSEmcPrecluster", kTRUE, kTRUE});
}

// -------------------------------------------------------------------------
void BSEmcPreclusterTimeBuncherProcess::GetDataContainer(PndContainerRegister *t_register)
{
  fPreclusters = t_register->GetOutput<BSEmcPrecluster>(fTargetBranchName);
  t_register->SetAsDefaultBranchFor(fTargetBranchName, BSEmcPrecluster().ClassName());
}

void BSEmcPreclusterTimeBuncherProcess::Process()
{
  fPreclusters->Reset();
  TClonesArray *tca = FairRootManager::Instance()->GetData(fSourceBranchName, fFunctor.get(), fTimebunchCutTime);

  Int_t nEntries = tca->GetEntriesFast();
  LOG(debug) << "BSEmcPreclusterTimeBuncherProcess::Process() pushing " << nEntries << " precluster from " << fSourceBranchName << " into " << fPreclusters->GetBranchName();
  for (Int_t i = 0; i < nEntries; ++i) {
    BSEmcPrecluster *tmp = dynamic_cast<BSEmcPrecluster *>(tca->At(i));
    fPreclusters->CreateCopy(*tmp);
  }
  tca->Clear();
  tca->Compress();
}

ClassImp(BSEmcPreclusterTimeBuncherProcess)
