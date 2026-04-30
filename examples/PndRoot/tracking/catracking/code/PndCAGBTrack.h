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

//-*- Mode: C++ -*-
// $Id: PndCAGBTrack.h,v 1.2 2010/08/02 16:45:28 ikulakov Exp $
// ************************************************************************
// This file is property of and copyright by the ALICE HLT Project        *
// ALICE Experiment at CERN, All rights reserved.                         *
// See cxx source for full Copyright notice                               *
//                                                                        *
//*************************************************************************

#ifndef PNDCAGBTRACK_H
#define PNDCAGBTRACK_H

#include "PndCADef.h"
#include "PndCATrackParam.h"

/**
 * @class PndCAGBTrack
 *
 *
 */
class PndCAGBTrack {
  //     friend std::istream &operator>>( std::istream &, PndCAGBTrack & );
  //     friend std::ostream &operator<<( std::ostream &, const PndCAGBTrack & );
 public:
  PndCAGBTrack() : fFirstHitRef(0), fNHits(0), fInnerParam(), fOuterParam(), fDeDx(0) {}

  int NHits() const { return fNHits; }
  int FirstHitRef() const { return fFirstHitRef; }
  const PndCATrackParam &Param() const { return InnerParam(); }
  const PndCATrackParam &InnerParam() const { return fInnerParam; }
  const PndCATrackParam &OuterParam() const { return fOuterParam; }
  float DeDx() const { return fDeDx; }

  void SetNHits(int v) { fNHits = v; }
  void SetFirstHitRef(int v) { fFirstHitRef = v; }
  void SetInnerParam(const PndCATrackParam &v) { fInnerParam = v; }
  void SetOuterParam(const PndCATrackParam &v) { fOuterParam = v; }
  void SetDeDx(float v) { fDeDx = v; }

  static bool ComparePNClusters(const PndCAGBTrack *a, const PndCAGBTrack *b) { return (a->fNHits > b->fNHits); }

 protected:
  int fFirstHitRef;            // index of the first hit reference in track->hit reference array
  int fNHits;                  // number of track hits
  PndCATrackParam fInnerParam; // fitted track parameters
  PndCATrackParam fOuterParam;
  float fDeDx; //* DE/DX
};

// std::istream &operator>>( std::istream &, PndCAGBTrack & );
// std::ostream &operator<<( std::ostream &, const PndCAGBTrack & );

#endif
