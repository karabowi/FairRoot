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
// Created by Bartosz Sobol on 15.06.2020.
//

#pragma once

#include <utility>

#include "PndFtsGeom.h"
#include "PndFtsConstants.h"

namespace PndFtsTrackFinder {

enum class PndFtsMomEstMethod {
  CURVATURE, //!< Take curvature radius straight form FT34 ZOX circle fitting.
  TANGENT    //!< Reconstruct curvature radius from FT12 and FT56 ZOX lines if possible. Usually more accurate.
};

class PndFtsContext {
 public:
  explicit PndFtsContext(PndFtsGeom geom, int debug = 0, int verbose = 0, const PndFtsMomEstMethod momEstMethod = PndFtsMomEstMethod::CURVATURE, PndFtsConstants constants = {})
    : fGeom{std::move(geom)}, fDebug{debug}, fVerbose{verbose}, fConstants{constants}, fMomEstMethod{momEstMethod}
  {
  }

  const PndFtsGeom fGeom;
  const int fDebug;
  const int fVerbose;
  const PndFtsConstants fConstants;
  const PndFtsMomEstMethod fMomEstMethod;
};

} // namespace PndFtsTrackFinder
