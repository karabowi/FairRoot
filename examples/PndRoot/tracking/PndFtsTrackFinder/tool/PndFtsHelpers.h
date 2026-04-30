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
// Created by Bartosz Sobol on 13.05.2020.
//

#pragma once

#include <utility>
#include <vector>
#include <array>
#include <algorithm>

#include "PndFtsStraw.h"
#include "PndFtsGeom.h"
#include "PndFtsSimpleHit.h"

namespace PndFtsTrackFinder {

struct XZPoint : public TObject {
  XZPoint() = default;

  inline explicit XZPoint(const PndFtsStraw &straw) : fX{straw.fX}, fZ{straw.fZ} {};

  inline XZPoint(const float x, const float z) : fX{x}, fZ{z} {};

  float fX{};
  float fZ{};

  ClassDef(XZPoint, 1);
};

struct Line : public TObject {
  Line() = default;

  Line(float slope, float intercept) : fSlope{slope}, fIntercept{intercept} {}

  float fSlope{};
  float fIntercept{};

  ClassDef(Line, 1);
};

using TangentLineArray = std::array<Line, 4>;

struct Circle : public TObject {
  Circle() = default;

  Circle(float radius, XZPoint origin) : fRadius{radius}, fOrigin{std::move(origin)} {}

  float fRadius{};
  XZPoint fOrigin{};

  ClassDef(Circle, 1);
};

struct Point2D {
  inline Point2D(float abscissa, float ordinate) : fAbscissa(abscissa), fOrdinate(ordinate) {}

  float fAbscissa;
  float fOrdinate;
};

struct YZVirtualHit {
  YZVirtualHit() = default;

  inline YZVirtualHit(const PndFtsSimpleHit &baseHit, float y, float z) : fBaseHit{baseHit}, fY{y}, fZ{z} {};

  PndFtsSimpleHit fBaseHit;
  float fY;
  float fZ;
};

using ZYVirtualHitVector = std::vector<YZVirtualHit>;
using ZYVirtualHitVectorPair = std::pair<ZYVirtualHitVector, ZYVirtualHitVector>;
using ZYVirtualHitPair = std::pair<YZVirtualHit, YZVirtualHit>;
using ZYVirtualHitPairVector = std::vector<ZYVirtualHitPair>;

struct TrackRange {
  uint32_t fBegin;
  uint32_t fNTracks;
};

} // namespace PndFtsTrackFinder
