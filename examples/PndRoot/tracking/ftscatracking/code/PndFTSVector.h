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

/**************************************************************************
 * This file is property of and copyright by the ALICE HLT Project        *
 * All rights reserved.                                                   *
 *                                                                        *
 * Primary Authors:                                                       *
 *     Copyright 2009       Matthias Kretz <kretz@kde.org>                *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

#ifndef PNDFTSVECTOR_H
#define PNDFTSVECTOR_H

#include "PndFTSCADef.h"
#include "PndFTSCAMath.h"

static inline uint_m validHitIndexes(const uint_v &v)
{
  return static_cast<int_v>(v) >= int_v(Vc::Zero);
}

namespace CAMath {
template <>
inline Vc::int_v Abs<Vc::int_v>(const Vc::int_v &x)
{
  return Vc::abs(x);
}
#define SPECIALIZATION(T)                                 \
  template <>                                             \
  inline Vc::T Min<Vc::T>(const Vc::T &x, const Vc::T &y) \
  {                                                       \
    return Vc::min(x, y);                                 \
  }                                                       \
  template <>                                             \
  inline Vc::T Max<Vc::T>(const Vc::T &x, const Vc::T &y) \
  {                                                       \
    return Vc::max(x, y);                                 \
  }
SPECIALIZATION(int_v)
SPECIALIZATION(uint_v)
#undef SPECIALIZATION
#define SPECIALIZATION(T)                                   \
  template <>                                               \
  inline T Min<T>(const T &x, const T &y)                   \
  {                                                         \
    return Vc::min(x, y);                                   \
  }                                                         \
  template <>                                               \
  inline T Max<T>(const T &x, const T &y)                   \
  {                                                         \
    return Vc::max(x, y);                                   \
  }                                                         \
  template <>                                               \
  inline T Sqrt<T>(const T &x)                              \
  {                                                         \
    return Vc::sqrt(x);                                     \
  }                                                         \
  template <>                                               \
  inline T Abs<T>(const T &x)                               \
  {                                                         \
    return Vc::abs(x);                                      \
  }                                                         \
  template <>                                               \
  inline T Log<T>(const T &x)                               \
  {                                                         \
    return Vc::log(x);                                      \
  }                                                         \
  template <>                                               \
  inline T Reciprocal<T>(const T &x)                        \
  {                                                         \
    return Vc::reciprocal(x);                               \
  }                                                         \
  template <>                                               \
  inline T Round<T>(const T &x)                             \
  {                                                         \
    return Vc::round(x);                                    \
  }                                                         \
  template <>                                               \
  inline T RSqrt<T>(const T &x)                             \
  {                                                         \
    return Vc::rsqrt(x);                                    \
  }                                                         \
  template <>                                               \
  struct FiniteReturnTypeHelper<T> {                        \
    typedef T::Mask R;                                      \
  };                                                        \
  template <>                                               \
  inline FiniteReturnTypeHelper<T>::R Finite<T>(const T &x) \
  {                                                         \
    return Vc::isfinite(x);                                 \
  }                                                         \
  template <>                                               \
  inline T ATan2<T>(const T &x, const T &y)                 \
  {                                                         \
    return Vc::atan2(x, y);                                 \
  }                                                         \
  template <>                                               \
  inline T ASin<T>(const T &x)                              \
  {                                                         \
    return Vc::asin(x);                                     \
  }                                                         \
  template <>                                               \
  inline T Sin<T>(const T &x)                               \
  {                                                         \
    return Vc::sin(x);                                      \
  }                                                         \
  template <>                                               \
  inline T Cos<T>(const T &x)                               \
  {                                                         \
    return Vc::cos(x);                                      \
  }

SPECIALIZATION(float_v)
SPECIALIZATION(double_v)
#undef SPECIALIZATION

#ifdef USE_TBB
static void AtomicMax(unsigned int volatile *addr, uint_v val)
{
  for (int i = 0; i < uint_v::Size; ++i) {
    AtomicMax(&addr[i], val[i]);
  }
}
#endif // USE_TBB
} // namespace CAMath

#endif // PNDFTSVECTOR_H
