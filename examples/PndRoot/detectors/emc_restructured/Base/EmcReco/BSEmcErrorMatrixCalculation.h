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

//#pragma once
#ifndef BSEMCERRORMATRIXCALCULATION_HH
#define BSEMCERRORMATRIXCALCULATION_HH

#include <cstdlib>

#include "RtypesCore.h"
#include "TMatrixD.h"
#include "TMatrixDfwd.h"
#include "TString.h"

#include "BSEmcRecoHit.h"

class BSEmcErrorCalculationPar;
class BSEmcRecoHit;

/**
 * @class BSEmcErrorMatrixCalculation
 * @brief Calculate Error Matrix for the given BSEmcRecoHit with parametrization defined by the given parameter BSEmcErrorCalculationPar
 * @details new version of PndEmcErrorMatrix
 * More information can be found in emc+error+matrix+note.pdf
 * @author Dima Melnychuk
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcErrorMatrixCalculation {
 public:
  // Constructors
  BSEmcErrorMatrixCalculation();
  // Destructor
  virtual ~BSEmcErrorMatrixCalculation();

  void SetupParameters(BSEmcErrorCalculationPar *t_par);

  // Error Matrix for Cluster
  // 4x4 matrix in E, theta, phi, R
  TMatrixD CalculateErrorMatrix(const BSEmcRecoHit *t_recoHit) const;

  // 4x4 matrix in (px, py, pz, E)
  TMatrixD Get4MomentumErrorMatrix(const BSEmcRecoHit *t_recoHit) const;

  // 7x7 matrix in (x, y, z, px, py, pz, E)
  TMatrixD GetErrorP7(const BSEmcRecoHit *t_recoHit) const;

 protected:
  Double_t fScaleFactor{-1};
  Double_t fMinEnergyCutOff{-1};
  Double_t fMaxEnergyCutOff{-1};
  Double_t fDetectorPosition{-1};

  Double_t fEnergyParA{-1};
  Double_t fEnergyPower{-1};
  Double_t fEnergyConst{-1};
  Double_t fEnergyQuadr{-1};
  Double_t fPosition1ParA{-1};
  Double_t fPosition1Power{-1};
  Double_t fPosition1Const{-1};
  Double_t fPosition2ParA{-1};
  Double_t fPosition2Power{-1};
  Double_t fPosition2Const{-1};

  virtual TMatrixD GetTransformationMatrix(const BSEmcRecoHit *t_recoHit) const;
  virtual Double_t GetEnergyError(Double_t t_ceiledenergy, Double_t t_recoHitEnergy) const;
  virtual Double_t GetPosition1Error(Double_t t_energy) const;
  virtual Double_t GetPosition2Error(Double_t t_energy) const;

 private:
  BSEmcErrorMatrixCalculation(const BSEmcErrorMatrixCalculation &t_other);
  BSEmcErrorMatrixCalculation &operator=(const BSEmcErrorMatrixCalculation & /*unused*/) { return *this; };

  // helper function to convert error matrix
  // copied from BbrGeom/BbrError.cc
  TMatrixD SymmetricMultiplication(const TMatrixD &t_mat, const TMatrixD &t_m1) const;
};

#endif /*BSEMCERRORMATRIXCALCULATION_HH*/
