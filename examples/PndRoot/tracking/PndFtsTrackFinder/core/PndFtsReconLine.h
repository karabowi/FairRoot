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
// Created by Bartosz Sobol on 13.05.2020.
//

#pragma once

#include "PndFtsReconLineZOX.h"
#include "PndFtsReconLineZOY.h"

namespace PndFtsTrackFinder {

class PndFtsReconLine {
 public:
  PndFtsReconLine() = delete;

  explicit PndFtsReconLine(const PndFtsContext &context) : fReconZOX{context}, fReconZOY{context}, fContext{context} {};

  PndFtsReconLine(const PndFtsContext &&context) = delete;

  [[nodiscard]] LineTrackVector Exec(const SimpleHitVector &lhsTrack) const;

 private:
  const PndFtsReconLineZOX fReconZOX;
  const PndFtsReconLineZOY fReconZOY;

  const PndFtsContext &fContext;
};

} // namespace PndFtsTrackFinder