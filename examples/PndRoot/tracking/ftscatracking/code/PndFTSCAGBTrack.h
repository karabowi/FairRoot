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
// $Id: AliHLTTPCCAGBTrack.h,v 1.2 2016/11/22 17:23:46 mpugach Exp $
// ************************************************************************
// This file is property of and copyright by the ALICE HLT Project        *
// ALICE Experiment at CERN, All rights reserved.                         *
// See cxx source for full Copyright notice                               *
//                                                                        *
//*************************************************************************

#ifndef PNDFTSCAGBTRACK_H
#define PNDFTSCAGBTRACK_H

#include "PndFTSCADef.h"
#include "PndFTSCATrackParam.h"

#include <ostream>
#include <istream>

// namespace std
//{
//  template<typename T> struct char_traits;
//  template<typename _CharT, typename _Traits> class basic_istream;
//  typedef basic_istream<char, char_traits<char> > std::istream;
//  template<typename _CharT, typename _Traits> class basic_ostream;
//  typedef basic_ostream<char, char_traits<char> > std::ostream;
//} // namespace std

/**
 * @class PndFTSCAGBTrack
 *
 *The PndFTSCAGBTrack class is the internal representation of track's parameters
 */
class PndFTSCAGBTrack {
  friend std::istream &operator>>(std::istream &, PndFTSCAGBTrack &);
  friend std::ostream &operator<<(std::ostream &, const PndFTSCAGBTrack &);

 public:
  PndFTSCAGBTrack() : fFirstHitRef(0), fNHits(0), fInnerParam(), fOuterParam(), fDeDx(0) {}

  int NHits() const { return fNHits; }
  int FirstHitRef() const { return fFirstHitRef; }
  const PndFTSCATrackParam &Param() const { return InnerParam(); }
  const PndFTSCATrackParam &InnerParam() const { return fInnerParam; }
  const PndFTSCATrackParam &OuterParam() const { return fOuterParam; }
  float DeDx() const { return fDeDx; }

  void SetNHits(int v) { fNHits = v; }
  void SetFirstHitRef(int v) { fFirstHitRef = v; }
  void SetInnerParam(const PndFTSCATrackParam &v) { fInnerParam = v; }
  void SetOuterParam(const PndFTSCATrackParam &v) { fOuterParam = v; }
  void SetDeDx(float v) { fDeDx = v; }

  static bool ComparePNClusters(const PndFTSCAGBTrack *a, const PndFTSCAGBTrack *b) { return (a->fNHits > b->fNHits); }

 protected:
  int fFirstHitRef;               // index of the first hit reference in track->hit reference array
  int fNHits;                     // number of track hits
  PndFTSCATrackParam fInnerParam; // fitted track parameters
  PndFTSCATrackParam fOuterParam;
  float fDeDx; //* DE/DX
};

std::istream &operator>>(std::istream &, PndFTSCAGBTrack &);
std::ostream &operator<<(std::ostream &, const PndFTSCAGBTrack &);

#endif
