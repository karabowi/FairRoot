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
//	Class BSEmcErrorMatrixCalculation
//      Calculate Error Matrix for the given BSEmcRecoHit
//      with parametrization defined by the given parameter BSEmcErrorCalculationPar
//
//------------------------------------------------------------------------

#include "BSEmcErrorMatrixCalculation.h"

#include <cmath>

#include "TMatrixT.h"
#include "TVector3.h"

#include "fairlogger/Logger.h"

#include "BSEmcErrorCalculationPar.h"
#include "BSEmcRecoHit.h"

BSEmcErrorMatrixCalculation::BSEmcErrorMatrixCalculation() {}

void BSEmcErrorMatrixCalculation::SetupParameters(BSEmcErrorCalculationPar *t_par)
{
  fScaleFactor = t_par->GetScaleFactor();
  fMinEnergyCutOff = t_par->GetMinEnergyCutOff();
  fMaxEnergyCutOff = t_par->GetMaxEnergyCutOff();
  fDetectorPosition = t_par->GetDetectorPosition();
  fEnergyParA = t_par->GetEnergyParA();
  fEnergyPower = t_par->GetEnergyPower();
  fEnergyConst = t_par->GetEnergyConst();
  fEnergyQuadr = t_par->GetEnergyQuadr();
  fPosition1ParA = t_par->GetPosition1ParA();
  fPosition1Power = t_par->GetPosition1Power();
  fPosition1Const = t_par->GetPosition1Const();
  fPosition2ParA = t_par->GetPosition2ParA();
  fPosition2Power = t_par->GetPosition2Power();
  fPosition2Const = t_par->GetPosition2Const();
  LOG(debug) << "void BSEmcErrorMatrixCalculation::SetupParameters() - t_par " << t_par << ", fScaleFactor " << fScaleFactor << ", fMinEnergyCutOff " << fMinEnergyCutOff
             << ", fMaxEnergyCutOff " << fMaxEnergyCutOff << ", fDetectorPosition " << fDetectorPosition << ", fEnergyParA " << fEnergyParA << ", fEnergyPower " << fEnergyPower
             << ", fEnergyConst " << fEnergyConst;
}

BSEmcErrorMatrixCalculation::~BSEmcErrorMatrixCalculation() {}

/**
 * @brief
 *
 * @param recoHit
 * @return TMatrixD 4x4 matrix
 */
TMatrixD BSEmcErrorMatrixCalculation::CalculateErrorMatrix(const BSEmcRecoHit *t_recoHit) const
{
  Double_t recoHitEnergy = t_recoHit->GetRawEnergy();

  // functions used for parametrization
  Double_t energy = recoHitEnergy;
  if (recoHitEnergy > fMaxEnergyCutOff) {
    energy = fMaxEnergyCutOff; // Why?
  }
  if (recoHitEnergy < fMinEnergyCutOff) {
    energy = fMinEnergyCutOff;
  }

  // energy error
  // Energy: Delta(E)/E = (a^2/E^power) + const^2 + (quadr/E)^2
  Double_t errEnergy = GetEnergyError(energy, recoHitEnergy);

  // fPosition coordinate errors
  // fPosition: Delta(x)=(a*a/E^power) + const^2
  Double_t position1Err = GetPosition1Error(energy);
  Double_t position2Err = GetPosition2Error(energy);

  // error matrix for E, fPosition1, fPosition2 and arbitrary fPosition3
  TMatrixD theError(3, 3);
  theError(0, 0) = errEnergy * errEnergy;
  theError(1, 1) = position1Err * position1Err;
  theError(2, 2) = position2Err * position2Err;

  const TMatrixD &trans = GetTransformationMatrix(t_recoHit);
  // Error matrix in (E, theta, phi, R)
  const TMatrixD &errorMatrix = SymmetricMultiplication(theError, trans);

  return errorMatrix;
}

Double_t BSEmcErrorMatrixCalculation::GetEnergyError(Double_t t_energy, Double_t t_recoHitEnergy) const
{
  Double_t errEnergy = fEnergyParA * fEnergyParA / pow(t_energy, fEnergyPower) + pow(fEnergyConst, 2.0) + pow(fEnergyQuadr / t_energy, 2.0);
  return errEnergy * t_recoHitEnergy;
}

Double_t BSEmcErrorMatrixCalculation::GetPosition1Error(Double_t t_energy) const
{
  Double_t position1Err = pow(fPosition1ParA, 2.) / pow(t_energy, fPosition1Power) + pow(fPosition1Const, 2.);
  return position1Err * fScaleFactor;
}

Double_t BSEmcErrorMatrixCalculation::GetPosition2Error(Double_t t_energy) const
{
  Double_t position2Err = pow(fPosition2ParA, 2.) / pow(t_energy, fPosition2Power) + pow(fPosition2Const, 2.);
  return position2Err * fScaleFactor;
}
/**
 * @brief
 *
 * @param recoHit
 * @return TMatrixD 4x3 matrix
 */
TMatrixD BSEmcErrorMatrixCalculation::GetTransformationMatrix(const BSEmcRecoHit *t_recoHit) const
{
  Double_t recoHitTheta = t_recoHit->GetPosition().Theta();
  Double_t recoHitPhi = t_recoHit->GetPosition().Phi();

  TMatrixD trans(4, 3);
  Double_t sin_theta = sin(recoHitTheta);
  Double_t sin_phi = sin(recoHitPhi);
  Double_t cos_theta = cos(recoHitTheta);
  Double_t cos_phi = cos(recoHitPhi);
  Double_t R = fabs(fScaleFactor * 100. / cos_theta); // a bit ugly, but theta is never 90 deg for these components
  // trans(row, col)
  trans(0, 0) = 1.; // dE/dE
  trans(0, 1) = 0.; // dE/dx
  trans(0, 2) = 0.; // dE/dy

  trans(1, 0) = 0.;                      // dTheta/dE
  trans(1, 1) = cos_theta * cos_phi / R; // dTheta/dx
  trans(1, 2) = cos_theta * sin_phi / R; // dTheta/dy

  trans(2, 0) = 0.;                         // dPhi/dE
  trans(2, 1) = -sin_phi / (R * sin_theta); // dPhi/dx
  trans(2, 2) = cos_phi / (R * sin_theta);  // dPhi/dy

  trans(3, 0) = 0.; // dR/dE
  // trans(4,2)= sin_theta * cos_phi /R;  //dR/dx
  // trans(4,3)= sin_theta * sin_phi /R;  //dR/dy
  trans(3, 1) = 0.0; // dR/dx
  trans(3, 2) = 0.0; // dR/dy

  return trans;
}

/**
 * @brief
 *
 * @param recoHit
 * @return TMatrixD 4x4
 */
TMatrixD BSEmcErrorMatrixCalculation::Get4MomentumErrorMatrix(const BSEmcRecoHit *t_recoHit) const
{
  // Conversion from (E, theta, phi, r) to ( px, py, pz, E )

  Double_t z_recoHit = t_recoHit->GetPosition().Z();
  Double_t perp = t_recoHit->GetPosition().Perp();
  Double_t mag = t_recoHit->GetPosition().Mag();
  Double_t cos_theta = z_recoHit / mag;
  Double_t sin_theta = perp / mag;
  Double_t sin_phi = t_recoHit->GetPosition().Y() / perp;
  Double_t cos_phi = t_recoHit->GetPosition().X() / perp;
  Double_t e = t_recoHit->GetRawEnergy();
  Double_t p = e;

  // Create a matrix to transform the error matrix
  TMatrixD toComp(4, 4);
  toComp(0, 0) = sin_theta * cos_phi * e / p;
  toComp(0, 1) = p * cos_theta * cos_phi;
  toComp(0, 2) = -p * sin_theta * sin_phi;
  toComp(0, 3) = 0;
  toComp(1, 0) = sin_theta * sin_phi * e / p;
  toComp(1, 1) = p * cos_theta * sin_phi;
  toComp(1, 2) = p * sin_theta * cos_phi;
  toComp(1, 3) = 0;
  toComp(2, 0) = cos_theta * e / p;
  toComp(2, 1) = -p * sin_theta;
  toComp(2, 2) = 0;
  toComp(2, 3) = 0;
  toComp(3, 0) = 1;
  toComp(3, 1) = 0;
  toComp(3, 2) = 0;
  toComp(3, 3) = 0;

  const TMatrixD &tmpMatrix = CalculateErrorMatrix(t_recoHit);
  const TMatrixD &errorMatrix = SymmetricMultiplication(tmpMatrix, toComp);

  return errorMatrix;
}

/**
 * @brief
 *
 * @param recoHit
 * @return TMatrixD 7x7
 */
TMatrixD BSEmcErrorMatrixCalculation::GetErrorP7(const BSEmcRecoHit *t_recoHit) const
{
  // Conversion from (E, theta, phi, r) to ( x, y, z, px, py, pz, E )

  Double_t z_recoHit = t_recoHit->GetPosition().Z();
  Double_t perp = t_recoHit->GetPosition().Perp();
  Double_t mag = t_recoHit->GetPosition().Mag();
  Double_t cos_theta = z_recoHit / mag;
  Double_t sin_theta = perp / mag;
  Double_t sin_phi = t_recoHit->GetPosition().Y() / perp;
  Double_t cos_phi = t_recoHit->GetPosition().X() / perp;
  Double_t e = t_recoHit->GetRawEnergy();
  Double_t p = e;

  // Create a matrix to transform the error matrix
  TMatrixD toComp(7, 4);
  toComp(0, 0) = 0;
  toComp(0, 1) = mag * cos_theta * cos_phi;
  toComp(0, 2) = -mag * sin_theta * sin_phi;
  toComp(0, 3) = sin_theta * cos_phi;
  toComp(1, 0) = 0;
  toComp(1, 1) = mag * cos_theta * sin_phi;
  toComp(1, 2) = mag * sin_theta * cos_phi;
  toComp(1, 3) = sin_theta * sin_phi;
  toComp(2, 0) = 0;
  toComp(2, 1) = -mag * sin_theta;
  toComp(2, 2) = 0;
  toComp(2, 3) = cos_theta;
  toComp(3, 0) = sin_theta * cos_phi;
  toComp(3, 1) = p * cos_theta * cos_phi;
  toComp(3, 2) = -p * sin_theta * sin_phi;
  toComp(3, 3) = 0;
  toComp(4, 0) = sin_theta * sin_phi;
  toComp(4, 1) = p * cos_theta * sin_phi;
  toComp(4, 2) = p * sin_theta * cos_phi;
  toComp(4, 3) = 0;
  toComp(5, 0) = cos_theta;
  toComp(5, 1) = -p * sin_theta;
  toComp(5, 2) = 0;
  toComp(5, 3) = 0;
  toComp(6, 0) = 1;
  toComp(6, 1) = 0;
  toComp(6, 2) = 0;
  toComp(6, 3) = 0;

  const TMatrixD &tmpMatrix = CalculateErrorMatrix(t_recoHit);
  const TMatrixD &errorMatrix = SymmetricMultiplication(tmpMatrix, toComp);

  return errorMatrix;
}

// Function is copied from BbrGeom/BbrError.cc
// It does the same as m1*mat*m1^T
// but with assumption that mat, and output matrix are symmetric

TMatrixD BSEmcErrorMatrixCalculation::SymmetricMultiplication(const TMatrixD &t_mat, const TMatrixD &t_m1) const
{
  TMatrixD result(t_m1.GetNrows(), t_m1.GetNrows());

  TMatrixD temp = t_m1 * t_mat;
  Double_t tmp = NAN;

  for (Int_t r = 0; r < t_m1.GetNrows(); r++) {
    for (Int_t c = 0; c <= r; c++) {
      tmp = 0.;
      for (Int_t k = 0; k < t_m1.GetNcols(); k++) {
        tmp += temp(r, k) * t_m1(c, k);
      }
      result(r, c) = tmp;
      // Modification from original code, to make output matrix explicitly symmetric
      // In original babar code symmetric matrix were stored as lower triangular.
      if (r != c) {
        result(c, r) = tmp;
      }
    }
  }

  return result;
}
