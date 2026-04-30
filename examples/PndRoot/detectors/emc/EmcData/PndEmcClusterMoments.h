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
//	Class PndEmcClusterMoments.
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Stephen J. Gowdy	Originator
//	Phil Strother   	Originator
//
// Copyright Information:
//	Copyright (C) 1998	University of Edinburgh
//
// Dima Melnichuk, adaption for pandaroot
//------------------------------------------------------------------------
//#pragma once
#ifndef PNDEMCCLUSTERMOMENTS_H
#define PNDEMCCLUSTERMOMENTS_H

#include "PndEmcAbsClusterProperty.h"
#include "TObject.h"
#include <iostream>

class TVector3;
class PndEmcTwoCoordIndex;
class PndEmcDigi;

class PndEmcClusterMoments : public PndEmcAbsClusterProperty {

 public:
  // Constructors
  PndEmcClusterMoments(const PndEmcCluster &cluster, const TClonesArray *digiArray);
  PndEmcClusterMoments(const PndEmcClusterMoments &);

  // Destructor
  virtual ~PndEmcClusterMoments();

  // Selectors (const)

  // Second moment of energy about centroid
  virtual double SecondMoment() const;
  virtual double SecondMomentTP() const;
  // First moment wght energy and theta
  virtual double Theta1() const;
  // First moment wght energy and phi
  virtual double Phi1() const;
  // Second moment wght energy and theta
  virtual double Theta2() const;
  // Second moment wght energy and phi
  virtual double Phi2() const;
  // First moment wrt major axis
  virtual double Major1() const;
  // Second moment wrt major axis
  virtual double Major2() const;
  // First moment wrt minor axis
  virtual double Minor1() const;
  // Second moment wrt minor axis
  virtual double Minor2() const;
  // First moment of energy about centre
  virtual TVector3 Centre1() const;
  // Second moment of energy about centre
  virtual TVector3 Centre2() const;

  // Printing
  virtual void Print(const Option_t *opt = "") const;

 private:
  PndEmcClusterMoments();
  PndEmcClusterMoments &operator=(const PndEmcClusterMoments &other);

  ClassDef(PndEmcClusterMoments, 1)
};

#endif // PNDEMCCLUSTERMOMENTS_H
