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
// Created by Bartosz Sobol on 25.04.2020.
//

#pragma once

#include <cstdint>

namespace PndFtsTrackFinder {

/**
 * @enum FtsStation
 * Enum representing main 3 (FT12/34/56) tracking stations of PANDA's Forward Tracker.
 */
enum class PndFtsStation : uint8_t { FT12 = 0, FT34 = 1, FT56 = 2 };

} // namespace PndFtsTrackFinder
