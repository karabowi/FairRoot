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

//-----------------------------------------------------------------------
// File and Version Information:
// 	$Id: $
//
// Description:
//	PndEmcXtal Class -
//
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI
//
// Author List:
//	Gautier Hamel de Monchenault - CEN Saclay & Lawrence Berkeley Lab
//      Stephen J. Gowdy              University of Edinburgh
// Dima Melnichuk - adaption for PANDA
//
// Copyright Information:
//	Copyright (C) 1996		Lawrence Berkeley Laboratory
//	Copyright (C) 1996	       CEA - Centre d'Etude de Saclay
//------------------------------------------------------------------------
//#pragma once
#ifndef PNDEMCXTAL_H
#define PNDEMCXTAL_H

//#include <vector>
#include "TVector3.h"
#include "TGeoArb8.h"
#include "TGeoMatrix.h"

class PndEmcTwoCoordIndex;

/**
 * @brief represents coordinates of one crystal
 * @ingroup PndEmc
 */
class PndEmcXtal : public TObject {

 public:
  // Constructors
  PndEmcXtal();
  PndEmcXtal(const PndEmcTwoCoordIndex *id, const TGeoTrap &trap, const TVector3 &pos, const TGeoRotation &rot);

  // Destructor
  virtual ~PndEmcXtal();

  // Operators
  virtual bool operator==(const PndEmcXtal &compare) const;
  virtual bool operator<(const PndEmcXtal &compare) const;
  // Accessors (const)
  const PndEmcTwoCoordIndex *myIndex() const;
  const TVector3 &centre() const;
  const TVector3 &frontCentre() const;
  const TVector3 &normalToFrontFace() const;
  const TVector3 &axisVector() const;
  const TGeoTrap &geometry() const;
  const TGeoRotation &rotation() const;

  double npAngle() const;

 private:
  const PndEmcTwoCoordIndex *fTCIIndex; //!
  double fLength;                       // length of the crystal
  const TGeoTrap fTrap;
  const TGeoRotation fRotation;
  TVector3 fCentre;
  TVector3 fFrontCentre;
  TVector3 fNormalToFrontFace;
  TVector3 fAxis;

  PndEmcXtal(const PndEmcXtal &L);
  PndEmcXtal &operator=(const PndEmcXtal &) { return *this; }

  ClassDef(PndEmcXtal, 1)
};

#endif // PNDEMCXTAL_HH
