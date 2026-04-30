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
// Created by Bartosz Sobol on 07.06.2020.
//

#include <cmath>
#include <algorithm>
#include <numeric>

#include "tool/PndFtsLineHitSet.h"
#include "tool/PndFtsMath.h"
#include "tool/PndFtsUtility.h"

#include "PndFtsReconLineZOY.h"

namespace PndFtsTrackFinder {

PndFtsReconLineZOY::PndFtsReconLineZOY(const PndFtsContext &context) : fContext{context} {}

LineSubtrackVector PndFtsReconLineZOY::Exec(const LineSubtrackVector &zoxTracks, const PndFtsLineHitSet &hitSet) const
{
  LineSubtrackVector found_tracks{};

  for (const auto &zox_track : zoxTracks) {
    found_tracks.emplace_back(ExecItem(zox_track, hitSet));
  }

  return found_tracks;
}

LineSubtrack PndFtsReconLineZOY::ExecItem(const LineSubtrack &zoxTrack, const PndFtsLineHitSet &hits) const
{
  const bool ft12{fContext.fGeom.Straw(hits.fSkewed.front()).fStation == PndFtsStation::FT12};
  const auto [virtualHits, virtualHitsOutermost] = CreateVirtualHits(zoxTrack, hits);

  SimpleHitVector trackHits{};
  trackHits.reserve(8);

  float dd = std::numeric_limits<float>::max();
  std::array<float, 16> dArr{};
  std::array<float, 16> dMin{};
  dMin.fill(std::numeric_limits<float>::max());
  std::array<std::size_t, 16> minTrackIndices{};
  Line minLine{};

  for (const auto &hitBeg : virtualHitsOutermost) {
    for (const auto &hitEnd : virtualHitsOutermost) {
      const auto layerBeg = fContext.fGeom.Straw(hitBeg.fBaseHit).fLayerId;
      const auto layerEnd = fContext.fGeom.Straw(hitEnd.fBaseHit).fLayerId;

      if (layerEnd <= layerBeg + 1) {
        continue;
      }

      std::array<std::size_t, 16> tmpMinTrackIndices{};
      dArr.fill(std::numeric_limits<float>::max());

      const auto hitHitLine = PndFtsMath::YZPointsToLine(hitBeg, hitEnd);

      for (std::size_t iPoint = 0; iPoint < virtualHits.size(); ++iPoint) {
        const auto &point = virtualHits[iPoint];
        const auto pointLineDist = PndFtsMath::LineYZPointDistance(point, hitHitLine);
        const std::size_t iLayer = fContext.fGeom.Straw(point.fBaseHit).fLayerId % 16; // INFO layer index in darr

        if (pointLineDist < fContext.fConstants.fDelta2 && pointLineDist < dArr[iLayer]) {
          dArr[iLayer] = pointLineDist;
          tmpMinTrackIndices[iLayer] = iPoint;
        }
      }

      const auto nCandidateHits = std::count_if(dArr.cbegin(), dArr.cend(), &PndFtsUtility::IfNotMax<float>);

      if (nCandidateHits > 6) {
        const float dArrSum = std::accumulate(dArr.cbegin(), dArr.cend(), 0.f, &PndFtsUtility::PlusIfNotMax<float>);
        if (dArrSum < dd) {
          dd = dArrSum;
          dMin = dArr;
          minTrackIndices = tmpMinTrackIndices;
          minLine = hitHitLine;
        }
      }
    }
  }

  for (std::size_t i = 0; i < dMin.size(); ++i) {
    if (dMin[i] < std::numeric_limits<float>::max()) {
      trackHits.emplace_back(virtualHits[minTrackIndices[i]].fBaseHit);
    }
  }

  // INFO when no ZOY track candidate is legitimate subtrack.dd will be set to numeric_limits<float>::max()
  return {dd, ZOYLineTransform(ft12, zoxTrack.fLine, minLine), trackHits, zoxTrack.fParentEventId};
}

ZYVirtualHitVectorPair PndFtsReconLineZOY::CreateVirtualHits(const LineSubtrack &zoxTrack, const PndFtsLineHitSet &hits) const
{
  ZYVirtualHitVector virtualHits{}, virtualHitsOutermost{};
  virtualHits.reserve(hits.fSkewed.size() * 2);
  virtualHitsOutermost.reserve(hits.fOutermostSkewed.size() * 2);

  const auto &zoxLine = zoxTrack.fLine;
  for (const auto &hitSkewed : hits.fSkewed) {
    const auto &straw = fContext.fGeom.Straw(hitSkewed);

    // INFO intersection hit_straw-Z'OY_plane point P(x,y,z)
    const float x = PndFtsMath::LineValue(zoxLine, straw.fZ);
    const float y = straw.fSlope * (x - straw.fX) + straw.fY;
    const float z = std::sqrt(x * x + std::pow(straw.fZ + zoxLine.fIntercept / zoxLine.fSlope, 2.f));

    // INFO may be in fact a good optimisation... should be discussed
    if (x * straw.fX < 0 || std::abs(y) > 70) {
      continue;
    }

    const float cc = hitSkewed.fIsochrone * std::sqrt(1 + straw.fSlope * straw.fSlope);

    const float y1 = y + cc;
    const float y2 = y - cc;

    virtualHits.emplace_back(hitSkewed, y1, z);
    virtualHits.emplace_back(hitSkewed, y2, z);
    if (straw.fOutermost) {
      virtualHitsOutermost.emplace_back(hitSkewed, y1, z);
      virtualHitsOutermost.emplace_back(hitSkewed, y2, z);
    }
  }
  return {virtualHits, virtualHitsOutermost};
}

Line PndFtsReconLineZOY::ZOYLineTransform(const bool ft12, const Line &zoxLine, const Line &minLine) const
{
  const float zBegL = ft12 ? fContext.fGeom.LayerZ(3) : fContext.fGeom.LayerZ(35);
  const float zEndL = ft12 ? fContext.fGeom.LayerZ(14) : fContext.fGeom.LayerZ(46);

  const float xBegL = PndFtsMath::LineValue(zoxLine, zBegL);
  const float xEndL = PndFtsMath::LineValue(zoxLine, zEndL);

  const float zEnd2 = zEndL + zoxLine.fIntercept / zoxLine.fSlope;
  const float zBeg2 = zBegL + zoxLine.fIntercept / zoxLine.fSlope;

  const float zBeg = std::sqrt(xBegL * xBegL + zBeg2 * zBeg2);
  const float yBeg = PndFtsMath::LineValue(minLine, zBeg);

  const float zEnd = std::sqrt(xEndL * xEndL + zEnd2 * zEnd2);
  const float yEnd = PndFtsMath::LineValue(minLine, zEnd);

  return PndFtsMath::PointsToLine(zBegL, yBeg, zEndL, yEnd);
}

} // namespace PndFtsTrackFinder