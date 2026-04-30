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
// Created by Bartosz Sobol on 25.12.2020.
//

#pragma once

#include "PndFtsReconCircleZOX.h"
#include "PndFtsReconCircleZOY.h"

namespace PndFtsTrackFinder {

class PndFtsReconCircle {
 public:
  PndFtsReconCircle() = delete;

  explicit PndFtsReconCircle(const PndFtsContext &context) : fReconZOX{context}, fReconZOY{context}, fContext{context} {}

  PndFtsReconCircle(const PndFtsContext &&context) = delete;

  [[nodiscard]] CircleTrackVector Exec(const LineTrackVector &ft12Tracks, const SimpleHitVector &inHits) const;

 private:
  const PndFtsReconCircleZOX fReconZOX;
  const PndFtsReconCircleZOY fReconZOY;

  const PndFtsContext &fContext;
};

} // namespace PndFtsTrackFinder
