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
//      an xyz reco hit
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//			Ralf Kliemt					 TUD						(application to PndMvd)
//
//-----------------------------------------------------------

//! An xyz reco hit
/** @author Ralf Kliemt TUD
 */

// Base Class Headers ----------------

#ifndef PNDHYPRECOHIT_HH
#define PNDHYPRECOHIT_HH

// Base Class Headers ----------------
#include "GFRecoHitIfc.h"
#include "GFPlanarHitPolicy.h"
#include "PndHypGeoHandling.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
class FairMCPoint;
class PndHypPoint;
class PndHypHit;

class PndHypRecoHit : public GFRecoHitIfc<GFPlanarHitPolicy> {
 public:
  // Constructors/Destructors ---------
  PndHypRecoHit();

  PndHypRecoHit(PndHypPoint *point); // from lab MC points
  PndHypRecoHit(PndHypHit *hit);     // from lab cluster hits TODO change to local
  //   PndHypRecoHit(const TVector3& hit, const TMatrixT<Double_t>& cov,
  //              const DetPlane* detplane);//from lab
  //   PndHypRecoHit(const TVector3& hit, const TMatrixT<Double_t>& cov,
  //              const TVector3& o,const TVector3& u,const TVector3& v);//from lab
  //   PndHypRecoHit(const TMatrixT<Double_t>& xy,const TMatrixT<Double_t>& cov,
  //              const DetPlane* detplane);//from sensor plane

  virtual ~PndHypRecoHit();

  virtual GFAbsRecoHit *clone() { return new PndHypRecoHit(*this); };

  // Operations ----------------------
  virtual TMatrixT<double> getHMatrix(const GFAbsTrackRep *stateVector);

  virtual Double_t residualScalar(GFAbsTrackRep *stateVector, const TMatrixT<Double_t> &state);

 private:
  // Private Data Members ------------
  static const Int_t fNparHitRep = 3;
  PndHypGeoHandling *fGeoH;

  // Private Methods -----------------

 public:
  ClassDef(PndHypRecoHit, 1);
};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
