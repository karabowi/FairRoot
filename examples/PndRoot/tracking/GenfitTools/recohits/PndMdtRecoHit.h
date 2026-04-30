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
//      an xyz reco hit (z being considered as a free parameter)
//      hitCoord=(x,y)^T
//      s=z
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//      Stefano Spataro      UNITO          (adapted for MDT - 29/01/10)
//
//
//-----------------------------------------------------------

#ifndef MDTRECOHIT_HH
#define MDTRECOHIT_HH

// Base Class Headers ----------------
#include "GFRecoHitIfc.h"
#include "GFSpacepointHitPolicy.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

#include "TRandom.h"

// Collaborating Class Declarations --
class PndMdtHit;
class PndMdtPoint;

typedef GFRecoHitIfc<GFSpacepointHitPolicy> SpacepointRecoHit;

class PndMdtRecoHit : public SpacepointRecoHit {

 public:
  // Constructors/Destructors ---------
  PndMdtRecoHit();
  PndMdtRecoHit(double x, double y, double z, double sigx, double sigy, double sigz);
  PndMdtRecoHit(const TVector3 &pos, const TVector3 &sig, bool smear = false);
  PndMdtRecoHit(PndMdtPoint *point);
  PndMdtRecoHit(PndMdtHit *hit);

  virtual ~PndMdtRecoHit();

  virtual GFAbsRecoHit *clone();

  // Operations ----------------------
  virtual TMatrixT<double> getHMatrix(const GFAbsTrackRep *stateVector);

  PndMdtHit *GetHit() { return fHit; }

 private:
  // Private Data Members ------------
  static const int NparHitRep = 3;

  PndMdtHit *fHit;
  // Private Methods -----------------

 public:
  ClassDef(PndMdtRecoHit, 1)
};

#endif
