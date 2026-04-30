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

#include "tool/PndFtsCircleHitSet.h"
#include "tool/PndFtsTrack.h"
#include "tool/PndFtsContext.h"
#include "tool/PndFtsLineHitSet.h"

namespace PndFtsTrackFinder {

class PndFtsReconCircleZOX {
 public:
  PndFtsReconCircleZOX() = delete;

  explicit PndFtsReconCircleZOX(const PndFtsContext &context);

  PndFtsReconCircleZOX(const PndFtsContext &&context) = delete;

  [[nodiscard]] CircleSubtrackVector Exec(const LineTrackVector &ft12Tracks, const PndFtsCircleHitSet &hitSet) const;

 private:
  [[nodiscard]] CircleSubtrack ExecItem(const LineTrack &ft12Track, const PndFtsCircleHitSet &hits) const;

  const PndFtsContext &fContext;
};

} // namespace PndFtsTrackFinder
