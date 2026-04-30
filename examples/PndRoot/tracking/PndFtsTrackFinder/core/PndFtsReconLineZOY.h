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

#pragma once

#include "tool/PndFtsContext.h"
#include "tool/PndFtsGeom.h"
#include "tool/PndFtsTrack.h"

namespace PndFtsTrackFinder {

class PndFtsReconLineZOY {
 public:
  PndFtsReconLineZOY() = delete;

  explicit PndFtsReconLineZOY(const PndFtsContext &context);

  PndFtsReconLineZOY(const PndFtsContext &&context) = delete;

  [[nodiscard]] LineSubtrackVector Exec(const LineSubtrackVector &zoxTracks, const PndFtsLineHitSet &hitSet) const;

 private:
  [[nodiscard]] LineSubtrack ExecItem(const LineSubtrack &zoxTrack, const PndFtsLineHitSet &hits) const;

  [[nodiscard]] Line ZOYLineTransform(const bool ft12, const Line &zoxLine, const Line &minLine) const;

  [[nodiscard]] ZYVirtualHitVectorPair CreateVirtualHits(const LineSubtrack &zoxTrack, const PndFtsLineHitSet &hits) const;

  const PndFtsContext &fContext;
};

} // namespace PndFtsTrackFinder