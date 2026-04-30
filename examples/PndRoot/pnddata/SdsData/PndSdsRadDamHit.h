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
 * PndSdsRadDamHit.h
 *
 *  Created on: Dec 16, 2008
 *      Author: stockman
 */

#ifndef PNDSDSRADDAMHIT_H_
#define PNDSDSRADDAMHIT_H_

#include <TObject.h>
#include <TString.h>
#include <TVector3.h>

class PndSdsRadDamHit : public TObject {
 public:
  PndSdsRadDamHit();
  PndSdsRadDamHit(Int_t trackID, Int_t hitID, TString detName, Int_t pdgCode, Double_t energy, TVector3 pos, TVector3 mom, Double_t radDam);
  virtual ~PndSdsRadDamHit(){};

  void SetRadDam(Double_t radDam) { fRadDam = radDam; };
  Double_t GetRadDam() { return fRadDam; };

 private:
  Int_t fTrackID;
  Int_t fHitID;
  TString fDetName;
  Int_t fPdgCode;
  Double_t fEnergy;
  TVector3 fPos;
  TVector3 fMom;
  Double_t fRadDam;

  ClassDef(PndSdsRadDamHit, 2);
};

#endif /* PNDSDSRADDAMHIT_H_ */
