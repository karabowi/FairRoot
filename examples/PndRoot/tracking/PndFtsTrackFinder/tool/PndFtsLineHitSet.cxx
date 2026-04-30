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

#include "PndFtsLineHitSet.h"

namespace PndFtsTrackFinder {

PndFtsLineHitSet::PndFtsLineHitSet(const SimpleHitVector &hits, const PndFtsGeom &geom)
{
  for (const auto &hit : hits) {
    Add(hit, geom);
  }
}

bool PndFtsLineHitSet::IsComplete() const noexcept
{
  return !(fVertical.empty() or fSkewed.empty() or fOutermostSkewed.empty() or fOutermostVertical.empty());
}

void PndFtsLineHitSet::Reserve(const std::size_t size)
{
  fSkewed.reserve(size);
  fVertical.reserve(size);
  fOutermostSkewed.reserve(size / 2);
  fOutermostVertical.reserve(size / 2);
}

void PndFtsLineHitSet::Add(const PndFtsSimpleHit &hit, const PndFtsGeom &geom)
{
  const auto straw = geom.Straw(hit);

  if (straw.fSkewed) {
    fSkewed.emplace_back(hit);
    if (straw.fOutermost) {
      fOutermostSkewed.emplace_back(hit);
    }
  } else {
    fVertical.emplace_back(hit);
    if (straw.fOutermost) {
      fOutermostVertical.emplace_back(hit);
    }
  }
}

} // namespace PndFtsTrackFinder
