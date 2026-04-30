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

// -------------------------------------------------------------------------
// -----                     PndRichHit header file                    -----
// -----               Created 11/10/06 by Annalisa Cecchi             -----
// -----                                                               -----
// -------------------------------------------------------------------------

#ifndef PNDRICHHIT_H
#define PNDRICHHIT_H

#include "FairHit.h"

class PndRichHit : public FairHit {

 public:
  /** Default constructor **/
  PndRichHit();

  PndRichHit(Int_t detID, Int_t sensorId, TVector3 &pos, TVector3 &dpos, Double_t thetaC, Double_t errThetaC, Int_t index);

  /** Copy constructor **/
  // PndRichHit(const PndRichHit& hit) { *this = hit; }; // not needed

  /** Destructor **/
  virtual ~PndRichHit();

  /** Output to screen **/
  virtual void Print(const Option_t *opt = "") const;

  /** Modifiers **/

  /** Accessors **/
  virtual Double_t GetThetaC() { return fThetaC; }
  virtual Double_t GetErrThetaC() { return fErrThetaC; }
  virtual Int_t GetRefIndex() { return fRefIndex; }
  Int_t GetSensorId() { return fSensorId; }

  TVector3 GetPosition() const { return TVector3(fX, fY, fZ); }

 protected:
  Int_t fSensorId;
  Double_t fThetaC, fErrThetaC;

  ClassDef(PndRichHit, 1)
};

#endif // PNDRICHHIT_H
