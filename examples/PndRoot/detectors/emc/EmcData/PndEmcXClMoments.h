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
//	Class PndEmcXClMoments.
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
//#pragma once
#ifndef PNDEMCXCLMOMENTS_H
#define PNDEMCXCLMOMENTS_H

#include "PndEmcAbsClusterProperty.h"
#include "TObject.h"
#include <iostream>
#include <vector>

class TVector3;
class PndEmcDigi;
class PndEmcCluster;

struct PndEmcClEnergyDeposition {
  Double_t deposited_energy;
  Double_t r;
  Double_t phi;
};

class PndEmcXClMoments : public PndEmcAbsClusterProperty {

 public:
  // Constructors

  PndEmcXClMoments(const PndEmcCluster &cluster, const TClonesArray *digiArray);
  PndEmcXClMoments(const PndEmcXClMoments &);

  // Destructor

  virtual ~PndEmcXClMoments();

  // Operators

  // Selectors (const)

  // Second moment of energy about centroid
  virtual Double_t SecondMoment() const;
  virtual Double_t SecondMomentPhi() const;
  virtual Double_t SecondMomentTheta() const;

  // Absolute value Zernike-Moment of index(m,n); according to the
  // definition of the Zernike moments, n and m must satisfy
  // (a) n>=m  (b) n-m even  (c) n,m>=0
  // Otherwise this function returns -1;
  virtual Double_t AbsZernikeMoment(int n, int m, Double_t R0 = 15) const;

  // Parameter for description of the LATeral energydeposition
  // within the cluster; if cluster contains less than 3 digis,
  // the function return 0
  virtual Double_t Lat() const;

  virtual void Print(const Option_t *opt = "") const;

  // Modifiers

 protected:
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

  // Calculation of Zernike-Moments for n<=5 :
  Double_t Fast_AbsZernikeMoment(int n, int m, Double_t R0) const;

  // Calculation of Zernike-Moments for general values of (n,m)
  Double_t Calc_AbsZernikeMoment(int n, int m, Double_t R0) const;

  Double_t Fak(int n) const;

 private:
  // These should never get used
  PndEmcXClMoments();
  PndEmcXClMoments &operator=(const PndEmcXClMoments &);

  // Data members
  std::vector<PndEmcClEnergyDeposition> *fEnergyDistribution;
  Int_t fClusterSize;
  Double_t (PndEmcXClMoments::*fFcn[12])(Double_t) const;
  //	Double_t (PndEmcXClMoments::*fFcn2)( Double_t ); // STE: Is it needed??

  ClassDef(PndEmcXClMoments, 1)
};

#endif // PNDEMCXCLMOMENTS_HH
