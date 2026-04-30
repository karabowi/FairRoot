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

#include "BSEmcFinishRecoHitProcess.h"

#include "FairParSet.h"
#include "fairlogger/Logger.h"

#include "PndMutableContainerI.h"
#include "PndParameterRegister.h"
#include "PndProcess.h"

#include "BSEmcCorrection.h"
#include "BSEmcErrorCalculationPar.h"
#include "BSEmcErrorMatrixCalculation.h"
#include "BSEmcRecoHit.h"

BSEmcFinishRecoHitProcess::BSEmcFinishRecoHitProcess()
  : PndProcess{"BSEmcFinishRecoHitProcess"}, fECF(nullptr), fErrorCalculator(nullptr), fRecoHitArray(nullptr), fErrorCalculationParName(BSEmcErrorCalculationPar::fgParameterName)
{
}

BSEmcFinishRecoHitProcess::~BSEmcFinishRecoHitProcess() {}

void BSEmcFinishRecoHitProcess::SetDetectorName(const std::string &t_detectorName)
{
  fDetectorName = t_detectorName;
  fErrorCalculationParName += fDetectorName;
  fParameterList.push_back(fErrorCalculationParName);
}

void BSEmcFinishRecoHitProcess::SetupParameters(const PndParameterRegister *t_parameterRegister)
{
  if (fErrorCalculator != nullptr) {
    BSEmcErrorCalculationPar *parameter = dynamic_cast<BSEmcErrorCalculationPar *>(t_parameterRegister->GetParameter(fErrorCalculationParName));
    LOG(debug) << "BSEmcFinishRecoHitProcess::SetupParameters() - Fetching " << fErrorCalculationParName;
    fErrorCalculator->SetupParameters(parameter);
  }
}

// -------------------------------------------------------------------------
void BSEmcFinishRecoHitProcess::RequestDataContainer(PndContainerRegister *t_register)
{
  t_register->Request({fRecoHitBranchname, "BSEmcRecoHit", kTRUE, kTRUE});
}

// -------------------------------------------------------------------------
void BSEmcFinishRecoHitProcess::GetDataContainer(PndContainerRegister *t_register)
{
  fRecoHitArray = t_register->GetOutput<BSEmcRecoHit>(fRecoHitBranchname);
}

void BSEmcFinishRecoHitProcess::Process()
{
  Apply(fRecoHitArray->GetVector());
}

void BSEmcFinishRecoHitProcess::Apply(const std::vector<BSEmcRecoHit *> &t_recoHitCont) const
{
  for (BSEmcRecoHit *recoHit : t_recoHitCont) {
    if (fECF != nullptr) {
      fECF->Correct(recoHit);
    }

    if (fErrorCalculator != nullptr) {
      recoHit->SetCovP4(fErrorCalculator->Get4MomentumErrorMatrix(recoHit));
      recoHit->SetCov7(fErrorCalculator->GetErrorP7(recoHit));
    }
  }
}
