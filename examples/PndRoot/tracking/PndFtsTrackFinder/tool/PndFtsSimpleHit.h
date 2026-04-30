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
// Created by Bartosz Sobol on 30.04.2020.
//

#pragma once

#include <vector>
#include <cstdint>
#include <PndFtsHit.h>

#include "PndFtsSimpleHit.h"

namespace PndFtsTrackFinder {

/**
 * @class SimpleHit
 * Struct representing a simplified (raw) PndFtsSimpleHit data
 */
struct PndFtsSimpleHit {

  PndFtsSimpleHit() = default;

  explicit PndFtsSimpleHit(const PndFtsHit *pndHit, const uint16_t pndHitId)
    : fPndHitId{pndHitId}, fStrawId{static_cast<uint16_t>(pndHit->GetTubeID())}, fIsochrone{static_cast<float>(pndHit->GetIsochrone())}
  {
  }

  uint16_t fPndHitId; //!< Hit Id in Panda Event
  uint16_t fStrawId;  //!< ID (number) of the hit Straw
  float fIsochrone;   //!< Drift radius

  /**
   * Compares two SimpleHits by their straw_ids.
   * SimpleHit with smaller fStrawId is considered "less".
   * @param rhs other SimpleHit
   * @return result of the comparison
   */
  inline bool operator<(const PndFtsSimpleHit &rhs) const noexcept { return this->fStrawId < rhs.fStrawId; }
};

using SimpleHitVector = std::vector<PndFtsSimpleHit>;

} // namespace PndFtsTrackFinder