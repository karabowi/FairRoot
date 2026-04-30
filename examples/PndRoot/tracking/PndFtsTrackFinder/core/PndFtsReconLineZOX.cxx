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

#include <algorithm>

#include "FairLogger.h"

#include "tool/PndFtsLineHitSet.h"
#include "tool/PndFtsMath.h"
#include "PndFtsReconLineZOX.h"

namespace PndFtsTrackFinder {

PndFtsReconLineZOX::PndFtsReconLineZOX(const PndFtsContext &context) : fContext{context} {}

LineSubtrackVector PndFtsReconLineZOX::Exec(const PndFtsLineHitSet &hitSet) const
{
  LineSubtrackVector foundTracks{};
  // This will not be enough in most cases, but will still save many reallocations
  foundTracks.reserve(8);

  if (hitSet.IsComplete()) {
    auto eventTracks = ExecItem(hitSet);
    RemoveDuplicates(eventTracks);

    const auto newEnd = std::remove_if(eventTracks.begin(), eventTracks.end(), [](const auto &track) { return track.fDd == std::numeric_limits<float>::max(); });

    eventTracks.erase(newEnd, eventTracks.cend());

    foundTracks.insert(foundTracks.cend(), eventTracks.cbegin(), eventTracks.cend());
  }

  return foundTracks;
}

LineSubtrackVector PndFtsReconLineZOX::ExecItem(const PndFtsLineHitSet &hits) const
{
  // TODO 0.5 d_bias is inconsistent between publications, equals 0.55 in other one (similar case with delta_l)
  // TODO FT12 optimizations //creates additional branches

  LineSubtrackVector foundTracks{};
  foundTracks.reserve(hits.fOutermostVertical.size() / 2);

  SimpleHitVector candidateHits{};
  candidateHits.reserve(8);
  std::vector<SimpleHitVector> tangentMatchHits(4);
  std::vector<float> dd(4);

  for (const auto &hitBeg : hits.fOutermostVertical) {
    const auto &strawBeg = fContext.fGeom.Straw(hitBeg);
    for (const auto &hitEnd : hits.fOutermostVertical) {
      const auto &strawEnd = fContext.fGeom.Straw(hitEnd);

      if (strawEnd.fLayerId <= strawBeg.fLayerId + 1) {
        continue;
      }

      const float dThreshold = std::max(hitBeg.fIsochrone, hitEnd.fIsochrone) + fContext.fConstants.fDBias;
      const auto wireWireLine = PndFtsMath::XZStrawsToLine(strawBeg, strawEnd);

      candidateHits.clear();

      std::array<PndFtsSimpleHit, 16> candidateHitsArr;
      std::array<float, 16> dArr;
      dArr.fill(std::numeric_limits<float>::max());

      for (const auto &hitVertical : hits.fVertical) {
        const auto &straw = fContext.fGeom.Straw(hitVertical);

        const auto dValue = PndFtsMath::StrawXZLineDistance(straw, wireWireLine);
        const auto iLayer = straw.fLayerId % 16;

        if (dValue < dThreshold and dValue < dArr[iLayer]) {
          candidateHitsArr[iLayer] = hitVertical;
          dArr[iLayer] = dValue;
        }
      }

      for (auto iLayer = 0; iLayer < 16; ++iLayer) {
        if (dArr[iLayer] < std::numeric_limits<float>::max()) {
          candidateHits.emplace_back(candidateHitsArr[iLayer]);
        }
      }

      if (candidateHits.size() <= 6) {
        continue;
      }

      tangentMatchHits.clear();
      dd.clear();
      const auto tangents = TangentLines(hitBeg, hitEnd);

      for (const auto &tangent : tangents) {
        tangentMatchHits.emplace_back().reserve(8);
        dd.emplace_back(0);
        for (const auto &hit : candidateHits) {
          const auto &straw = fContext.fGeom.Straw(hit);
          const float ds = std::abs(PndFtsMath::StrawXZLineDistance(straw, tangent) - hit.fIsochrone);
          if (ds < fContext.fConstants.fDeltaL) {
            tangentMatchHits.back().emplace_back(hit);
            dd.back() += ds;
          }
        }
      }

      std::size_t iMinDD = 0;
      for (std::size_t iMatch = 0; iMatch < tangentMatchHits.size(); ++iMatch) {
        if (tangentMatchHits[iMatch].size() <= 6) {
          dd[iMatch] = std::numeric_limits<float>::max();
          continue;
        }
        if (dd[iMatch] < dd[iMinDD]) {
          iMinDD = iMatch;
        }
      }

      if (dd[iMinDD] < std::numeric_limits<float>::max()) {
        // INFO tangent_match_hits[min_dd_i] contains track candidate hits
        foundTracks.emplace_back(dd[iMinDD], tangents[iMinDD], std::move(tangentMatchHits[iMinDD]), 0);
      }
    }
  }

  return foundTracks;
}

TangentLineArray PndFtsReconLineZOX::TangentLines(const PndFtsSimpleHit &hitBeg, const PndFtsSimpleHit &hitEnd) const
{
  const auto &strawBeg = fContext.fGeom.Straw(hitBeg);
  const auto &strawEnd = fContext.fGeom.Straw(hitEnd);
  const XZPoint p1{strawBeg}, p2{strawEnd};
  const float r1 = hitBeg.fIsochrone, r2 = hitEnd.fIsochrone;

  // INFO there will always be 4 tangent lines (because of distance between straws)
  // TODO r1 == r2 case not handled !!!
  const XZPoint outerIntersect = [&]() {
    if (r1 > r2) { // TODO is this if necessary?
      return XZPoint{(r2 * p1.fX - r1 * p2.fX) / (r2 - r1), (r2 * p1.fZ - r1 * p2.fZ) / (r2 - r1)};
    }
    return XZPoint{(r1 * p2.fX - r2 * p1.fX) / (r1 - r2), (r1 * p2.fZ - r2 * p1.fZ) / (r1 - r2)};
  }();

  const XZPoint outerPoint1 = PndFtsMath::TangencyPoint(r1, p1, outerIntersect, -1.);
  const XZPoint outerPoint2 = PndFtsMath::TangencyPoint(r1, p1, outerIntersect, 1.);

  const XZPoint innerIntersect{(r1 * p2.fX + r2 * p1.fX) / (r1 + r2), (r1 * p2.fZ + r2 * p1.fZ) / (r1 + r2)};

  const XZPoint innerPoint1 = PndFtsMath::TangencyPoint(r1, p1, innerIntersect, -1.);
  const XZPoint innerPoint2 = PndFtsMath::TangencyPoint(r1, p1, innerIntersect, 1.);

  return {PndFtsMath::XZPointsToLine(outerIntersect, outerPoint1), PndFtsMath::XZPointsToLine(outerIntersect, outerPoint2), PndFtsMath::XZPointsToLine(innerIntersect, innerPoint1),
          PndFtsMath::XZPointsToLine(innerIntersect, innerPoint2)};
}

void PndFtsReconLineZOX::RemoveDuplicates(LineSubtrackVector &tracks)
{

  //  Two track candidates are considered to be an ``repetition event'' if
  //  they contain the same lit straws in at least 7 out of 8 layers or  //COND_1
  //  if out of 7 hit straws no more than 4 straws are not exactly the same but have neighboring numbers. //COND_2
  //  out of two such candidates the one with more hits or with smaller value of dd is accepted.
  // INFO I'm not able to make predicate from this, one from previous implementation is used below

  constexpr auto maxFloat = std::numeric_limits<float>::max();
  for (std::size_t iLhs = 0; iLhs < tracks.size(); ++iLhs) {
    auto &lhsTrack = tracks[iLhs];

    for (std::size_t iRhs = iLhs + 1; iRhs < tracks.size(); ++iRhs) {
      auto &rhsTrack = tracks[iRhs];

      if (lhsTrack.fDd == maxFloat) {
        break;
      }
      if (rhsTrack.fDd == maxFloat) {
        continue;
      }

      int exact = 0;
      int neighbour = 0;
      for (const auto &lhsHit : lhsTrack.fHits) {
        const int lhsStrawId = lhsHit.fStrawId;

        for (const auto &rhsHit : rhsTrack.fHits) {
          const int rhsStrawId = rhsHit.fStrawId;
          if (lhsStrawId == rhsStrawId) {
            ++exact;
            break;
          }
          if (std::abs(lhsStrawId - rhsStrawId) == 1) {
            // INFO subtracting uints but promoted to signed, so result is ok
            ++neighbour;
            break;
          }
        }
      }

      const bool removeCondition = (exact > 6) || (exact == 6 && neighbour > 0) || (exact == 5 && neighbour > 1) || (exact == 4 && neighbour > 2) || (exact == 3 && neighbour > 3);

      if (removeCondition) {
        const auto lhsSize = lhsTrack.fHits.size();
        const auto rhsSize = rhsTrack.fHits.size();
        if (lhsSize == rhsSize) {
          if (lhsTrack.fDd < rhsTrack.fDd) {
            rhsTrack.fDd = maxFloat;
          } else {
            lhsTrack.fDd = maxFloat;
          }
        } else if (lhsSize > rhsSize) {
          rhsTrack.fDd = maxFloat;
        } else {
          lhsTrack.fDd = maxFloat;
        }
      }
    }
  }
}

} // namespace PndFtsTrackFinder