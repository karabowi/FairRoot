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
//	Class PndEmcClusterDistances.
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Phil Strother   	Originator
//
// Copyright Information:
//	Copyright (C) 1998	University of Edinburgh
//
// Dima Melnichuk, adaption for pandaroot
//------------------------------------------------------------------------
//#pragma once
#ifndef PNDEMCCLUSTERDISTANCES_H
#define PNDEMCCLUSTERDISTANCES_H

#include "PndEmcAbsClusterProperty.h"
#include "TObject.h"

class TVector3;
class PndEmcXtal;
class PndEmcDigi;

class PndEmcClusterDistances : public PndEmcAbsClusterProperty {

 public:
  // Constructors
  PndEmcClusterDistances(const PndEmcCluster &cluster, const TClonesArray *digiArray);
  PndEmcClusterDistances(const PndEmcClusterDistances &);

  // Destructor
  virtual ~PndEmcClusterDistances();

  Double_t RadialDistance(const PndEmcDigi *const) const;
  // This is the radial distance (in cm) of the digi from the cluster
  // centroid in a plane containing the estimated shower max (assumed EM)
  // whose normal points back to the IP.

  Double_t AngularDistance(const PndEmcDigi *const) const;
  // Just sqrt( dTheta**2 + dPhi**2).  In mr.

  Double_t AngularSeparation(const PndEmcDigi *const) const;

  // The angle between the cluster centroid and the digi in question, in mr.

 private:
  PndEmcClusterDistances &operator=(const PndEmcClusterDistances &) { return *this; };

  // Data members
  TVector3 *fShowerMaxLocation;
  TVector3 *fClusterLocation;
  PndEmcXtal *fTheGeom;
  TVector3 *fTheNormalToFrontFace;
  Double_t fDistanceOfPlane;
  Double_t fClusPhi;
  Double_t fClusTheta;

  ClassDef(PndEmcClusterDistances, 1)
};

#endif // PNDEMCCLUSTERDISTANCES_HH
