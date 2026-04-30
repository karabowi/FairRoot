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

#include "BSEmcDigiTimeBuncherProcess.h"

#include <vector>

#include "TClonesArray.h"
#include "TObject.h"
#include "TString.h"

#include "FairLink.h"
#include "FairParSet.h"
#include "FairRootManager.h"
#include "fairlogger/Logger.h"

#include "PndContainerI.h"
#include "PndMutableContainerI.h"
#include "PndParameterRegister.h"

#include "BSEmcClusteringPar.h"
#include "BSEmcDataBranchNames.h"
#include "BSEmcDigi.h"

BSEmcDigiTimeBuncherProcess::BSEmcDigiTimeBuncherProcess(const TString &t_sourcebranchname) : PndProcess("BSEmcDigiTimeBuncherProcess"), fSourceBranchName(t_sourcebranchname) {}

BSEmcDigiTimeBuncherProcess::~BSEmcDigiTimeBuncherProcess() {}

void BSEmcDigiTimeBuncherProcess::SetDetectorName(const std::string &t_detectorName)
{
  fDetectorName = t_detectorName;
  fClusteringParName = BSEmcClusteringPar::fgParameterName + fDetectorName;

  fParameterList.push_back(fClusteringParName);
}

void BSEmcDigiTimeBuncherProcess::SetupParameters(const PndParameterRegister *t_paramRegister)
{
  BSEmcClusteringPar *clusteringPar = dynamic_cast<BSEmcClusteringPar *>(t_paramRegister->GetParameter(fClusteringParName));
  fTimebunchCutTime = clusteringPar->GetClusterActiveTime() * 1e9;
}

// -------------------------------------------------------------------------
void BSEmcDigiTimeBuncherProcess::RequestDataContainer(PndContainerRegister *t_register)
{
  // Lets use the defaults set by the task
  fSourceBranchName = (fSourceBranchName == "") ? "Sorted" + BSEmcDataBranchNames::fgDigiBranchName + this->fDetectorName : fSourceBranchName;
  fTargetBranchName = (fTargetBranchName == "") ? "Timebunched" + BSEmcDataBranchNames::fgDigiBranchName + this->fDetectorName : fSourceBranchName;

  t_register->Request({fSourceBranchName, "BSEmcDigi", kFALSE, kFALSE}); // Only to fail, if not existent
  t_register->Request({fTargetBranchName, "BSEmcDigi", kTRUE, kTRUE});
}

// -------------------------------------------------------------------------
void BSEmcDigiTimeBuncherProcess::GetDataContainer(PndContainerRegister *t_register)
{
  fDigis = t_register->GetOutput<BSEmcDigi>(fTargetBranchName);
  t_register->SetAsDefaultBranchFor(fTargetBranchName, BSEmcDigi().ClassName());
}

void BSEmcDigiTimeBuncherProcess::Process()
{
  fDigis->Reset();
  TClonesArray *tca = FairRootManager::Instance()->GetData(fSourceBranchName, fDigiFunctor.get(), fTimebunchCutTime);

  Int_t nEntries = tca->GetEntriesFast();
  LOG(debug) << "BSEmcDigiTimeBuncherProcess::Process() pushing " << nEntries << " digis from " << fSourceBranchName << " into " << fDigis->GetBranchName();
  for (Int_t i = 0; i < nEntries; ++i) {
    BSEmcDigi *tmp = dynamic_cast<BSEmcDigi *>(tca->At(i));
    BSEmcDigi *newDigi = fDigis->CreateCopy(*tmp);
    FairLink link{fDigis->GetBranchName(), i};
    link.SetEntry(fEvent);
    newDigi->SetEntryNr(link);
  }
  tca->Clear();
  tca->Compress();
  ++fEvent;
}

ClassImp(BSEmcDigiTimeBuncherProcess)
