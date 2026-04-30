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

#ifndef PNDFTSCAPARAMETERS_H
#define PNDFTSCAPARAMETERS_H

#include "PndFTSVector.h"

struct PndFTSCAParameters { /// Global parameters
  enum {
#ifdef STAR_HFT
    MaxNStations = 4,            // max number of stations
    MinimumHitsForRecoTrack = 4, // 3
    MaxCellLength = 3,           // length of triplet
    LastCellLength = 3           // length of rightmost cell
#elif ALICE_ITS
    MaxNStations = 8,
    MinimumHitsForRecoTrack = 4, // 3
    MaxCellLength = 3,
    LastCellLength = 3 // length of rightmost cell
#elif PANDA_STT
    MaxNStations = 30,
    NMVDStations = 4,
    MinimumHitsForRecoTrack = 3,
    MaxCellLength = 6,
    LastCellLength = 4 // length of rightmost cell
#elif PANDA_FTS
    MaxNStations = 48,
    NMVDStations = 0,
    MinimumHitsForRecoTrack = 6,
    MaxCellLength = 6,
    LastCellLength = 6 // length of rightmost cell
#endif
  };

  // Transform CALocal coordinates in global x y z coordinates and back
  // CALocal coor are: X0 is normal to the station module, X2 is paralel to magnetic field. X0 X1 X2 are right handed coordinates
#ifndef PANDA_FTS
  template <typename T>
  static void CALocalToGlobal(T x0, T x1, T angle, T &x, T &y);
  template <typename T>
  static void GlobalToCALocal(T x, T y, T angle, T &x0, T &x1);
#endif
  template <typename T>
  static void CALocalToGlobal(T x0, T x1, T x2, T angle, T &x, T &y, T &z);
  template <typename T>
  static void GlobalToCALocal(T x, T y, T z, T angle, T &x0, T &x1, T &x2);

}; // namespace PndFTSCAParameters
// namespace Parameters{  using namespace PndFTSCAParameters; }

#ifdef PANDA_STT
// Panda global coordinates: watching downstream: x looks to the left, y - up, z - front.
// Panda STT local coordinates: watching downstream: X0 is "radial" - normal to the layer, looks from beam, X1 - left, X2 - back.
template <typename T>
void PndFTSCAParameters::CALocalToGlobal(T x0, T x1, T angle, T &x, T &y)
{
  const T cA = CAMath::Cos(angle);
  const T sA = CAMath::Sin(angle);
  x = x0 * sA + x1 * cA;
  y = x0 * cA - x1 * sA;
}

template <typename T>
void PndFTSCAParameters::CALocalToGlobal(T x0, T x1, T x2, T angle, T &x, T &y, T &z)
{
  CALocalToGlobal<T>(x0, x1, angle, x, y);
  z = -x2;
}

template <typename T>
void PndFTSCAParameters::GlobalToCALocal(T x, T y, T angle, T &x0, T &x1)
{
  const T cA = CAMath::Cos(angle);
  const T sA = CAMath::Sin(angle);
  x0 = x * sA + y * cA;
  x1 = x * cA - y * sA;
}

template <typename T>
void PndFTSCAParameters::GlobalToCALocal(T x, T y, T z, T angle, T &x0, T &x1, T &x2)
{
  GlobalToCALocal<T>(x, y, angle, x0, x1);
  x2 = -z;
}
#elif PANDA_FTS
// x0 along the beam (normal to planes)
// x1 to the right if look downstream
// x2 vertical (along the mag.field)
template <typename T>
void PndFTSCAParameters::CALocalToGlobal(T x0, T x1, T x2, T angle, T &x, T &y, T &z)
{
  UNUSED_PARAM1(angle);
  x = x1;
  y = x2;
  z = x0;
}
template <typename T>
void PndFTSCAParameters::GlobalToCALocal(T x, T y, T z, T angle, T &x0, T &x1, T &x2)
{
  UNUSED_PARAM1(angle);
  x0 = z;
  x1 = x;
  x2 = y;
}
#else
template <typename T>
void PndFTSCAParameters::CALocalToGlobal(T x0, T x1, T angle, T &x, T &y)
{
  const T cA = CAMath::Cos(angle);
  const T sA = CAMath::Sin(angle);
  x = x0 * cA - x1 * sA;
  y = x0 * sA + x1 * cA;
}

template <typename T>
void PndFTSCAParameters::CALocalToGlobal(T x0, T x1, T x2, T angle, T &x, T &y, T &z)
{
  CALocalToGlobal<T>(x0, x1, angle, x, y);
  z = x2;
}

template <typename T>
void PndFTSCAParameters::GlobalToCALocal(T x, T y, T angle, T &x0, T &x1)
{
  const T cA = CAMath::Cos(angle);
  const T sA = CAMath::Sin(angle);
  x0 = x * cA + y * sA;
  x1 = -x * sA + y * cA;
}

template <typename T>
void PndFTSCAParameters::GlobalToCALocal(T x, T y, T z, T angle, T &x0, T &x1, T &x2)
{
  GlobalToCALocal<T>(x, y, angle, x0, x1);
  x2 = z;
}
#endif

namespace PndFTSCAPParameters { /// parameters for global Performance
/**
 *   MC track is reconstructable if:
 *   - it have at least MinimumHitsForMCTrack hits
 *   - momentum of it is not lower than PndFTSCAParameters::ExtraThreshold
 */

enum {

#ifdef PANDA_STT
  /**
   * The minimum number of hits for a MCtrack to be reconstructable
   */
  MinimumHitsForMCTrack = 6, // PndFTSCAParameters::MinimumHitsForRecoTrack,

  /**
   * The minimum number of hits for a MCtrack to be reconstructable
   */
  MinimumMCPointsForMCTrack = MinimumHitsForMCTrack, // PndFTSCAParameters::MinimumHitsForRecoTrack,
#else
  MinimumHitsForMCTrack = 4,
  MinimumMCPointsForMCTrack = 4,
#endif

  /**
   * MCTrack is reconstructed if correspondent recoTrack has NHits >= MinimumHitsForRecoTrack
   */
  MinimumHitsForRecoTrack = PndFTSCAParameters::MinimumHitsForRecoTrack
}; // enum

/**
 *  if p < ExtraThreshold => track is not reconstructable
 */
static const float ExtraThreshold = 0.05;

/**
 *  if p < RefThreshold => track is not reference track
 */
static const float RefThreshold = 1.;

/**
 *  max percent of one MCTrack MCPoints in current track should be more than this value, then thrack is not ghost.
 */
static const float MinTrackPurity = .75;

} //  namespace PndFTSCAPParameters
namespace PParameters {
using namespace PndFTSCAPParameters;
}

#endif // ALIHLTTPCCAPARAMETERS_H
