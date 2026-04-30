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

#pragma once

#include "tool/PndFtsMath.h"
#include "tool/PndFtsTrack.h"
#include "tool/PndFtsContext.h"
#include <optional>

namespace PndFtsTrackFinder {

class PndFtsReconMerge {
 public:
  PndFtsReconMerge() = delete;

  explicit PndFtsReconMerge(const PndFtsContext &context);

  PndFtsReconMerge(const PndFtsContext &&context) = delete;

  [[nodiscard]] FullTrackVector Exec(const LineTrackVector &ft12Tracks, const CircleTrackVector &ft34Tracks, const LineTrackVector &ft56Tracks) const;

 private:
  void ExecItem(const LineTrackVector &ft12Tracks, const LineTrackVector &ft56Tracks, const CircleTrackVector &ft34Tracks, const std::vector<TrackRange> &track56Ranges,
                const std::vector<TrackRange> &track1234Ranges, std::vector<std::optional<std::size_t>> &foundMatches) const;

  const PndFtsContext &fContext;
};

} // namespace PndFtsTrackFinder
