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

#include "tool/PndFtsLineHitSet.h"
#include "tool/PndFtsContext.h"
#include "tool/PndFtsTrack.h"
#include "tool/PndFtsGeom.h"

namespace PndFtsTrackFinder {

class PndFtsReconLineZOX {
 public:
  PndFtsReconLineZOX() = delete;

  explicit PndFtsReconLineZOX(const PndFtsContext &context);

  PndFtsReconLineZOX(const PndFtsContext &&context) = delete;

  [[nodiscard]] LineSubtrackVector Exec(const PndFtsLineHitSet &hitSet) const;

 private:
  [[nodiscard]] LineSubtrackVector ExecItem(const PndFtsLineHitSet &hits) const;

  [[nodiscard]] TangentLineArray TangentLines(const PndFtsSimpleHit &hitBeg, const PndFtsSimpleHit &hitEnd) const;

  static void RemoveDuplicates(LineSubtrackVector &tracks);

  const PndFtsContext &fContext;
};

} // namespace PndFtsTrackFinder