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

#include "BSEmcFwEndcapAPDAndVPTTWaveformGenerator.h"

#include "PndConstContainerI.h"
#include "PndMutableContainerI.h"

#include "BSEmcFwEndcapCosmicWaveformGenPar.h"
#include "BSEmcIdealDigitizationProcess.h"
#include "BSEmcMCDeposit.h"

#include "PndParameterRegister.h"

BSEmcFwEndcapAPDAndVPTTWaveformGenerator::BSEmcFwEndcapAPDAndVPTTWaveformGenerator() : PndProcess{"BSEmcFwEndcapAPDAndVPTTWaveformGenerator"} {}

BSEmcFwEndcapAPDAndVPTTWaveformGenerator::~BSEmcFwEndcapAPDAndVPTTWaveformGenerator() {}

void BSEmcFwEndcapAPDAndVPTTWaveformGenerator::SetDetectorName(const std::string &t_detectorname)
{
  fDetectorName = t_detectorname;
  fAPDWaveformGenerator->SetWaveformGenParName(BSEmcFwEndcapCosmicWaveformGenPar::fgParameterName + "APD");
  fAPDWaveformGenerator->SetPulseshapeLUTName("LUTCosmicWaveformSADC2022FwEC_APD.txt");
  fAPDWaveformGenerator->SetDetectorName("FwEndcap");

  fVPTTWaveformGenerator->SetWaveformGenParName(BSEmcFwEndcapCosmicWaveformGenPar::fgParameterName + "VPTT");
  fVPTTWaveformGenerator->SetPulseshapeLUTName("LUTCosmicWaveformSADC2022FwEC_VPTT.txt");
  fVPTTWaveformGenerator->SetDetectorName("FwEndcap");

  for (const std::string &param : fVPTTWaveformGenerator->GetListOfRequiredParameters()) {
    fParameterList.push_back(param);
  }
  for (const std::string &param : fAPDWaveformGenerator->GetListOfRequiredParameters()) {
    fParameterList.push_back(param);
  }
  if (fWaveformBranchName == "") {
    fWaveformBranchName = BSEmcDataBranchNames::fgMultiWaveformBranchName + this->fDetectorName;
  }
  if (fWaveformDataBranchName == "") {
    fWaveformDataBranchName = BSEmcDataBranchNames::fgWaveformDataBranchName + this->fDetectorName;
  }

  fAPDWaveformGenerator->SetWaveformBranchName(fWaveformBranchName);
  fAPDWaveformGenerator->SetWaveformDataBranchName(fWaveformDataBranchName);

  fVPTTWaveformGenerator->SetWaveformBranchName(fWaveformBranchName);
  fVPTTWaveformGenerator->SetWaveformDataBranchName(fWaveformDataBranchName);
}

void BSEmcFwEndcapAPDAndVPTTWaveformGenerator::SetupParameters(const PndParameterRegister *t_paramRegister)
{
  fAPDWaveformGenerator->SetupParameters(t_paramRegister);
  fVPTTWaveformGenerator->SetupParameters(t_paramRegister);
}

// -------------------------------------------------------------------------
void BSEmcFwEndcapAPDAndVPTTWaveformGenerator::RequestDataContainer(PndContainerRegister *t_register)
{
  // Lets use the defaults set by the task
  fAPDWaveformGenerator->RequestDataContainer(t_register);
  fVPTTWaveformGenerator->RequestDataContainer(t_register);
  t_register->Request({fMCDepositBranchName, "BSEmcMCDeposit", kFALSE, kFALSE});
}

// -------------------------------------------------------------------------
void BSEmcFwEndcapAPDAndVPTTWaveformGenerator::GetDataContainer(PndContainerRegister *t_register)
{
  fAPDWaveformGenerator->GetDataContainer(t_register);
  fVPTTWaveformGenerator->GetDataContainer(t_register);
  fMCDepositArray = t_register->GetInput<BSEmcMCDeposit>(fMCDepositBranchName);
}

void BSEmcFwEndcapAPDAndVPTTWaveformGenerator::Process()
{
  Int_t nDeposits = fMCDepositArray->GetSize();

  for (Int_t depositIndex = 0; depositIndex < nDeposits; ++depositIndex) {
    const BSEmcMCDeposit *deposit = fMCDepositArray->GetConstElementPtr(depositIndex);
    if (IsVpttDeposit(deposit)) {
      fVPTTWaveformGenerator->AddDepositData(deposit, depositIndex);
    } else {
      fAPDWaveformGenerator->AddDepositData(deposit, depositIndex);
    }
  }
}

Bool_t BSEmcFwEndcapAPDAndVPTTWaveformGenerator::IsVpttDeposit(const BSEmcMCDeposit *t_deposit) const
{
  return std::binary_search(fVpttIds.begin(), fVpttIds.end(), t_deposit->GetDetectorID());
}
