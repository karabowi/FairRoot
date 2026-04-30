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

#ifndef PNDSTTRECOHIT2_HH
#define PNDSTTRECOHIT2_HH

// Base Class Headers ----------------

#include "WireMeasurement.h"
#include "TrackCandHit.h"
#include "PndSttHit.h"

#include <TClonesArray.h>

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
class PndSttHit;
class TClonesArray;

class PndSttRecoHit2 : public genfit::WireMeasurement {
 public:
  // Constructors/Destructors ---------
  PndSttRecoHit2();
  //PndSttRecoHit2(PndSttHit *currenthit);
  PndSttRecoHit2(PndSttHit *currenthit, const genfit::TrackCandHit *hit, TClonesArray *tubeArr);

  virtual ~PndSttRecoHit2();

  virtual genfit::AbsMeasurement *clone() { return new PndSttRecoHit2(*this); };

 private:
  // Private Data Members ------------

 protected:
  static const int NparHitRep = 7;

  // Private Methods -----------------

 public:
  ClassDef(PndSttRecoHit2, 1)
};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
