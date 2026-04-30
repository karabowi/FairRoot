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

/*
 * PndMvdRadDamHit.h
 *
 *  Created on: Dec 16, 2008
 *      Author: stockman
 */

#ifndef PNDMVDRADDAMHIT_H_
#define PNDMVDRADDAMHIT_H_

#include "TObject.h"
#include "TVector3.h"
#include "FairHit.h"
class PndMvdRadDamHit : public FairHit {
 public:
  PndMvdRadDamHit();
  PndMvdRadDamHit(Int_t trackID, Int_t hitID, Int_t detId, Int_t pdgCode, Double_t energy, TVector3 pos, TVector3 mom, Double_t radDam);
  virtual ~PndMvdRadDamHit(){};

  void SetRadDam(Double_t radDam) { fRadDam = radDam; };
  Double_t GetRadDam() { return fRadDam; };

 private:
  Int_t fTrackID;
  Int_t fHitID;
  Int_t fDetId;
  Int_t fPdgCode;
  Double_t fEnergy;
  TVector3 fMom;
  Double_t fRadDam;

  ClassDef(PndMvdRadDamHit, 3);
};

#endif /* PNDMVDRADDAMHIT_H_ */
