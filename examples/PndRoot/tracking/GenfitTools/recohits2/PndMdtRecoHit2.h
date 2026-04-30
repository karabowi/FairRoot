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
//      modified by Elisabetta Prencipe 19/05/2014
//
//-----------------------------------------------------------

#ifndef MDTRECOHIT2_HH
#define MDTRECOHIT2_HH

// Base Class Headers ----------------
#include "SpacepointMeasurement.h"
#include "TrackCandHit.h"
// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

#include "TRandom.h"

// Collaborating Class Declarations --
class PndMdtHit;
class PndMdtPoint;

class PndMdtRecoHit2 : public genfit::SpacepointMeasurement {

 public:
  // Constructors/Destructors ---------
  PndMdtRecoHit2();
  PndMdtRecoHit2(double x, double y, double z, double sigx, double sigy, double sigz);
  PndMdtRecoHit2(const TVector3 &pos, const TVector3 &sig, bool smear = false);
  PndMdtRecoHit2(PndMdtPoint *point);
  PndMdtRecoHit2(PndMdtHit *hit, const genfit::TrackCandHit *candHit);

  virtual ~PndMdtRecoHit2();

  virtual genfit::AbsMeasurement *clone() { return new PndMdtRecoHit2(*this); }

  PndMdtHit *GetHit() { return fHit; }

 private:
  // Private Data Members ------------
  static const int NparHitRep = 3;

  PndMdtHit *fHit;
  // Private Methods -----------------

 public:
  ClassDef(PndMdtRecoHit2, 1)
};

#endif
