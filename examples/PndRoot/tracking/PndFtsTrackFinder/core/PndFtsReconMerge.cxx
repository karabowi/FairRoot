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
// Created by Bartosz Sobol on 30.03.2021.
//

#include <cmath>
#include <algorithm>

#include <FairLogger.h>

#include "PndFtsReconMerge.h"
#include "tool/PndFtsMath.h"

namespace PndFtsTrackFinder {

PndFtsReconMerge::PndFtsReconMerge(const PndFtsContext &context) : fContext{context} {}

FullTrackVector PndFtsReconMerge::Exec(const LineTrackVector &ft12Tracks, const CircleTrackVector &ft34Tracks, const LineTrackVector &ft56Tracks) const
{
  FullTrackVector mergedTracks{};

  if (ft12Tracks.size() != ft34Tracks.size()) {
    LOG(fatal) << "PndFtsTrackReconMerge::Exec:"
               << "FT12 and FT34 track sets have different sizes: " << ft12Tracks.size() << " and " << ft34Tracks.size() << ". Returning empty track set." << std::endl;
    return {};
  }

  for (std::size_t iTrack = 0; iTrack < ft12Tracks.size(); ++iTrack) {
    const auto &ft12Track = ft12Tracks[iTrack];
    const auto &ft34Track = ft34Tracks[iTrack];

    mergedTracks.emplace_back(ft12Track.fLineZOX, ft12Track.fLineZOY, ft34Track.fCircleZOX, ft34Track.fLineZOY, ft12Track.fParentEventId);

    auto &hits = mergedTracks.back().fHits;

    hits.insert(hits.end(), ft12Track.fHits.cbegin(), ft12Track.fHits.cend());
    hits.insert(hits.end(), ft34Track.fHits.cbegin(), ft34Track.fHits.cend());
  }

  std::vector<TrackRange> track1234Ranges{{0, static_cast<uint32_t>(ft12Tracks.size())}};
  std::vector<TrackRange> track56Ranges{{0, static_cast<uint32_t>(ft56Tracks.size())}};

  std::vector<std::optional<std::size_t>> foundMatches{};
  foundMatches.resize(ft56Tracks.size());

  ExecItem(ft12Tracks, ft56Tracks, ft34Tracks, track56Ranges, track1234Ranges, foundMatches);

  for (std::size_t iMatch = 0; iMatch < foundMatches.size(); ++iMatch) {
    const auto &match = foundMatches[iMatch];
    const auto &ft56Track = ft56Tracks[iMatch];

    if (match) {
      auto &finalTrack = mergedTracks[match.value()];
      finalTrack.fFT56LineZOX = ft56Track.fLineZOX;
      finalTrack.fFT56LineZOY = ft56Track.fLineZOY;
      finalTrack.fHits.insert(finalTrack.fHits.end(), ft56Track.fHits.cbegin(), ft56Track.fHits.cend());
      std::sort(finalTrack.fHits.begin(), finalTrack.fHits.end());
    }
  }

  return mergedTracks;
}

void PndFtsReconMerge::ExecItem(const LineTrackVector &ft12Tracks, const LineTrackVector &ft56Tracks, const CircleTrackVector &ft34Tracks,
                                const std::vector<TrackRange> &track56Ranges, const std::vector<TrackRange> &track1234Ranges,
                                std::vector<std::optional<std::size_t>> &foundMatches) const
{
  const TrackRange event1234Tracks = track1234Ranges[0];
  const TrackRange event56Tracks = track56Ranges[0];

  for (auto iFt56Track = event56Tracks.fBegin; iFt56Track - event56Tracks.fBegin < event56Tracks.fNTracks; ++iFt56Track) {
    std::optional<std::size_t> iFT1234Match{};
    float dMinSum = std::numeric_limits<float>::max();

    const auto &ft56Track = ft56Tracks[iFt56Track];

    const float ft56X = PndFtsMath::LineValue(ft56Track.fLineZOX, fContext.fConstants.fZBEnd);
    const float ft56Y = PndFtsMath::LineValue(ft56Track.fLineZOY, fContext.fConstants.fZBEnd);

    for (auto iTrack = event1234Tracks.fBegin; iTrack - event1234Tracks.fBegin < event1234Tracks.fNTracks; ++iTrack) {

      const auto begIt = foundMatches.cbegin() + event56Tracks.fBegin;
      if (std::find(begIt, begIt + event56Tracks.fNTracks, iTrack) != begIt + event56Tracks.fNTracks) {
        continue;
      }

      const auto &ft12Track = ft12Tracks[iTrack];
      const auto &ft34Track = ft34Tracks[iTrack];

      const float ft34XRelative = PndFtsMath::XZCircleXCoord(ft34Track.fCircleZOX, fContext.fConstants.fZBEnd);

      const float ft34XOnCircle1 = ft34Track.fCircleZOX.fOrigin.fX + ft34XRelative;
      const float ft34XOnCircle2 = ft34Track.fCircleZOX.fOrigin.fX - ft34XRelative;

      const float ft34XShift = PndFtsMath::LineValue(ft12Track.fLineZOX, fContext.fConstants.fZBEnd);

      const float ft34X = (std::abs(ft34XOnCircle1 - ft34XShift) < std::abs(ft34XOnCircle2 - ft34XShift)) ? ft34XOnCircle1 : ft34XOnCircle2;

      const float ft34Y = PndFtsMath::LineValue(ft34Track.fLineZOY, fContext.fConstants.fZBEnd);

      const float dZOX = std::abs(ft56X - ft34X);
      const float dZOY = std::abs(ft56Y - ft34Y);

      const float dSum = dZOX + dZOY;

      if (dSum < dMinSum and dSum <= 50) {
        dMinSum = dSum;
        iFT1234Match = iTrack;
      }
    }

    foundMatches[iFt56Track] = iFT1234Match;
  }
}

} // namespace PndFtsTrackFinder