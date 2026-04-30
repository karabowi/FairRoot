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
// Created by Bartosz Sobol on 12.07.2020.
//

#pragma once

#include "FairLogger.h"

#include "PndFtsReconMerge.h"
#include "PndFtsReconCircle.h"
#include "PndFtsReconLine.h"
#include "PndFtsReconLineZOX.h"
#include "PndFtsReconCircleZOX.h"
#include "PndFtsReconLineZOY.h"
#include "PndFtsReconCircleZOY.h"

namespace PndFtsTrackFinder {

class PndFtsReconRunner {
 public:
  PndFtsReconRunner() = delete;

  explicit PndFtsReconRunner(const PndFtsContext &context) : fReconLine{context}, fReconCircle{context}, fReconMerge{context}, fContext{context} {}

  PndFtsReconRunner(const PndFtsContext &&context) = delete;

  [[nodiscard]] FullTrackVector Exec(const SimpleHitVector &inHits);

 private:
  const PndFtsReconLine fReconLine;
  const PndFtsReconCircle fReconCircle;
  const PndFtsReconMerge fReconMerge;

  SimpleHitVector fFT12Hits;
  SimpleHitVector fFT34Hits;
  SimpleHitVector fFT56Hits;

  const PndFtsContext &fContext;
};

} // namespace PndFtsTrackFinder
