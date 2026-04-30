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

#include "PndFtsReconLine.h"

namespace PndFtsTrackFinder {

[[nodiscard]] LineTrackVector PndFtsReconLine::Exec(const SimpleHitVector &inHits) const
{
  if (fContext.fVerbose > 0 and inHits.empty()) {
    LOG(info) << "PndFtsTrackReconLine::Exec:"
              << "Empty inHits. Returning empty track set.\n";
    return {};
  }

  const auto hitSet = PndFtsLineHitSet{inHits, fContext.fGeom};

  const auto foundTracksZOX = [this, &hitSet]() {
    if (fContext.fDebug) {
      auto reconZoxResult = fReconZOX.Exec(hitSet);

      std::stable_sort(reconZoxResult.begin(), reconZoxResult.end(), [](const auto &lhsTrack, const auto &rhsTrack) {
        return lhsTrack.fParentEventId < rhsTrack.fParentEventId || (lhsTrack.fParentEventId == rhsTrack.fParentEventId && lhsTrack.fDd < rhsTrack.fDd);
      });

      return reconZoxResult;
    } else {
      return fReconZOX.Exec(hitSet);
    }
  }();

  if (fContext.fVerbose > 0 and foundTracksZOX.empty()) {
    LOG(info) << "PndFtsTrackReconLine::Exec:"
              << "Empty ZOX tracks found. Returning empty track set.\n";
    return {};
  }

  const auto foundTracksZOY = fReconZOY.Exec(foundTracksZOX, hitSet);

  if (fContext.fVerbose > 0 and foundTracksZOY.empty()) {
    LOG(info) << "PndFtsTrackReconLine::Exec:"
              << "Empty ZOX tracks found. Returning empty track set.\n";
    return {};
  }
  if (foundTracksZOX.size() != foundTracksZOY.size()) {
    LOG(error) << "PndFtsTrackReconLine::Exec:"
               << "ZOX and ZOY track sets have different sizes: " << foundTracksZOX.size() << " and " << foundTracksZOX.size() << ". Returning empty track set." << std::endl;
    return {};
  }

  LineTrackVector foundTracks{};
  foundTracks.reserve(foundTracksZOX.size());

  const float maxFloat = std::numeric_limits<float>::max();
  for (std::size_t i = 0; i < foundTracksZOX.size(); ++i) {
    const auto &zoxTrack = foundTracksZOX[i];
    const auto &zoyTrack = foundTracksZOY[i];

    if (zoxTrack.fDd < maxFloat && zoyTrack.fDd < maxFloat) {
      foundTracks.emplace_back(zoxTrack.fLine, zoyTrack.fLine, zoxTrack.fParentEventId);

      auto &hits = foundTracks.back().fHits;
      hits.reserve(zoxTrack.fHits.size() + zoyTrack.fHits.size());

      hits.insert(hits.end(), zoxTrack.fHits.cbegin(), zoxTrack.fHits.cend());
      hits.insert(hits.end(), zoyTrack.fHits.cbegin(), zoyTrack.fHits.cend());

      std::sort(hits.begin(), hits.end());
    }
  }

  return foundTracks;
}

} // namespace PndFtsTrackFinder