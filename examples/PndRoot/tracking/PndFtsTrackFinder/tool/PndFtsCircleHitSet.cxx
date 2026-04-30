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
// Created by Bartosz Sobol on 11.11.2021.
//

#include "PndFtsCircleHitSet.h"

namespace PndFtsTrackFinder {

PndFtsCircleHitSet::PndFtsCircleHitSet(const SimpleHitVector &hits, const PndFtsGeom &geom)
{
  for (const auto &hit : hits) {
    Add(hit, geom);
  }
}

bool PndFtsCircleHitSet::IsComplete() const noexcept
{
  return !(fVertical.empty() || fSkewed.empty() || fLayer3132.empty());
}

void PndFtsCircleHitSet::Reserve(const std::size_t size)
{
  fSkewed.reserve(size);
  fVertical.reserve(size);
  fLayer3132.reserve(size / 8);
}

void PndFtsCircleHitSet::Add(const PndFtsSimpleHit &hit, const PndFtsGeom &geom)
{
  const auto straw = geom.Straw(hit);

  if (straw.fSkewed) {
    fSkewed.emplace_back(hit);
  } else {
    fVertical.emplace_back(hit);
    if (straw.fLayerId == 31 || straw.fLayerId == 32) {
      fLayer3132.emplace_back(hit);
    }
  }
}

} // namespace PndFtsTrackFinder
