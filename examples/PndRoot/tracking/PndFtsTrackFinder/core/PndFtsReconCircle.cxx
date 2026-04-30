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
// Created by Bartosz Sobol on 21.12.2021.
//

#include "FairLogger.h"

#include "PndFtsReconCircle.h"

namespace PndFtsTrackFinder {

[[nodiscard]] CircleTrackVector PndFtsReconCircle::Exec(const LineTrackVector &ft12Tracks, const SimpleHitVector &inHits) const
{
  if (fContext.fVerbose > 0 and inHits.empty()) {
    LOG(info) << "PndFtsTrackReconCircle::Exec:"
              << "Empty input inHits. Returning empty track set.\n";
    return {};
  }

  const auto hitSet = PndFtsCircleHitSet{inHits, fContext.fGeom};

  const auto foundTracksZOX = fReconZOX.Exec(ft12Tracks, hitSet);

  if (fContext.fVerbose > 0 and foundTracksZOX.empty()) {
    LOG(info) << "PndFtsTrackReconCircle::Exec:"
              << "Empty ZOX tracks found. Returning empty track set.\n";
    return {};
  }

  const auto foundTracksZOY = fReconZOY.Exec(ft12Tracks, hitSet, foundTracksZOX);

  if (fContext.fVerbose > 0 and foundTracksZOY.empty()) {
    LOG(info) << "PndFtsTrackReconCircle::Exec:"
              << "Empty ZOY tracks found. Returning empty track set.\n";
    return {};
  }
  if (foundTracksZOX.size() != foundTracksZOY.size()) {
    LOG(error) << "PndFtsTrackReconCircle::Exec:"
               << "ZOX and ZOY track sets have different sizes: " << foundTracksZOX.size() << " and " << foundTracksZOX.size() << ". Returning empty track set." << std::endl;
    return {};
  }

  CircleTrackVector foundTracks{};
  foundTracks.reserve(foundTracksZOX.size());

  for (std::size_t i = 0; i < foundTracksZOX.size(); ++i) {
    const auto &zoxTrack = foundTracksZOX[i];
    const auto &zoyTrack = foundTracksZOY[i];

    foundTracks.emplace_back(zoxTrack.fCircle, zoyTrack.fLine, zoxTrack.fParentEventId);

    auto &hits = foundTracks.back().fHits;
    hits.reserve(zoxTrack.fHits.size() + zoyTrack.fHits.size());

    hits.insert(hits.end(), zoxTrack.fHits.cbegin(), zoxTrack.fHits.cend());
    hits.insert(hits.end(), zoyTrack.fHits.cbegin(), zoyTrack.fHits.cend());

    std::sort(hits.begin(), hits.end());
  }

  return foundTracks;
}

} // namespace PndFtsTrackFinder
