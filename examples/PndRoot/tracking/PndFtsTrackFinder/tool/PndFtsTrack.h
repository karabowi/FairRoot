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

#pragma once

#include <utility>
#include <vector>

#include "PndFtsHelpers.h"

namespace PndFtsTrackFinder {

struct LineSubtrack {

  LineSubtrack() = default;

  inline LineSubtrack(const float dd, Line line, SimpleHitVector hits, const uint32_t parentEventId)
    : fDd{dd}, fLine{std::move(line)}, fHits{std::move(hits)}, fParentEventId{parentEventId} {};

  inline LineSubtrack(const float dd, Line line, const uint32_t parentEventId) : fDd{dd}, fLine{std::move(line)}, fHits{}, fParentEventId{parentEventId} {};

  float fDd{};
  Line fLine{};
  SimpleHitVector fHits{};
  uint32_t fParentEventId{};
};

using LineSubtrackVector = std::vector<LineSubtrack>;

struct LineTrack {

  LineTrack() = default;

  inline LineTrack(Line lineZOX, Line lineZOY, SimpleHitVector hits, uint32_t parentEventId)
    : fLineZOX{std::move(lineZOX)}, fLineZOY{std::move(lineZOY)}, fHits{std::move(hits)}, fParentEventId{parentEventId} {};

  // It does construct not finished object, but is more efficient.
  inline LineTrack(Line lineZOX, Line lineZOY, uint32_t parentEventId) : fLineZOX{std::move(lineZOX)}, fLineZOY{std::move(lineZOY)}, fHits{}, fParentEventId{parentEventId} {};

  inline bool operator<(const LineTrack &rhs) const { return this->fParentEventId < rhs.fParentEventId; }

  Line fLineZOX{};
  Line fLineZOY{};
  SimpleHitVector fHits{};
  uint32_t fParentEventId{};
};

using LineTrackVector = std::vector<LineTrack>;

struct CircleSubtrack {

  CircleSubtrack() = default;

  inline CircleSubtrack(const float dd, Circle circle, SimpleHitVector hits, const uint32_t parentEventId)
    : fDd{dd}, fCircle{std::move(circle)}, fHits{std::move(hits)}, fParentEventId{parentEventId} {};

  float fDd{};
  Circle fCircle{};
  SimpleHitVector fHits{};
  uint32_t fParentEventId{};
};

using CircleSubtrackVector = std::vector<CircleSubtrack>;

struct CircleTrack {

  inline CircleTrack(Circle circleZOX, Line lineZOY, SimpleHitVector hits, uint32_t parentEventId)
    : fCircleZOX{std::move(circleZOX)}, fLineZOY{std::move(lineZOY)}, fHits{std::move(hits)}, fParentEventId{parentEventId} {};

  // It does construct not finished object, but is more efficient.
  inline CircleTrack(Circle circleZOX, Line lineZOY, uint32_t parentEventId)
    : fCircleZOX{std::move(circleZOX)}, fLineZOY{std::move(lineZOY)}, fHits{}, fParentEventId{parentEventId} {};

  Circle fCircleZOX;
  Line fLineZOY;
  SimpleHitVector fHits;
  uint32_t fParentEventId;

  inline bool operator<(const CircleTrack &rhs) const { return this->fParentEventId < rhs.fParentEventId; }
};

using CircleTrackVector = std::vector<CircleTrack>;

struct PndFtsFullTrack {
  inline PndFtsFullTrack() = default;

  inline PndFtsFullTrack(Line ft12LineZOX, Line ft12LineZOY, Circle ft34CircleZOX, Line ft34LineZOY, uint32_t parentEventId)
    : fFT12LineZOX{std::move(ft12LineZOX)}, fFT12LineZOY{std::move(ft12LineZOY)}, fFT56LineZOX{}, fFT56LineZOY{}, fFT34LineZOY{std::move(ft34LineZOY)},
      fFT34CircleZOX{std::move(ft34CircleZOX)}, fHits{}, fParentEventId{parentEventId}
  {
  }

  inline PndFtsFullTrack(Line ft12LineZOX, Line ft12LineZOY, Line ft56LineZOX, Line ft56LineZOY, Line ft34LineZOY, Circle ft34CircleZOX, SimpleHitVector hits, uint32_t parentEventId)
    : fFT12LineZOX{std::move(ft12LineZOX)}, fFT12LineZOY{std::move(ft12LineZOY)}, fFT56LineZOX{std::move(ft56LineZOX)}, fFT56LineZOY{std::move(ft56LineZOY)},
      fFT34LineZOY{std::move(ft34LineZOY)}, fFT34CircleZOX{std::move(ft34CircleZOX)}, fHits{std::move(hits)}, fParentEventId{parentEventId}
  {
  }

  Line fFT12LineZOX{};
  Line fFT12LineZOY{};
  Line fFT56LineZOX{};
  Line fFT56LineZOY{};
  Line fFT34LineZOY{};
  Circle fFT34CircleZOX{};
  SimpleHitVector fHits{};
  uint32_t fParentEventId{};

  inline bool operator<(const PndFtsFullTrack &rhs) const { return this->fParentEventId < rhs.fParentEventId; }
};

struct PndFtsAnalyticTrack : public FairTimeStamp {
  inline PndFtsAnalyticTrack() = default;

  explicit inline PndFtsAnalyticTrack(const PndFtsFullTrack &fullTrack)
    : fFT12LineZOX{fullTrack.fFT12LineZOX}, fFT12LineZOY{fullTrack.fFT12LineZOY}, fFT56LineZOX{fullTrack.fFT56LineZOX}, fFT56LineZOY{fullTrack.fFT56LineZOY},
      fFT34LineZOY{fullTrack.fFT34LineZOY}, fFT34CircleZOX{fullTrack.fFT34CircleZOX}, fDeltaThetaZOX{std::abs(std::atan(fFT56LineZOX.fSlope) - std::atan(fFT12LineZOX.fSlope))}
  {
  }

  Line fFT12LineZOX{};
  Line fFT12LineZOY{};
  Line fFT56LineZOX{};
  Line fFT56LineZOY{};
  Line fFT34LineZOY{};
  Circle fFT34CircleZOX{};
  float fDeltaThetaZOX{};

  ClassDef(PndFtsAnalyticTrack, 1);
};

using FullTrackVector = std::vector<PndFtsFullTrack>;

} // namespace PndFtsTrackFinder