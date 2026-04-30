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

#ifndef PNDSTTRECOHIT_HH
#define PNDSTTRECOHIT_HH

// Base Class Headers ----------------
#include "GFRecoHitIfc.h"
#include "GFWireHitPolicy.h"
#include "PndSttHit.h"
#include "TClonesArray.h"

// Collaborating Class Headers -------
#include <ostream> // remove if you do not need streaming op

// Collaborating Class Declarations --
class PndSttHit;
class TClonesArray;

typedef GFRecoHitIfc<GFWireHitPolicy> WireHitRecoHit;

class PndSttRecoHit : public WireHitRecoHit {
 public:
  // Constructors/Destructors ---------
  PndSttRecoHit();
  PndSttRecoHit(PndSttHit *currenthit);
  PndSttRecoHit(PndSttHit *currenthit, TClonesArray *tubeArr);

  virtual ~PndSttRecoHit();

  virtual GFAbsRecoHit *clone() { return new PndSttRecoHit(*this); };

  // Operations ----------------------
  virtual TMatrixT<double> getHMatrix(const GFAbsTrackRep *stateVector);

 private:
  // Private Data Members ------------

 protected:
  static const int NparHitRep = 7;

  // Private Methods -----------------

 public:
  ClassDef(PndSttRecoHit, 1)
};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
