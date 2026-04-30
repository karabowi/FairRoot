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

// modified by Elisabetta Prencipe 19/5/2014
#ifndef PNDFTSRECOHIT2_HH
#define PNDFTSRECOHIT2_HH

// Base Class Headers ----------------
///#include "GFRecoHitIfc.h"
#include "WireMeasurement.h"
#include "WireTrackCandHit.h"
#include "PndFtsHit.h"
#include "TClonesArray.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
class PndFtsHit;
class TClonesArray;

class PndFtsRecoHit2 : public genfit::WireMeasurement {
 public:
  // Constructors/Destructors ---------
  PndFtsRecoHit2();
  PndFtsRecoHit2(PndFtsHit *currenthit);
  PndFtsRecoHit2(PndFtsHit *currenthit, const genfit::TrackCandHit *hit, TClonesArray *tubeArr);

  virtual ~PndFtsRecoHit2();

  virtual genfit::AbsMeasurement *clone() { return new PndFtsRecoHit2(*this); };

 private:
  // Private Data Members ------------

 protected:
  static const int NparHitRep = 7;

  // Private Methods -----------------

 public:
  ClassDef(PndFtsRecoHit2, 1)
};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
