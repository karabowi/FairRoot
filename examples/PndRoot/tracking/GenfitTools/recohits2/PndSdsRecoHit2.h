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
//      a planar (x,y) reco hit & its sensor plane
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//			Ralf Kliemt					 TUD						(application to PndSds)
//      modified by Elisabetta Prencipe 19/5/2014
//-----------------------------------------------------------

//! A planar (x,y) reco hit & its sensor plane
/** @author Ralf Kliemt TUD
 */
#ifndef PNDSDSRECOHIT2_H
#define PNDSDSRECOHIT2_H

// Base Class Headers ----------------

#include "PlanarMeasurement.h"
#include "PndGeoHandling.h"
#include "TrackCandHit.h"
#include "AbsMeasurement.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
class FairMCPoint;
class PndSdsMCPoint;
class PndSdsHit;

class PndSdsRecoHit2 : public genfit::PlanarMeasurement {
 public:
  // Constructors/Destructors ---------
  PndSdsRecoHit2();

  PndSdsRecoHit2(PndSdsMCPoint *point);                                // from lab MC points
  PndSdsRecoHit2(PndSdsHit *hit, const genfit::TrackCandHit *candHit); // from lab cluster hits

  virtual ~PndSdsRecoHit2();

  virtual genfit::AbsMeasurement *clone() { return new PndSdsRecoHit2(*this); };

 private:
  // Private Data Members ------------
  static const Int_t fNparHitRep = 2; // 3;
  PndGeoHandling *fGeoH;              //!

  // Private Methods -----------------

  // public:
  ClassDef(PndSdsRecoHit2, 1);
};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
