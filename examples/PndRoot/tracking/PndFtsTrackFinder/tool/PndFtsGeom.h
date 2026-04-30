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
// Created by Bartosz Sobol on 03.05.2020.
//

#pragma once

#include <array>

#include "PndFtsStraw.h"
#include "PndFtsSimpleHit.h"

namespace PndFtsTrackFinder {

constexpr uint16_t GEOM_STRAWS_ARRAY_SIZE = 12224;
constexpr uint16_t GEOM_STRAWS_LAYERS_Z_SIZE = 48;

using GeomStrawsArrayT = std::array<PndFtsStraw, GEOM_STRAWS_ARRAY_SIZE>;
using GeomLayersZArrayT = std::array<float, GEOM_STRAWS_LAYERS_Z_SIZE>;

struct PndFtsGeom {

  constexpr inline PndFtsGeom(GeomStrawsArrayT straws, GeomLayersZArrayT layersZ) noexcept : fStraws{straws}, fLayersZ{layersZ} {}

  [[nodiscard]] constexpr inline PndFtsStraw Straw(const uint16_t strawId) const noexcept { return fStraws[strawId - 1]; }

  [[nodiscard]] constexpr inline PndFtsStraw Straw(const PndFtsSimpleHit &hit) const noexcept { return fStraws[hit.fStrawId - 1]; }

  [[nodiscard]] constexpr inline float LayerZ(const uint16_t layerId) const noexcept { return fLayersZ[layerId - 1]; }

  const GeomStrawsArrayT fStraws;
  const GeomLayersZArrayT fLayersZ;
};

} // namespace PndFtsTrackFinder
