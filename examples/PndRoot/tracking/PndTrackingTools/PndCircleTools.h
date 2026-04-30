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

#pragma once

#include "FairHit.h"
#include "TVector2.h"

namespace PANDA {
namespace CircleTools {

/**
 * @brief      Calculates the transverse momentum from a given radius
 *
 * @param[in]  B      The magnetic field in [T]
 * @param[in]  r      The radius of the circle in [cm]
 * @param[out]        The transverse momentum in [GeV/c]
 */
double PtFromCircle(double B, double r)
{
  // 0.01 to recalculate [cm] in [m]
  // 0.3 from constants when setting Lorentz force equal to centrifugal force
  // pT [GeV/c] = 0.3 * B [T] * abs(r [cm]) * 0.01;
  return 0.003 * B * abs(r);
}

/**
 * @brief      Calculates the angle phi for a hit depending on the circle center
 *
 * @param[in]  hit      A hit
 * @param[in]  circle   The parameters of the circle given in a TVector3 as circle Center and radius (X, Y, R)
 * @param[out]          Phi in [rad]
 */
double PhiForHit(FairHit *hit, TVector3 &circle)
{
  TVector2 hitV(hit->GetX(), hit->GetY());
  TVector2 circleCenter(circle.X(), circle.Y());
  return (hitV - circleCenter).Phi();
}

/**
 * @brief      Determines the rotation direction of the particle
 *             If the particle rotates anticlockwise it returns -1, alse it returns +1
 *
 * @param[in]  firstHit    A first hit of the track
 * @param[in]  nextHit     A hit of the track occuring after the first hit
 * @param[in]  circle      The track parameters as circle approximation (X, Y, R)
 * @param[out]             If the particle rotates anticlockwise it returns -1, alse it returns +1

 */
int RotationDirection(FairHit *firstHit, FairHit *nextHit, TVector3 &circle)
{
  double phiFirst = PhiForHit(firstHit, circle);
  double phiNext = PhiForHit(nextHit, circle);

  if (phiNext - phiFirst > 0)
    return -1;
  else
    return 1;
}

/**
 * @brief      Determins the position on the circle for a given hit
 *
 * @param[in]  hit      A hit of the track
 * @param[in]  circle   The circle parameters that describe the track
 * @param[out] result   The position on the circle for a given hit
 */
TVector2 PositionOnCircle(FairHit *hit, TVector3 &circle)
{

  double phiHit = PhiForHit(hit, circle);

  TVector2 circleCenter(circle.X(), circle.Y());
  TVector2 result(circle.Z(), 0);
  result = result.Rotate(phiHit);
  result += circleCenter;
  return result;
}

/**
 * @brief      Rotates the transverse momentum by phi to determine the track's momentum direction at a hit point
 *
 * @param[in]  hit        A hit of the track
 * @param[in]  circle     The circle parameters that describe the track
 * @param[in]  direction  The moving direction of the particle (similar to charge)
 * @param[in]  B          The magnetic field
 * @param[out] result     Momentum direction at the given hit point
 */
TVector2 PtOnCircle(FairHit *hit, TVector3 &circle, int direction, double B)
{
  TVector2 result(0, PtFromCircle(B, circle.Z()));
  double phiOnCircle = PhiForHit(hit, circle);
  if (direction > 0) {
    phiOnCircle += TMath::Pi();
  }
  return result.Rotate(phiOnCircle);
}

} // namespace CircleTools
} // namespace PANDA
