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

#include "PndFtsReconRunner.h"

namespace PndFtsTrackFinder {

[[nodiscard]] FullTrackVector PndFtsReconRunner::Exec(const SimpleHitVector &inHits)
{
  if (inHits.empty()) {
    LOG(info) << "PndFtsTrackReconRunner::Exec: "
              << "Empty inHits. Returning empty track set. \n";
    return {};
  }

  fFT12Hits.clear();
  fFT34Hits.clear();
  fFT56Hits.clear();

  for (const auto hit : inHits) {
    switch (fContext.fGeom.Straw(hit).fStation) {
    case PndFtsStation::FT12: fFT12Hits.emplace_back(hit); break;
    case PndFtsStation::FT34: fFT34Hits.emplace_back(hit); break;
    case PndFtsStation::FT56: fFT56Hits.emplace_back(hit); break;
    default: break;
    }
  }

  const auto ft12LineTracks = fReconLine.Exec(fFT12Hits);
  const auto ft56LineTracks = fReconLine.Exec(fFT56Hits);

  const auto ft34CircleTracks = fReconCircle.Exec(ft12LineTracks, fFT34Hits);

  return fReconMerge.Exec(ft12LineTracks, ft34CircleTracks, ft56LineTracks);
}

} // namespace PndFtsTrackFinder
