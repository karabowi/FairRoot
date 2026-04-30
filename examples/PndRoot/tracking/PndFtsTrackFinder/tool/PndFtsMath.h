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

//
// Created by Bartosz Sobol on 12.07.2020.
//

#pragma once

#include <cmath>

#include "PndFtsStraw.h"
#include "PndFtsHelpers.h"

namespace PndFtsTrackFinder::PndFtsMath {

inline float LineValue(const Line &line, const float arg) noexcept
{
  return arg * line.fSlope + line.fIntercept;
}

inline float CircleValue(const Circle &circle, const float arg) noexcept
{
  //  const float val = std::sqrt(circle.fRadius * circle.fRadius - arg * arg);
  const float val = std::sqrt(circle.fRadius * circle.fRadius - (arg - circle.fOrigin.fZ) * (arg - circle.fOrigin.fZ));
  return circle.fOrigin.fX > 0 ? circle.fOrigin.fX - val : circle.fOrigin.fX + val;
}

inline Line PointsToLine(float abscissa1, float ordinate1, float abscissa2, float ordinate2) noexcept
{
  // INFO ordinate = slope * abscissa + intercept // no need to handle zero denominator case
  const float slope = (ordinate1 - ordinate2) / (abscissa1 - abscissa2);
  const float intercept = ordinate1 - abscissa1 * slope;
  return {slope, intercept};
}

inline Line PointsToLine(const Point2D &point1, const Point2D &point2) noexcept
{
  // INFO ordinate = slope * abscissa + intercept // no need to handle zero denominator case
  return PointsToLine(point1.fAbscissa, point1.fOrdinate, point2.fAbscissa, point2.fOrdinate);
}

inline Line XZStrawsToLine(const PndFtsStraw &straw1, const PndFtsStraw &straw2) noexcept
{
  // INFO x = slope * z + intercept // no need to handle zero denominator case
  return PointsToLine(straw1.fZ, straw1.fX, straw2.fZ, straw2.fX);
}

inline Line XZPointsToLine(const XZPoint &point1, const XZPoint &point2) noexcept
{
  // INFO x = slope * z + intercept // no need to handle zero denominator case
  return PointsToLine(point1.fZ, point1.fX, point2.fZ, point2.fX);
}

inline Line YZPointsToLine(const YZVirtualHit &point1, const YZVirtualHit &point2) noexcept
{
  // INFO y = slope * z + intercept // no need to handle zero denominator case
  return PointsToLine(point1.fZ, point1.fY, point2.fZ, point2.fY);
}

inline float PointLineDistance(const Point2D &point, const Line &line) noexcept
{
  // INFO y = slope * z + intercept
  return std::fabs(point.fOrdinate - line.fSlope * point.fAbscissa - line.fIntercept) / std::sqrt(line.fSlope * line.fSlope + 1);
}

inline float StrawXZLineDistance(const PndFtsStraw &straw, const Line &line) noexcept
{
  // INFO x = slope * z + intercept
  return std::fabs(straw.fX - line.fSlope * straw.fZ - line.fIntercept) / std::sqrt(line.fSlope * line.fSlope + 1);
}

inline float LineYZPointDistance(const YZVirtualHit &point, const Line &line) noexcept
{
  // INFO y = slope * z + intercept
  return std::fabs(point.fY - line.fSlope * point.fZ - line.fIntercept) / std::sqrt(line.fSlope * line.fSlope + 1);
}

inline XZPoint TangencyPoint(const float r, const XZPoint &s, const XZPoint &p, const float sign) noexcept
{
  // INFO based on https://mathworld.wolfram.com/CircleTangentLine.html
  const float distSq = (p.fZ - s.fZ) * (p.fZ - s.fZ) + (p.fX - s.fX) * (p.fX - s.fX);
  const float distMinusRsq = std::sqrt(distSq - r * r);
  const float x = s.fX + r * (r * (p.fX - s.fX) - sign * (p.fZ - s.fZ) * distMinusRsq) / distSq;
  const float z = s.fZ + r * (r * (p.fZ - s.fZ) + sign * (p.fX - s.fX) * distMinusRsq) / distSq;
  return {x, z};
}

inline Circle LineStrawCircle(const Line &xzLine, const float zTangency, const PndFtsStraw &straw, const float isochrone = 0.) noexcept
{
  // INFO Constructs circle tangent to straw and xz_line in point with Z = z_tangency
  const float xTangency = LineValue(xzLine, zTangency);

  const Line tanOriLine{-1 / xzLine.fSlope, xTangency + zTangency / xzLine.fSlope};
  const Line tanStrawLine = PointsToLine(zTangency, xTangency, straw.fZ, straw.fX + isochrone);

  const XZPoint tanStrawMid{0.5f * (xTangency + straw.fX + isochrone), 0.5f * (zTangency + straw.fZ)};

  const Line midOriLine{-1 / tanStrawLine.fSlope, tanStrawMid.fX + tanStrawMid.fZ / tanStrawLine.fSlope};

  const float originZ = (midOriLine.fIntercept - tanOriLine.fIntercept) / (tanOriLine.fSlope - midOriLine.fSlope);
  const float originX = LineValue(midOriLine, originZ);

  const XZPoint origin{originX, originZ};
  const float radius = std::sqrt(std::pow(zTangency - origin.fZ, 2.f) + std::pow(xTangency - origin.fX, 2.f));

  return {radius, origin};
}

inline float StrawCircleDistance(const PndFtsStraw &straw, const Circle &circle) noexcept
{
  const float strawOriDistSq = std::pow(straw.fX - circle.fOrigin.fX, 2.f) + std::pow(straw.fZ - circle.fOrigin.fZ, 2.f);
  const float strawOriDist = std::sqrt(strawOriDistSq);

  return std::abs(circle.fRadius - strawOriDist);
}

inline float XZCircleXCoord(const Circle &circle, const float zCoord) noexcept
{
  const double radiusSq = std::pow(circle.fRadius, 2);
  const double zDistSq = std::pow(zCoord - circle.fOrigin.fZ, 2);
  return static_cast<float>(std::sqrt(std::abs(radiusSq - zDistSq)));
}

} // namespace PndFtsTrackFinder::PndFtsMath
