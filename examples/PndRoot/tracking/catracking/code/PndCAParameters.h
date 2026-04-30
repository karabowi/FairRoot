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

#ifndef PNDCAPARAMETERS_H
#define PNDCAPARAMETERS_H

#include "PndCAVector.h"

struct PndCAParameters { /// Global parameters
  enum {
    MaxNStations = 30,
    NMVDStations = 4,
    MinimumHitsForRecoTrack = 3,
    MaxCellLength = 6,
    //    LastCellLength = 4 // length of rightmost cell
    LastCellLength = 3
  };

  // Transform CALocal coordinates in global x y z coordinates and back
  // CALocal coor are: X0 is normal to the station module, X2 is paralel to magnetic field. X0 X1 X2 are right handed coordinates
  template <typename T>
  static void CALocalToGlobal(T x0, T x1, T angle, T &x, T &y);
  template <typename T>
  static void GlobalToCALocal(T x, T y, T angle, T &x0, T &x1);
  template <typename T>
  static void CALocalToGlobal(T x0, T x1, T x2, T angle, T &x, T &y, T &z);
  template <typename T>
  static void GlobalToCALocal(T x, T y, T z, T angle, T &x0, T &x1, T &x2);

}; // namespace PndCAParameters
// namespace Parameters{  using namespace PndCAParameters; }

/*
template< typename T >
void PndCAParameters::CALocalToGlobal( T x0, T x1, T angle, T& x, T& y ) {
  const T cA = CAMath::Cos( angle );
  const T sA = CAMath::Sin( angle );
  x =  x0*sA + x1*cA;
  y =    x0*cA - x1*sA;
}

template< typename T >
void PndCAParameters::CALocalToGlobal( T x0, T x1, T x2, T angle, T& x, T& y, T &z ) {
  CALocalToGlobal<T>(x0,x1,angle,x,y);
  z =  - x2;
}
*/
template <typename T>
void PndCAParameters::GlobalToCALocal(T x, T y, T angle, T &x0, T &x1)
{
  const T cA = CAMath::Cos(angle);
  const T sA = CAMath::Sin(angle);
  // SG!!!
  // x0 = x*cA + y*sA;
  // x1 =  x*cA - y*cA;
  x0 = x * sA + y * cA;
  x1 = x * cA - y * sA;
}

template <typename T>
void PndCAParameters::GlobalToCALocal(T x, T y, T z, T angle, T &x0, T &x1, T &x2)
{
  GlobalToCALocal<T>(x, y, angle, x0, x1);
  x2 = -z;
}

namespace PndCAPParameters { /// parameters for global Performance
/**
 *   MC track is reconstructable if:
 *   - it have at least MinimumHitsForMCTrack hits
 *   - momentum of it is not lower than PndCAParameters::ExtraThreshold
 */

enum {

  /**
   * The minimum number of hits for a MCtrack to be reconstructable
   */
  MinimumHitsForMCTrack = 6, // PndCAParameters::MinimumHitsForRecoTrack,

  /**
   * The minimum number of hits for a MCtrack to be reconstructable
   */
  MinimumMCPointsForMCTrack = MinimumHitsForMCTrack, // PndCAParameters::MinimumHitsForRecoTrack,

  /**
   * MCTrack is reconstructed if correspondent recoTrack has NHits >= MinimumHitsForRecoTrack
   */
  MinimumHitsForRecoTrack = PndCAParameters::MinimumHitsForRecoTrack
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
} //  namespace PndCAPParameters
namespace PParameters {
using namespace PndCAPParameters;
}

#endif
