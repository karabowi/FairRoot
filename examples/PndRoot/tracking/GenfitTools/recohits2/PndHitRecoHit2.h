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

//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      3d reco hit
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//			Ralf Kliemt					 TUD						(application to PndSds)
//
//-----------------------------------------------------------

//! A planar (x,y) reco hit & its sensor plane
/** @author Ralf Kliemt Uni Bonn
 */
#ifndef PNDHITRECOHIT2_H
#define PNDHITRECOHIT2_H

// Base Class Headers ----------------
#include "SpacepointMeasurement.h"
// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
#include "FairMCPoint.h"
#include "FairHit.h"

class PndHitRecoHit2 : public genfit::SpacepointMeasurement {
 public:
  // Constructors/Destructors ---------
  PndHitRecoHit2();

  PndHitRecoHit2(FairMCPoint *point); // from lab MC points
  PndHitRecoHit2(FairHit *hit);       // from lab cluster hits

  virtual ~PndHitRecoHit2();

  virtual genfit::AbsMeasurement *clone() { return new PndHitRecoHit2(*this); };

 private:
  // Private Data Members ------------
  static const Int_t fNparHitRep = 3;

  // Private Methods -----------------

  // public:
  ClassDef(PndHitRecoHit2, 1);
};

#endif
