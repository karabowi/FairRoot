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

/** PndTrkHit
 ** Class for pattern recognition conformal hit
 ** @author Lia Lavezzi
 **
 **
 **/

#ifndef PNDTRKCONFORMALHIT_H
#define PNDTRKCONFORMALHIT_H 1

#include "TVector2.h"
#include "PndTrkHit.h"

class PndTrkConformalHit : public TObject {

 public:
  PndTrkConformalHit();
  PndTrkConformalHit(PndTrkHit *hit, double u, double v, double rd);
  PndTrkConformalHit(PndTrkHit *hit, double u, double v);
  ~PndTrkConformalHit();

  // inline get
  inline Int_t GetHitID() { return fHit->GetHitID(); }
  inline Int_t GetDetectorID() { return fHit->GetDetectorID(); }

  inline TVector2 GetPosition() { return fPosition; }
  inline Double_t GetU() { return fPosition.X(); }
  inline Double_t GetV() { return fPosition.Y(); }
  inline Double_t GetIsochrone() { return fIsochrone; }
  inline PndTrkHit *GetHit() { return fHit; }

  double GetDistanceFromTrack(double fitm, double fitp);

  void SetPosition(double x, double y) { fPosition.Set(x, y); }

  // info
  void Draw(Color_t color);
  void Print();

  // variables
  PndTrkHit *fHit;
  TVector2 fPosition;
  Double_t fIsochrone;

  ClassDef(PndTrkConformalHit, 1);
};

#endif
