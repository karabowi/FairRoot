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

#pragma once

#include "PndFtsStraw.h"
#include "PndFtsHelpers.h"

namespace PndFtsTrackFinder::GeomUtil {
/**
 * Auxiliary function to check whether the given layer contains fSkewed (layer is fSkewed) or vertical straws.
 * Skewed layers are: 3, 4, 5, 6, 11, 12, 13, 14, 19, 20, 21, 22, 27, 28, 29, 30, 35, 36, 37, 38, 43, 44, 45, 46.
 * @param layerId id (number) of the layer
 * @return true if straws in the given layer are fSkewed, false if vertical
 */
bool IsSkewedLayer(const uint8_t layerId);

/**
 * Auxiliary function to check whether the given layer is fOutermost layer.
 * Outermost layers are: 1, 2, 3, 4, 13, 14, 15, 16, 17, 18, 19, 20, 29, 30, 31, 32, 33, 34, 35, 36, 45, 46, 47, 48.
 * @param layerId id (number) of the layer
 * @return true if the given layer is fOutermost, false otherwise
 */
bool IsOutermostLayer(const uint8_t layerId);

/**
 * Auxiliary function to get a slope of the given layer in degrees.
 * Possible slope values are: 0, 11.43 or -11.43 deg.
 * Layers 3, 4, 11, 12, 19, 20, 27, 28, 35, 36, 43, 44 have negative slope. Other fSkewed layers have positive slope.
 * Vertical layers have no slope.
 * @param layerId id (number) of the layer
 * @return slope of the straws in the given layer
 */
float GetSlope(const uint8_t layerId);

/**
 * Auxiliary function to get a station containing the given layer.
 * Each FtsStation contains 16 layers.
 * @param layerId id (number) of the layer
 * @return FtsStation containing the given layer
 */
PndFtsStation GetStationFromLayer(const uint8_t layerId) noexcept;

/**
 * Auxiliary function to get a station containing the given Straw.
 * @param strawId id (number) of the straw
 * @return FtsStation containing the given Straw
 */
PndFtsStation GetStationFromStraw(const uint16_t strawId) noexcept;

} // namespace PndFtsTrackFinder::GeomUtil
