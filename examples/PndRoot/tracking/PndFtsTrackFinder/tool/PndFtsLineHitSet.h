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
// Created by Bartosz Sobol on 27.08.2020.
//

#pragma once

#include <iostream>

#include "PndFtsHelpers.h"

namespace PndFtsTrackFinder {

class PndFtsLineHitSet {
 public:
  PndFtsLineHitSet() = delete;

  PndFtsLineHitSet(const SimpleHitVector &hits, const PndFtsGeom &geom);

  template <class ItT>
  PndFtsLineHitSet(ItT begin, ItT end, const PndFtsGeom &geom)
  {
    Reserve(std::distance(begin, end));

    std::for_each(begin, end, [this, &geom](const auto hit) { Add(hit, geom); });
  }

  [[nodiscard]] bool IsComplete() const noexcept;

  SimpleHitVector fVertical{};
  SimpleHitVector fSkewed{};
  SimpleHitVector fOutermostVertical{};
  SimpleHitVector fOutermostSkewed{};

 private:
  void Reserve(const std::size_t size);

  void Add(const PndFtsSimpleHit &hit, const PndFtsGeom &geom);
};

} // namespace PndFtsTrackFinder