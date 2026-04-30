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

#include "FairLogger.h"

#include "tool/PndFtsMath.h"
#include "tool/PndFtsCircleHitSet.h"
#include "PndFtsReconCircleZOX.h"

namespace PndFtsTrackFinder {

PndFtsReconCircleZOX::PndFtsReconCircleZOX(const PndFtsContext &context) : fContext{context} {}

CircleSubtrackVector PndFtsReconCircleZOX::Exec(const LineTrackVector &ft12Tracks, const PndFtsCircleHitSet &hitSet) const
{
  CircleSubtrackVector foundTracks{};

  for (const auto &ft12Track : ft12Tracks) {
    if (hitSet.IsComplete()) {
      foundTracks.emplace_back(ExecItem(ft12Track, hitSet));
    } else {
      if (fContext.fDebug > 0) {
        LOG(info) << "PndFtsTrackReconCircleZOX::Exec: "
                  << "Incomplete HitSet. Adding empty CircleTrack.\n";
      }
      foundTracks.emplace_back(CircleSubtrack{std::numeric_limits<float>::max(), {}, {}, ft12Track.fParentEventId});
    }
  }

  return foundTracks;
}

CircleSubtrack PndFtsReconCircleZOX::ExecItem(const LineTrack &ft12Track, const PndFtsCircleHitSet &hits) const
{
  CircleSubtrack circleTrack{std::numeric_limits<float>::max(), {}, {}, ft12Track.fParentEventId};

  SimpleHitVector candidateHits{};
  candidateHits.reserve(8);

  float ddMeanMin = std::numeric_limits<float>::max();

  for (const auto &hit3132 : hits.fLayer3132) {
    const std::array<Circle, 2> circles{
      PndFtsMath::LineStrawCircle(ft12Track.fLineZOX, fContext.fConstants.fZBStart + fContext.fConstants.fZBCurvOffset, fContext.fGeom.Straw(hit3132), hit3132.fIsochrone),
      PndFtsMath::LineStrawCircle(ft12Track.fLineZOX, fContext.fConstants.fZBStart + fContext.fConstants.fZBCurvOffset, fContext.fGeom.Straw(hit3132), -hit3132.fIsochrone)};

    for (const auto &circle : circles) {
      float dd = 0;
      candidateHits.clear();
      std::array<PndFtsSimpleHit, 16> candidateHitsArr;
      std::array<float, 16> dArr;
      dArr.fill(std::numeric_limits<float>::max());

      for (const auto &hit : hits.fVertical) {
        const auto &straw = fContext.fGeom.Straw(hit);
        const auto hitCircDist = PndFtsMath::StrawCircleDistance(straw, circle);
        const float ds = std::fabs(hitCircDist - hit.fIsochrone);

        const auto iLayer = straw.fLayerId % 16;

        if (ds < fContext.fConstants.fDeltaC and ds < dArr[iLayer]) {
          candidateHitsArr[iLayer] = hit;
          dArr[iLayer] = ds;
        }
      }

      for (auto iLayer = 0; iLayer < 16; ++iLayer) {
        if (dArr[iLayer] < std::numeric_limits<float>::max()) {
          candidateHits.emplace_back(candidateHitsArr[iLayer]);
          dd += dArr[iLayer];
        }
      }

      if (candidateHits.size() > 2) {
        const float dd_mean = dd / static_cast<float>(candidateHits.size());

        if (dd_mean <= ddMeanMin) {
          circleTrack.fDd = dd;
          circleTrack.fHits = candidateHits;
          circleTrack.fCircle = circle;
          ddMeanMin = dd_mean;
        }
      }
    }
  }

  return circleTrack;
}

} // namespace PndFtsTrackFinder
