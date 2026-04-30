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

#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>

static class PndFTSCANoDebugStream {
 public:
  inline PndFTSCANoDebugStream &operator<<(std::ostream &(*)(std::ostream &)) { return *this; }

  template <typename T>
  inline PndFTSCANoDebugStream &operator<<(const T &)
  {
    return *this;
  }
} PndFTSCANoDebugStreamInst;

#ifdef DEBUG_MESSAGES
template <typename T>
inline void prepareForDebugOutput(const typename T::Mask k, T *x0, T *x1 = 0, T *x2 = 0, T *x3 = 0, T *x4 = 0, T *x5 = 0, T *x6 = 0, T *x7 = 0, T *x8 = 0, T *x9 = 0)
{
  if (x0)
    (*x0)(k) = T(0);
  if (x1)
    (*x1)(k) = T(0);
  if (x2)
    (*x2)(k) = T(0);
  if (x3)
    (*x3)(k) = T(0);
  if (x4)
    (*x4)(k) = T(0);
  if (x5)
    (*x5)(k) = T(0);
  if (x6)
    (*x6)(k) = T(0);
  if (x7)
    (*x7)(k) = T(0);
  if (x8)
    (*x8)(k) = T(0);
  if (x9)
    (*x9)(k) = T(0);
}
#if DEBUG_MESSAGES & 1
inline std::ostream &debugS()
{
  return std::cerr;
}
#else
inline PndFTSCANoDebugStream &debugS()
{
  return PndFTSCANoDebugStreamInst;
}
#endif
#if DEBUG_MESSAGES & 2
inline std::ostream &debugF()
{
  return std::cerr;
}
#else
inline PndFTSCANoDebugStream &debugF()
{
  return PndFTSCANoDebugStreamInst;
}
#endif
#if DEBUG_MESSAGES & 4
inline std::ostream &debugKF()
{
  return std::cerr;
}
#else
inline PndFTSCANoDebugStream &debugKF()
{
  return PndFTSCANoDebugStreamInst;
}
#endif
#if DEBUG_MESSAGES & 8
inline std::ostream &debugTS()
{
  return std::cerr;
}
#else
inline PndFTSCANoDebugStream &debugTS()
{
  return PndFTSCANoDebugStreamInst;
}
#endif
#if DEBUG_MESSAGES & 16
inline std::ostream &debugWO()
{
  return std::cerr;
}
#else
inline PndFTSCANoDebugStream &debugWO()
{
  return PndFTSCANoDebugStreamInst;
}
#endif
#else
template <typename T>
inline void prepareForDebugOutput(const typename T::Mask, T *, T * = 0, T * = 0, T * = 0, T * = 0, T * = 0, T * = 0, T * = 0, T * = 0, T * = 0)
{
}
inline PndFTSCANoDebugStream &debugS()
{
  return PndFTSCANoDebugStreamInst;
}
inline PndFTSCANoDebugStream &debugF()
{
  return PndFTSCANoDebugStreamInst;
}
inline PndFTSCANoDebugStream &debugKF()
{
  return PndFTSCANoDebugStreamInst;
}
inline PndFTSCANoDebugStream &debugTS()
{
  return PndFTSCANoDebugStreamInst;
}
inline PndFTSCANoDebugStream &debugWO()
{
  return PndFTSCANoDebugStreamInst;
}
#endif

#endif // DEBUG_H
