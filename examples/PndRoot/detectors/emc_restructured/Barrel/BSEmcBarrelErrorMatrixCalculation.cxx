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

//--------------------------------------------------------------------------
// Description:
//	Class PndEmcErrorMatrix
//      Calculate Error Matrix for the given EmcCluster
//      with parametrization defined by the given parameter PndEmcErrorMatrixPar
//
//------------------------------------------------------------------------
#include "BSEmcBarrelErrorMatrixCalculation.h"

#include <math.h>

#include "TMatrixT.h"
#include "TVector3.h"

#include "BSEmcRecoHit.h"

BSEmcBarrelErrorMatrixCalculation::BSEmcBarrelErrorMatrixCalculation() : BSEmcErrorMatrixCalculation() {}

BSEmcBarrelErrorMatrixCalculation::~BSEmcBarrelErrorMatrixCalculation() {}

Double_t BSEmcBarrelErrorMatrixCalculation::GetPosition2Error(Double_t t_energy) const
{
  return pow(fPosition2ParA, 2.) / pow(t_energy, fPosition2Power) + pow(fPosition2Const, 2.);
}

TMatrixD BSEmcBarrelErrorMatrixCalculation::GetTransformationMatrix(const BSEmcRecoHit *t_recoHit) const
{
  Double_t clusterTheta = t_recoHit->GetPosition().Theta();
  Double_t sin_theta = sin(clusterTheta);

  TMatrixD trans(4, 3);

  trans(0, 0) = 1.; // dE/dE
  trans(0, 1) = 0.; // dE/dz
  trans(0, 2) = 0.; // dE/dphi

  trans(1, 0) = 0.;                                         // dTheta/dE
  trans(1, 1) = -sin_theta * sin_theta / fDetectorPosition; // dTheta/dz
  trans(1, 2) = 0.;                                         // dTheta/dphi

  trans(2, 0) = 0.; // dPhi/dE
  trans(2, 1) = 0.; // dPhi/dz
  trans(2, 2) = 1.; // dPhi/dphi

  trans(3, 0) = 0.; // dR/dE
  trans(3, 1) = 0.; // dR/dz
  trans(3, 2) = 0.; // dR/dphi

  return trans;
}
