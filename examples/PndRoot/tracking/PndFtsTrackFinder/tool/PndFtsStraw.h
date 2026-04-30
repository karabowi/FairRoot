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
// Created by Bartosz Sobol on 22.04.2020.
//

#pragma once

#include "PndFtsStation.h"

namespace PndFtsTrackFinder {

/**
 * @class Straw
 * Struct representing a single FT straw.
 * Enclosed in pragma pack() - required for fast ROOT I/O
 */
#pragma pack(push, 1)
struct PndFtsStraw {
  uint16_t fId;           //!< ID (number) of the straw, in range [0, 12223]
  uint8_t fLayerId;       //!< ID (number) of the layer containing the straw, in range [1,48]
  float fX, fY, fZ;       //!< Coordinates of the straw in PANDA's coordinate system
  float fSlope;           //!< Slope of the straw in degrees one of {0, 11.43, -11.43}
  PndFtsStation fStation; //!< Station containing the straw
  bool fSkewed;           //!< If the straw is fSkewed (otherwise it's vertical)
  bool fOutermost;        //!< If the straw is in fOutermost layer, equal to what #IsOutermostLayer() returns
};
#pragma pack(pop)

} // namespace PndFtsTrackFinder