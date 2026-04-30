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

/** PndTrkTools
 **
 ** @author Lia Lavezzi
 **/

#ifndef PNDTRKTOOLS_H
#define PNDTRKTOOLS_H 1

#include "TVector3.h"
#include "TVector2.h"

class PndTrkTools : public TObject {

 public:
  PndTrkTools();
  ~PndTrkTools();

  void ComputeLinePointOfTangenceOnACircle(double m, double xc, double yc, double R, double &xi1, double &yi1, double &xi2, double &yi2);
  TVector2 ComputePocaToPointOnCircle2(double x, double y, double xc, double yc, double R);
  TVector3 ComputePocaToPointOnCircle3(double x, double y, double xc, double yc, double R);

  TVector2 ComputePocaToLineOnCircle(double m, double q, double xc, double yc, double R);
  Int_t ComputeSegmentCircleIntersection(TVector2 ex1, TVector2 ex2, double xc, double yc, double R, TVector2 &intersection1, TVector2 &intersection2);
  TVector2 ComputeTangentInPoint(double xc, double yc, TVector2 point);
  void ComputeTangentInPoint(double xc, double yc, TVector2 point, double &m, double &p);

  ClassDef(PndTrkTools, 1);
};

#endif
