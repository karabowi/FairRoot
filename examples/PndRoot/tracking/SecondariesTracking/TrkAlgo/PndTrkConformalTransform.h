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

#ifndef PNDTRKCONFORMALTRANSFORM_H
#define PNDTRKCONFORMALTRANSFORM_H

#include "TVector2.h"
#include "PndTrkConformalHit.h"

class PndTrkConformalTransform : public TObject {

 public:
  PndTrkConformalTransform();
  PndTrkConformalTransform(double x, double y, double delta);
  ~PndTrkConformalTransform();

  void SetTranslation(double x, double y);
  void SetRotation(double delta);
  TVector2 GetTranslation() { return fTrasl; }
  Double_t GetRotation() { return fAngle; }

  void PerformConformalTransformation(double x, double y, double rd, double &u, double &v, double &rc);
  void PerformRealTransformation(double u, double v, double rc, double &x, double &y, double &rd);

  PndTrkConformalHit GetConformalSttHit(PndTrkHit *hit);
  PndTrkConformalHit GetConformalHit(PndTrkHit *hit);
  double GetXConf(double x, double y, double rd);
  double GetYConf(double x, double y, double rd);
  double GetRConf(double x, double y, double rd);

  void SetOrigin(double x, double y, double delta);

 private:
  TVector2 fTrasl;
  Double_t fAngle;

  ClassDef(PndTrkConformalTransform, 1)
};

#endif
