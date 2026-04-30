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
// Created by Bartosz Sobol on 23.04.2020.
//

#include <array>
#include <algorithm>
#include <limits>

#include "PndFtsGeomUtil.h"

namespace PndFtsTrackFinder::GeomUtil {

bool IsSkewedLayer(const uint8_t layerId)
{
  constexpr std::array<uint8_t, 24> skewedLayers{3, 4, 5, 6, 11, 12, 13, 14, 19, 20, 21, 22, 27, 28, 29, 30, 35, 36, 37, 38, 43, 44, 45, 46};
  return std::find(skewedLayers.cbegin(), skewedLayers.cend(), layerId) != skewedLayers.cend();
}

bool IsOutermostLayer(const uint8_t layerId)
{
  constexpr std::array<uint8_t, 24> outermostLayers{1, 2, 3, 4, 13, 14, 15, 16, 17, 18, 19, 20, 29, 30, 31, 32, 33, 34, 35, 36, 45, 46, 47, 48};
  return std::find(outermostLayers.cbegin(), outermostLayers.cend(), layerId) != outermostLayers.cend();
}

float GetSlope(const uint8_t layerId)
{
  // INFO tan(85 deg) = 11.43
  constexpr float strawSlopeAbs = 11.43f;
  constexpr std::array<uint8_t, 12> negativelySloped{3, 4, 11, 12, 19, 20, 27, 28, 35, 36, 43, 44};

  if (!IsSkewedLayer(layerId)) {
    return std::numeric_limits<float>::infinity();
  }
  if (std::find(negativelySloped.cbegin(), negativelySloped.cend(), layerId) != negativelySloped.cend()) {
    return -strawSlopeAbs;
  }
  return strawSlopeAbs;
}

PndFtsStation GetStationFromLayer(const uint8_t layerId) noexcept
{
  if (layerId < 17) {
    return PndFtsStation::FT12;
  }
  if (layerId < 33) {
    return PndFtsStation::FT34;
  }
  return PndFtsStation::FT56;
}

PndFtsStation GetStationFromStraw(const uint16_t strawId) noexcept
{
  if (strawId < 2305) {
    return PndFtsStation::FT12;
  }
  if (strawId < 5633) {
    return PndFtsStation::FT34;
  }
  return PndFtsStation::FT56;
}

} // namespace PndFtsTrackFinder::GeomUtil