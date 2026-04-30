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
// Created by Bartosz Sobol on 24.03.2021.
//

#pragma once

#include <iostream>

#include "PndFtsHelpers.h"

namespace PndFtsTrackFinder {

class PndFtsCircleHitSet {
 public:
  PndFtsCircleHitSet() = delete;

  PndFtsCircleHitSet(const SimpleHitVector &hits, const PndFtsGeom &geom);

  template <class ItT>
  PndFtsCircleHitSet(ItT begin, ItT end, const PndFtsGeom &geom)
  {
    Reserve(std::distance(begin, end));

    std::for_each(begin, end, [this, &geom](const auto hit) { Add(hit, geom); });
  }

  [[nodiscard]] bool IsComplete() const noexcept;

  SimpleHitVector fVertical{};
  SimpleHitVector fSkewed{};
  SimpleHitVector fLayer3132{};

 private:
  void Reserve(const std::size_t size);

  void Add(const PndFtsSimpleHit &hit, const PndFtsGeom &geom);
};

} // namespace PndFtsTrackFinder