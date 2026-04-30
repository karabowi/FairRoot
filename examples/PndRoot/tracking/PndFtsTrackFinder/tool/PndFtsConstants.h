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
// Created by Bartosz Sobol on 13.09.2020.
//

#pragma once

#include <cstdint>

namespace PndFtsTrackFinder {

/**
 * @class Constants
 * Data struct containing program and algorithm constants.
 * Description of not self-describing or not documented members can be found in PANDA's FT Technical Design Report.
 */
struct PndFtsConstants {
  float fDBias = 0.5;
  float fDeltaL = 0.5;
  float fDeltaC = 2.0;
  float fDelta2 = 40.0;
  float fZBStart = 368.0;
  float fZBEnd = 560.0;
  float fZBCurvOffset = 1.5; //!< Offset for fZBStart in FT34 ZOX circle fitting.
  float fZBTanOffset = 6.0;  //!< Offset fot fZBStart and fZBEnd for PndFtsMomEstMethod::TANGENT-based momentum estimation.
};

} // namespace PndFtsTrackFinder
