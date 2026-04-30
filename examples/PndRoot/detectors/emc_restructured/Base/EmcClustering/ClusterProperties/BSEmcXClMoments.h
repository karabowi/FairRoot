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
// File and Version Information:
//
// Description:
//	Class BSEmcXClMoments.
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Thorsten Brandt    	Originator
//
// Copyright Information:
//
// Dima Melnichuk, adaption for pandaroot
//
//------------------------------------------------------------------------
#ifndef BSEMCXCLMOMENTS_HH
#define BSEMCXCLMOMENTS_HH

#include <iostream>
#include <memory>
#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"

#include "BSEmcCrystalPositionPar.h"
#include "BSEmcPositionAlgo.h"

class BSEmcDigi;
class BSEmcCluster;
class BSEmcCrystalPositionPar;
class TBuffer;
class TClass;
class TMemberInspector;

struct Moments {
  Double_t Lateral;
  Double_t Z20;
  Double_t Z53;
};

/**
 * @class BSEmcXClMoments
 * @brief Helper class to calculate Zernike and Lateral Moments
 * @details
 * @author Thorsten Brandt    	Originator
 * @author Dima Melnichuk, adaption for pandaroot
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcXClMoments {

 public:
  // Constructors
  BSEmcXClMoments();
  // Destructor
  virtual ~BSEmcXClMoments();

  void SetPositionPar(BSEmcCrystalPositionPar *t_positionPar)
  {
    fPositionPar = t_positionPar;
    fPositionAlgo->SetPositionPar(t_positionPar);
  }
  Moments CalculateMoments(const BSEmcCluster *t_cluster, const std::vector<const BSEmcDigi *> &t_digiArray);

 protected:
  struct BSEmcClEnergyDeposition {
    Double_t deposited_energy;
    Double_t r;
    Double_t phi;
  };

  // Helper functions

  void Init();

  // explicit implementation of polynomial part of
  // Zernike-Functions for n<=5;

  Double_t f00(Double_t r) const;
  Double_t f11(Double_t r) const;
  Double_t f20(Double_t r) const;
  Double_t f22(Double_t r) const;
  Double_t f31(Double_t r) const;
  Double_t f33(Double_t r) const;
  Double_t f40(Double_t r) const;
  Double_t f42(Double_t r) const;
  Double_t f44(Double_t r) const;
  Double_t f51(Double_t r) const;
  Double_t f53(Double_t r) const;
  Double_t f55(Double_t r) const;

  std::vector<BSEmcClEnergyDeposition> CreateEnergyDeposition(const BSEmcCluster *t_cluster, const std::vector<const BSEmcDigi *> &t_digiArray);

  // Calculation of Zernike-Moments for n<=5 :
  Double_t Fast_AbsZernikeMoment(Int_t t_n, Int_t t_m, Double_t t_r0, const std::vector<BSEmcClEnergyDeposition> &t_energydeposition) const;

  // Calculation of Zernike-Moments for general values of (n,m)
  Double_t Calc_AbsZernikeMoment(Int_t t_n, Int_t t_m, Double_t t_r0, const std::vector<BSEmcClEnergyDeposition> &t_energydeposition) const;

  Double_t Fak(Int_t t_n) const;

  // Absolute value Zernike-Moment of index(m,n); according to the
  // definition of the Zernike moments, n and m must satisfy
  // (a) n>=m  (b) n-m even  (c) n,m>=0
  // Otherwise this function returns -1;
  virtual Double_t AbsZernikeMoment(Int_t t_n, Int_t t_m, Double_t t_r0, const std::vector<BSEmcClEnergyDeposition> &t_energydeposition) const;

  // Parameter for description of the LATeral energydeposition
  // within the cluster; if cluster contains less than 3 digis,
  // the function return 0
  virtual Double_t Lat(const std::vector<BSEmcClEnergyDeposition> &t_energydeposition) const;

 private:
  // Data members
  const BSEmcCluster *fCurrentCluster{nullptr};
  std::unique_ptr<BSEmcPositionAlgo> fPositionAlgo{new BSEmcPositionAlgo};
  BSEmcCrystalPositionPar *fPositionPar{nullptr};
  Int_t fClusterSize{-1};

  Double_t (BSEmcXClMoments::*fFcn[12])(Double_t) const;

  ClassDef(BSEmcXClMoments, 1)
};

#endif /*BSEMCXCLMOMENTS_HH*/
