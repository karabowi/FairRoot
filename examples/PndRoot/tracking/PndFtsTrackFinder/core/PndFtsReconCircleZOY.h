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

#include "tool/PndFtsLineHitSet.h"
#include "tool/PndFtsContext.h"
#include "tool/PndFtsTrack.h"

namespace PndFtsTrackFinder {

class PndFtsReconCircleZOY {
 public:
  PndFtsReconCircleZOY() = delete;

  explicit PndFtsReconCircleZOY(const PndFtsContext &context);

  PndFtsReconCircleZOY(const PndFtsContext &&context) = delete;

  [[nodiscard]] LineSubtrackVector Exec(const LineTrackVector &ft12Tracks, const PndFtsCircleHitSet &hitSet, const CircleSubtrackVector &zoxTracks) const;

 private:
  [[nodiscard]] LineSubtrack ExecItem(const LineTrack &ft12Track, const PndFtsCircleHitSet &hits, const CircleSubtrack &zoxTrack) const;

  const PndFtsContext &fContext;
};

} // namespace PndFtsTrackFinder
