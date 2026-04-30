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
//	Ralf Kliemt	     TUD	    (application to PndMvd)
//      Radoslaw Karabowicz  MVD            (conversion to GEM)
//      Elisabetta Prencipe, 19/5/2014
//-----------------------------------------------------------

//! A planar (x,y) reco hit & its sensor plane
/** @author Radoslaw Karabowicz GSI
//Elisabetta Prencipe, 19/5/2014
 */
#ifndef PNDGEMRECOHIT2_H
#define PNDGEMRECOHIT2_H

// Base Class Headers ----------------
#include "PlanarMeasurement.h"
#include "AbsMeasurement.h"
#include "TrackCandHit.h"
// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
class FairMCPoint;
class PndGemMCPoint;
class PndGemHit;

class PndGemRecoHit2 : public genfit::PlanarMeasurement {
 public:
  // Constructors/Destructors ---------
  PndGemRecoHit2();

  PndGemRecoHit2(PndGemMCPoint *point);                                // from lab MC points
  PndGemRecoHit2(PndGemHit *hit, const genfit::TrackCandHit *candHit); // from lab cluster hits

  virtual ~PndGemRecoHit2();

  virtual genfit::AbsMeasurement *clone() { return new PndGemRecoHit2(*this); };

 private:
  // Private Data Members ------------
  static const Int_t fNparHitRep = 2; // 3;
  //  PndGemGeoHandling* fGeoH;

  // Private Methods -----------------

  // public:
  ClassDef(PndGemRecoHit2, 1);
};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
