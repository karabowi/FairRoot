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

/////////////////////////////////////////////////////////////
//
//  PndEmcHitProducer
//
//  Filler of PndEmcHit
//
//  Created 14/08/06  by S.Spataro
//
///////////////////////////////////////////////////////////////
#include "BSEmcBarrelZDependentMCDepositProductionProcess.h"

#include <memory>
#include <stdlib.h>
#include <string>
#include <vector>

#include "TFile.h"
#include "TString.h"
#include "TSystem.h"
#include "TVector3.h"

#include "FairParSet.h"
#include "fairlogger/Logger.h"

#include "PndParameterRegister.h"

#include "BSEmcBarrelDigiNonuniformityPar.h"
#include "BSEmcCrystalPositionPar.h"
#include "BSEmcMCPar.h"
#include "BSEmcMCPoint.h"

BSEmcBarrelZDependentMCDepositProductionProcess::BSEmcBarrelZDependentMCDepositProductionProcess() : BSEmcMCDepositProductionProcess()
{
  fParameterList.push_back("BarrelDigiNonuniformityPar");
}

void BSEmcBarrelZDependentMCDepositProductionProcess::SetupParameters(const PndParameterRegister *t_paramRegister)
{
  BSEmcMCDepositProductionProcess::SetupParameters(t_paramRegister);
  LOG(debug) << "BSEmcBarrelZDependentMCDepositProductionProcess::SetParContainers()"
             << " parameters: ";
  BSEmcMCPar *parameter = dynamic_cast<BSEmcMCPar *>(t_paramRegister->GetParameter(fMCParName.c_str()));
  fNonUniformityPar = dynamic_cast<BSEmcBarrelDigiNonuniformityPar *>(t_paramRegister->GetParameter("BarrelDigiNonuniformityPar"));
  Setup(parameter);
}

// -----------------------------------------------------------------------------
void BSEmcBarrelZDependentMCDepositProductionProcess::Setup(BSEmcMCPar *t_par)
{

  LOG(debug) << "BSEmcBarrelZDependentMCDepositProductionProcess::SetupNonUniformityParameters()"
             << " parameters: ";
  TString fileName = gSystem->Getenv("VMCWORKDIR");

  fileName += TString(t_par->GetNonUniformityFile());
  // fileName += ".root";
  LOG(debug) << "BSEmcBarrelZDependentMCDepositProductionProcess::SetupNonUniformityParameters()"
             << " parameters: "
             << " NonUniformityFile = " << TString(t_par->GetNonUniformityFile());
  std::unique_ptr<TFile> nonuniformityfile = std::unique_ptr<TFile>{new TFile(fileName)};
  if (nonuniformityfile == nullptr) {
    LOG(error) << "BSEmcBarrelZDependentMCDepositProductionProcess::SetupNonUniformityParameters()"
               << " parameters: "
               << "Could not open file " << fileName.Data() << " for Nonuniformity Information";
    throw std::exception(); // Exception is catched in PndProcessTask::Init(), which will return kERROR InitStatus
  } else {
    BSEmcBarrelDigiNonuniParObject *parObject{nullptr};
    nonuniformityfile->GetObject("BSEmcBarrelDigiNonuniParObject", parObject);
    nonuniformityfile->Close();
    if (parObject == nullptr) {
      LOG(error) << "BSEmcBarrelZDependentMCDepositProductionProcess::SetupNonUniformityParameters()"
                 << " parameters: "
                 << "Could not get Nonuniformity information from file " << fileName.Data();
      throw std::exception(); // Exception is catched in PndProcessTask::Init(), which will return kERROR InitStatus
    } else {
      fNonUniformityPar->SetNonuniParObject(parObject);
    }
  }
}

// -----------------------------------------------------------------------------
Bool_t BSEmcBarrelZDependentMCDepositProductionProcess::SkipPoint(const BSEmcMCPoint *t_point) const
{
  if (BSEmcMCDepositProductionProcess::SkipPoint(t_point)) {
    return kTRUE;
  }
  if (fIsDayOneSetup) {
    const Float_t phi = t_point->GetPosition().Phi() * TMath::RadToDeg();
    if (abs(phi - 90) < 22.5) {
      return kTRUE;
    }
    if (abs(phi - 270) < 22.5) {
      return kTRUE;
    }
    if (abs(phi + 90) < 22.5) {
      return kTRUE;
    }
  }
  return kFALSE;
}

// -----------------------------------------------------------------------------
Double_t BSEmcBarrelZDependentMCDepositProductionProcess::GetEnergy(const BSEmcMCPoint *t_point) const
{
  const Int_t DetId = t_point->GetDetectorID();
  const BSEmcCrystalPositionData &tmpXtal = fPositionPar->GetPositionData(DetId);
  TVector3 pointvec{0, 0, 0};
  t_point->Position(pointvec);
  const TVector3 frontvec = tmpXtal.GetFrontCentre();
  const TVector3 normvec = tmpXtal.GetNormalToFrontFace();
  const TVector3 distvec = pointvec - frontvec;
  const Double_t zpos = distvec.Dot(normvec);
  Double_t c[3] = {0, 0, 0};
  fNonUniformityPar->GetNonuniformityParameters(DetId, c);

  const Double_t energyscalefactor = c[0] + zpos * (c[1] + zpos * c[2]);
  LOG(trace) << "BSEmcBarrelZDependentMCDepositProductionProcess::AddEnergyToEventStorage() - "

             << "point with detID " << DetId << " has z Position " << zpos << " and energyloss " << t_point->GetEnergyLoss() << " and scaled with  " << energyscalefactor
             << "\nfront is at x: " << frontvec.X() << " y: " << frontvec.Y() << " z: " << frontvec.Z();
  return t_point->GetEnergyLoss() * energyscalefactor;
}
