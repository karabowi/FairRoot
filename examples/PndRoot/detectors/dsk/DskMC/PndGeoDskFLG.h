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
// -----                     PndGeoDskFLG header file                     -----
// -----               Created 29/04/10  by Y. Liang                  -----
// -------------------------------------------------------------------------

#ifndef PNDGEODSKFLG_H
#define PNDGEODSKFLG_H

#include "FairGeoSet.h"
#include "TVector3.h"

class PndGeoDskFLG : public FairGeoSet {

 private:
  double fRadius;              //!< disc radius [cm]
  Double_t fPosition_plate;    // position of plate, fix me.  need to retrieve from geo in future
  Double_t fThickness_plate;   // mm, fix me.  need to retrieve from geo in future
  Int_t fNumber_focusing_part; // 128 focusing parts, fix me.
  Double_t fReflectThreshold;

  Double_t a0; // curve function
  Double_t a1;
  Double_t a2;
  Double_t a3;
  Double_t a4;
  Double_t a5;

  int fVerbose;

 protected:
 public:
  PndGeoDskFLG();
  ~PndGeoDskFLG() {}

  void Propagate(TVector3 pos, TVector3 dir, Int_t &i_FLG, Int_t &i_Pixel);
  void LineCylinderInteraction(TVector3 pos, TVector3 dir, TVector3 &pos_interaction);
  Bool_t CurveFunction(TVector3 Point);
  Double_t SlopeCurveFunction(TVector3 Point);
  Bool_t LineFunction(TVector3 Point);

  Double_t radius() { return fRadius; }
  Double_t thickness() { return fThickness_plate; }
  Double_t postion_plate() { return fPosition_plate; }
  Double_t reflect_threshold() { return fReflectThreshold; }

  ClassDef(PndGeoDskFLG, 0) // Class for Dsk
};

#endif
