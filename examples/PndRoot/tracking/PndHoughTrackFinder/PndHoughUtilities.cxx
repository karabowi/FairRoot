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

/////////////////////////////////////////////////////////////
//  PndHoughUtilities
//  This class contains different methods to handle PndTracks vs Circles
/////////////////////////////////////////////////////////////////

/** PndHoughUtilities
 *@author Anna Alicke <a.alicke@fz-juelich.de>
 *@since 29.10.2018
 *@updated 02.02.2021
 *@version 3.0
 **
 ** PANDA class to handle PndTracks vs Circles
 ** Task level RECO
 **/

#include "TVector3.h"
#include "TMath.h"
#include "PndTrack.h"
#include "PndTrackCand.h"
#include "PndHoughUtilities.h"

using std::vector;

ClassImp(PndHoughUtilities)

  /**
   * @brief      Finds the correct intersection point.
   *             To find a special point on a circle a construction with a line and a circle or two circles is done. In both cases two intersection points exist. In this method the
   * intersection point is selected that is closer to the hit point.
   *
   * @param      intersectionPoints  The two intersection points stored as [x1, y1, x2, y2]
   * @param      point               The hit point
   *
   * @return     The correct intersection point
   */
  TVector2 PndHoughUtilities::findCorrectIntersectionPoint(std::vector<double> &intersectionPoints, TVector2 &point)
{
  double d1 = sqrt(((point.X() - intersectionPoints[0]) * (point.X() - intersectionPoints[0])) + ((point.Y() - intersectionPoints[1]) * (point.Y() - intersectionPoints[1])));
  double d2 = sqrt(((point.X() - intersectionPoints[2]) * (point.X() - intersectionPoints[2])) + ((point.Y() - intersectionPoints[3]) * (point.Y() - intersectionPoints[3])));
  TVector2 intersectionPoint(intersectionPoints[0], intersectionPoints[1]);
  if (d2 < d1) {
    intersectionPoint.Set(intersectionPoints[2], intersectionPoints[3]);
  }
  return intersectionPoint;
}

/**
 * @brief      Finds the intersection point between a line and a circle.
 *
 * @param      circle  The circle parameters (x, y, r)
 * @param[in]  m       The line's slope
 * @param[in]  b       The line's y-intercept
 * @param[in]  Ax      0 for a true line and x if m == 0 && b == 0
 *
 * @return     The two intersection points of a circle and a line stored as [x1, y1, x2, y2].
 */
std::vector<double> PndHoughUtilities::calcIntersectionPointCircleLine(std::vector<double> &circle, double m, double b, double Ax)
{
  double x0, y0, r, p, q, intersectionPointX1, intersectionPointY1, intersectionPointX2, intersectionPointY2;
  if (m == 0 && b == 0) {
    x0 = circle[0];
    y0 = circle[1];
    r = circle[2];
    intersectionPointX1 = Ax;
    intersectionPointX2 = Ax;
    intersectionPointY1 = y0 + sqrt(r * r - (Ax - x0) * (Ax - x0));
    intersectionPointY2 = y0 - sqrt(r * r - (Ax - x0) * (Ax - x0));
  } else {
    x0 = circle[0];
    y0 = circle[1];
    r = circle[2];
    p = (2 * m * (b - y0) - 2 * x0) / (m * m + 1);
    q = (x0 * x0 + (b - y0) * (b - y0) - r * r) / (m * m + 1);

    intersectionPointX1 = -p / 2 + sqrt(p * p / 4 - q);
    intersectionPointY1 = m * intersectionPointX1 + b;

    intersectionPointX2 = -p / 2 - sqrt(p * p / 4 - q);
    intersectionPointY2 = m * intersectionPointX2 + b;
  }
  vector<double> intersectionPoints;
  intersectionPoints.push_back(intersectionPointX1);
  intersectionPoints.push_back(intersectionPointY1);
  intersectionPoints.push_back(intersectionPointX2);
  intersectionPoints.push_back(intersectionPointY2);
  return intersectionPoints;
}

/**
 * @brief      Finds the point on a circle closest to the given hit point
 *
 * @param[in]  i       The index of the hit point
 * @param[in]  myCand  The track candidate the hit is chosen from
 * @param[in]  circle  The circle.
 *
 * @return     The point on the circle.
 */
TVector2 PndHoughUtilities::calcPointOnCircle(Int_t i, PndTrackCand &myCand, std::vector<double> &circle)
{
  FairLink hit = myCand.GetSortedHit(i);
  FairHit *hitPos = fMapFairLinktoFairHit[hit];
  double m = (hitPos->GetY() - circle[1]) / (hitPos->GetX() - circle[0]);
  double b = hitPos->GetY() - (hitPos->GetX() * m);
  std::vector<double> IntersectionPoints;
  if (m == 0 && b == 0) {
    double x = hitPos->GetX();
    IntersectionPoints = calcIntersectionPointCircleLine(circle, m, b, x);
  } else {
    IntersectionPoints = calcIntersectionPointCircleLine(circle, m, b, 0);
  }
  TVector2 hits(hitPos->GetX(), hitPos->GetY());
  TVector2 InteractionPoint = findCorrectIntersectionPoint(IntersectionPoints, hits);
  return InteractionPoint;
}

/**
 * @brief      Determines the transverse momentum.
 *
 * @param[in]  B     The z component of the magnetic field.
 * @param[in]  r     The radius of the circle approximating the particle track.
 *
 * @return     The transverse momentum.
 */
double PndHoughUtilities::Pt(double B, double r)
{
  double result = 0.3 * B * abs(r) * 0.01;
  return result;
}

/**
 * @brief      Determines the charge q and the momentum p of a hit
 *
 * @param[in]  i       The index of the hit
 * @param[in]  B       The z component of the magnetic field.
 * @param[in]  circle  The circle approximating the particle track.
 * @param[in]  myCand  The track candidate.
 *
 * @return     A tuple containing the momentum p and the charge q
 */
std::tuple<TVector3, Int_t> PndHoughUtilities::getPandqforHit(int i, double B, TVector3 &circle, PndTrackCand &myCand)
{
  TVector3 ptVec3;
  double pt = Pt(B, circle.Z());
  double pl = 0;
  FairLink hit = myCand.GetSortedHit(i);
  double x = circle.X();
  double y = circle.Y();
  double z = circle.Z();

  std::vector<double> circleTemp;
  circleTemp.push_back(x);
  circleTemp.push_back(y);
  circleTemp.push_back(z);

  TVector2 hitPos = calcPointOnCircle(i, myCand, circleTemp);

  double dx = hitPos.X() - circle.X();
  double dy = hitPos.Y() - circle.Y();

  Int_t q;

  TVector2 vCenterPoint;
  TVector2 vTrackDirection;

  vCenterPoint.Set(dx, dy);
  if (i > 0) {
    FairLink hitBefore = myCand.GetSortedHit(i - 1);
    FairHit *hitPosBefore = fMapFairLinktoFairHit[hitBefore];
    vTrackDirection.Set(hitPos.X() - hitPosBefore->GetX(), hitPos.Y() - hitPosBefore->GetY());
  } else {
    FairLink hitAfter = myCand.GetSortedHit(1);
    FairHit *hitPosAfter = fMapFairLinktoFairHit[hitAfter];
    vTrackDirection.Set(hitPosAfter->GetX() - hitPos.X(), hitPosAfter->GetY() - hitPos.Y());
  }
  TVector2 vTrackDirectionRot = vTrackDirection.Rotate(TMath::Pi() / 2);
  double res = vCenterPoint * vTrackDirectionRot;
  if (res > 0)
    q = 1;
  else
    q = -1;

  Double_t phi = TMath::ATan2(dy, dx);

  if (q == 1)
    phi = phi + TMath::Pi();
  TVector2 ptVec(0, pt);
  ptVec = ptVec.Rotate(phi);

  ptVec3.SetXYZ(ptVec.X(), ptVec.Y(), pl);

  return std::make_tuple(ptVec3, q);
}

/**
 * @brief      Helper function to find PndTrack parameters from a circle
 *
 * @param[in]  i       The index of the hit
 * @param[in]  B       The z component of the magnetic field.
 * @param[in]  myCand  The track candidate.
 * @param[in]  circle  The circle approximating the particle track.
 *
 * @return     The track parameters at the position of a hit.
 */
FairTrackParP PndHoughUtilities::getTrackParPForHit(Int_t i, Double_t B, PndTrackCand &myCand, TVector3 &circle)
{

  TVector3 hitPos;
  TVector3 hitPosError(0, 0, 0);
  TVector3 momError(2, 2, 2);
  TVector3 dj(1, 0, 0);
  TVector3 dk(0, 1, 0);
  TVector3 origin(0, 0, 1);
  FairLink hit = myCand.GetSortedHit(i);
  double x = circle.X();
  double y = circle.Y();
  double z = circle.Z();
  std::vector<double> circleTemp;
  circleTemp.push_back(x);
  circleTemp.push_back(y);
  circleTemp.push_back(z);
  TVector2 hitPosTemp = calcPointOnCircle(i, myCand, circleTemp);
  hitPos.SetXYZ(hitPosTemp.X(), hitPosTemp.Y(), 0);
  TVector3 p;
  Int_t q;
  std::tie(p, q) = getPandqforHit(i, B, circle, myCand);
  FairTrackParP result(hitPos, p, hitPosError, momError, q, origin, dj, dk);

  return result;
}

/**
 * @brief      Determined the PndTrack for a given track candidate and a circle.
 *
 * @param[in]  B       The z component of the magnetic field.
 * @param[in]  myCand  The track candidate.
 * @param[in]  circle  The circle approximating the particle track.
 *
 * @return     The PndTrack.
 */
PndTrack PndHoughUtilities::getPndTrack(Double_t B, PndTrackCand &myCand, TVector3 &circle)
{
  FairTrackParP first, last;
  first = getTrackParPForHit(0, B, myCand, circle);
  last = getTrackParPForHit(myCand.GetNHits() - 1, B, myCand, circle);

  return PndTrack(first, last, myCand);
}

/**
 * @brief      Determines the circle from a PndTrack.
 *
 * @param      tr    The PndTrack.
 *
 * @return     The circle corresponding to the PndTrack.
 */
std::vector<double> PndHoughUtilities::getCircleFromPndTrack(PndTrack &tr)
{
  double x1 = tr.GetParamFirst().GetX();
  double y1 = tr.GetParamFirst().GetY();
  double x2 = tr.GetParamLast().GetX();
  double y2 = tr.GetParamLast().GetY();

  double px1 = tr.GetParamFirst().GetPx();
  double py1 = tr.GetParamFirst().GetPy();
  double pz1 = tr.GetParamFirst().GetPz();
  double px2 = tr.GetParamLast().GetPx();
  double py2 = tr.GetParamLast().GetPy();
  double pz2 = tr.GetParamLast().GetPz();

  std::vector<double> real_circle(3, 0);

  double slopeP1 = py1 / px1;
  double slopeP2 = py2 / px2;

  double CircleCenterY = 1 / (slopeP2 - slopeP1) * (slopeP2 * y2 + x2 - slopeP1 * y1 - x1);
  double CircleCenterX = (slopeP2 * y2 + x2 - slopeP2 * CircleCenterY);

  real_circle[0] = CircleCenterX;
  real_circle[1] = CircleCenterY;
  real_circle[2] = sqrt(pow(real_circle[0] - x1, 2) + pow(real_circle[1] - y1, 2));

  return real_circle;
}

/**
 * @brief      Calculates the sum of distances from each hit of a track to the track circle
 *
 * @param      trackCand  The track candidate.
 * @param      circle     The circle.
 *
 * @return     The sum of distances from each hit of a track to the track circle
 */
double PndHoughUtilities::calcDistanceTrackCandToTrack(PndTrackCand &trackCand, std::vector<double> &circle)
{
  double dTot = 0;
  for (int i = 0; i < trackCand.GetNHits(); i++) {
    FairLink link = trackCand.GetSortedHit(i);
    FairHit *myHit = fMapFairLinktoFairHit[link];
    double rIso = fMapFairLinktoIsochrone[link];
    double isoX = myHit->GetX();
    double isoY = myHit->GetY();
    double isoZ = rIso;
    double d = calcDistanceFromIsochroneToCircle(isoX, isoY, isoZ, circle[0], circle[1], circle[2]);
    dTot += d;
  }
  return dTot;
}

/**
 * @brief      Calculates the distance from a  hitpoint (MVD hit or nearest point of stt isochrone to track) to a circle from hough space.
 *              - MVD:
 *                - R=Distance Track center to mvd point --> abs(r_A-R)=distance
 *              - STT:
 *                - D=Distance Track center to stt center --> R = min(D+r_stt, D-r_stt) --> abs(r_A-R)=distance
 *                - D=Distance Track center to stt center --> abs(r_A-D)=distance
 *
 * @param      xi    x component of the hit point (For MVD the Hit point, for STT the center of the straw tube)
 * @param      yi    y component of the hit point (For MVD the Hit point, for STT the center of the straw tube)
 * @param      ri    isochrone radius of the STT hit (For MVD 0, for STT the isochrone radius.)
 * @param      xT    The x component of the circle center corresponding to a particle track.
 * @param      yT    The y component of the circle center corresponding to a particle track.
 * @param      rT    The radiusof the circle corresponding to a particle track.
 *
 * @return     The distance from a hit (with or without isochrone radius ) to the circle.
 */
double PndHoughUtilities::calcDistanceFromIsochroneToCircle(double &xi, double &yi, double &ri, double &xT, double &yT, double &rT)
{
  double D = sqrt((xi - xT) * (xi - xT) + (yi - yT) * (yi - yT));
  double dCircleHit = D - abs(rT);
  double dcircleisochrone = abs(std::min(abs(dCircleHit) + ri, abs(dCircleHit) - ri));

  return dcircleisochrone;
}
