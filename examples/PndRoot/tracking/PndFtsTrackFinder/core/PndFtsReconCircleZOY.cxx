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
// Created by Bartosz Sobol on 11.03.2021.
//

#include <numeric>
#include <iostream>

#include "tool/PndFtsMath.h"
#include "tool/PndFtsCircleHitSet.h"
#include "PndFtsReconCircleZOY.h"

namespace PndFtsTrackFinder {

PndFtsReconCircleZOY::PndFtsReconCircleZOY(const PndFtsContext &context) : fContext{context} {}

LineSubtrackVector PndFtsReconCircleZOY::Exec(const LineTrackVector &ft12Tracks, const PndFtsCircleHitSet &hitSet, const CircleSubtrackVector &zoxTracks) const
{
  LineSubtrackVector foundTracks{};

  for (std::size_t iTrack = 0; iTrack < ft12Tracks.size(); ++iTrack) {
    const auto &ft12Track = ft12Tracks[iTrack];
    const auto &zoxTrack = zoxTracks[iTrack];

    if (zoxTrack.fDd < std::numeric_limits<float>::max()) {
      foundTracks.emplace_back(ExecItem(ft12Track, hitSet, zoxTrack));
    } else {
      foundTracks.emplace_back(LineSubtrack{std::numeric_limits<float>::max(), {}, {}, ft12Track.fParentEventId});
    }
  }

  return foundTracks;
}

LineSubtrack PndFtsReconCircleZOY::ExecItem(const LineTrack &ft12Track, const PndFtsCircleHitSet &hits, const CircleSubtrack &zoxTrack) const
{
  LineSubtrack foundTrack{0, ft12Track.fLineZOY, ft12Track.fParentEventId};

  std::array<float, 16> dmin{};
  std::array<PndFtsSimpleHit, 16> min_track_hits{};
  dmin.fill(std::numeric_limits<float>::max());

  for (const auto &hit : hits.fSkewed) {
    const auto &hitStraw = fContext.fGeom.Straw(hit);

    const float xRelative = PndFtsMath::XZCircleXCoord(zoxTrack.fCircle, hitStraw.fZ);

    const float xOnCircle1 = zoxTrack.fCircle.fOrigin.fX + xRelative;
    const float xOnCircle2 = zoxTrack.fCircle.fOrigin.fX - xRelative;

    const float xShift = PndFtsMath::LineValue(ft12Track.fLineZOX, fContext.fConstants.fZBStart);

    const float xOnCircle = (std::abs(xOnCircle1 - xShift) < std::abs(xOnCircle2 - xShift)) ? xOnCircle1 : xOnCircle2;

    const float yOnStraw = hitStraw.fSlope * (xOnCircle - hitStraw.fX) + hitStraw.fY;
    const float yOnTrack = PndFtsMath::LineValue(ft12Track.fLineZOY, hitStraw.fZ);

    const float d = std::abs(std::abs(yOnStraw - yOnTrack) - hit.fIsochrone);

    const std::size_t iD = hitStraw.fLayerId % 16;

    if (d < fContext.fConstants.fDelta2 * 2 and d < dmin[iD]) {
      dmin[iD] = d;
      min_track_hits[iD] = hit;
    }
  }

  for (std::size_t i = 0; i < dmin.size(); ++i) {
    if (dmin[i] < std::numeric_limits<float>::max()) {
      foundTrack.fHits.emplace_back(min_track_hits[i]);
      foundTrack.fDd += dmin[i];
    }
  }

  return foundTrack;
}

} // namespace PndFtsTrackFinder
