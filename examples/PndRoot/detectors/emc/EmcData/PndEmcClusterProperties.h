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

//----------------------------------------------------------------------
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI
//
// Author List:
//	Xiaorong Shi            Lawrence Livermore National Lab
//	Steve Playfer           University of Edinburgh
//	Stephen Gowdy           University of Edinburgh
//
//-----------------------------------------------------------------------
//#pragma once
#ifndef PNDEMCCLUSTERPROPERTIES_H
#define PNDEMCCLUSTERPROPERTIES_H

#include "TObject.h"
#include "PndEmcDataTypes.h"
#include "PndEmcAbsClusterProperty.h"
#include "PndEmcCluster.h"
#include "TMatrixD.h"
#include <vector>

class PndEmcDigi;
class PndEmcTwoCoordIndex;
class TVector3;

class PndEmcClusterProperties : public PndEmcAbsClusterProperty {

 public:
  // Constructors
  PndEmcClusterProperties(const PndEmcCluster &cluster, const TClonesArray *digiArray);

  // Destructor
  virtual ~PndEmcClusterProperties();

  // Total Energy of Cluster
  virtual Double_t Energy() const;

  // Position of the cluster
  TVector3 Where(TString method, std::vector<Double_t> params);
  // Position of the cluster - linear-logarithmic weighting
  TVector3 LiloWhere(std::vector<Double_t> params);
  // Position of the cluster - linear weighting
  TVector3 LinearWhere();
  // Centre of gravity centroid method/
  TVector3 GravWhere();

  // Angle of major axis wrt phi
  virtual Double_t Major_axis() const;

  virtual Double_t Mass() const; // Cluster mass

  ClassDef(PndEmcClusterProperties, 1)
};

#endif // PNDEMCCLUSTERPROPERTIES_H
