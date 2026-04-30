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
 * PndFtsLineComparator.cpp
 *
 *  Created on: Jun 2, 2016
 *      Author: kibellus
 */

#include "PndFtsLineComparator.h"

PndFtsLineComparator::~PndFtsLineComparator()
{
  // TODO Auto-generated destructor stub
}

Bool_t PndFtsLineComparator::areEqual(PndLine l1, PndLine l2)
{
  // base vectors
  TVector3 b1 = l1.getP1();
  TVector3 b2 = l2.getP1();

  // direction vectors
  TVector3 d1 = l1.getDir();
  TVector3 d2 = l2.getDir();

  if (getAngle(d1, d2) > fMaxAngle)
    return kFALSE;
  Double_t dist = getDist(b1, b2, d1, d2);
  if (dist > fMaxDistance)
    return kFALSE;
  return kTRUE;
}

Bool_t PndFtsLineComparator::areEqual2D(PndLine l1, PndLine l2)
{
  // base vectors
  TVector3 b1 = l1.getP1();
  TVector3 b2 = l2.getP1();

  // direction vectors
  TVector3 d1 = l1.getDir();
  TVector3 d2 = l2.getDir();
  if (getAngle2D(d1, d2) > fMaxAngle)
    return kFALSE;
  Double_t dist = getDist2D(b1, b2, d1, d2);
  if (dist > fMaxDistance)
    return kFALSE;
  return kTRUE;
}

Double_t PndFtsLineComparator::getQuality3D(PndLine l1, PndLine l2)
{
  // base vectors
  TVector3 b1 = l1.getP1();
  TVector3 b2 = l2.getP1();

  // direction vectors
  TVector3 d1 = l1.getDir();
  TVector3 d2 = l2.getDir();

  Double_t angle3D = getAngle(d1, d2);
  Double_t dist3D = getDist(b1, b2, d1, d2);
  Double_t quality3D = 100 * angle3D + 300 * dist3D;
  return quality3D;
}

Double_t PndFtsLineComparator::getQuality(PndLine l1, PndLine l2)
{
  // base vectors
  TVector3 b1 = l1.getP1();
  TVector3 b2 = l2.getP1();

  // direction vectors
  TVector3 d1 = l1.getDir();
  TVector3 d2 = l2.getDir();
  Double_t angle2D = getAngle2D(d1, d2);
  Double_t dist2D = getDist2D(b1, b2, d1, d2);
  Double_t quality2D = 100 * angle2D + 300 * dist2D;
  Double_t quality3D = getQuality3D(l1, l2);
  return 0.1 * quality3D + 0.9 * quality2D;
}

Double_t PndFtsLineComparator::getAngle(TVector3 d1, TVector3 d2)
{
  Double_t angle = d1.Angle(d2) * TMath::RadToDeg();
  angle = TMath::Min(angle, 180 - angle);
  return angle;
}

Double_t PndFtsLineComparator::getDist(TVector3 b1, TVector3 b2, TVector3 d1, TVector3 d2)
{
  Double_t lambda1 = (zValue - b1[2]) / d1[2];
  Double_t lambda2 = (zValue - b2[2]) / d2[2];
  TVector3 p1 = b1 + lambda1 * d1;
  TVector3 p2 = b2 + lambda2 * d2;
  return (p1 - p2).Mag();
}

Double_t PndFtsLineComparator::getAngle2D(TVector3 d1, TVector3 d2)
{
  d1[1] = 0;
  d2[1] = 0;
  return getAngle(d1, d2);
}

Double_t PndFtsLineComparator::getDist2D(TVector3 b1, TVector3 b2, TVector3 d1, TVector3 d2)
{
  d1[1] = 0;
  d2[1] = 0;
  b1[1] = 0;
  b2[1] = 0;
  return getDist(b1, b2, d1, d2);
}
