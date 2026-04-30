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

#ifndef PNDCASTATIONSTT_H
#define PNDCASTATIONSTT_H

#include "PndCADef.h"

#include <vector>
using std::vector;

class PndCAHitSTT {
 public:
  char fISta, fISec;
  int fOrigID;
  float fU, fDR;

  friend bool operator<(const PndCAHitSTT &a, const PndCAHitSTT &b) { return (a.fISec < b.fISec) || ((a.fISec == b.fISec) && (a.fU < b.fU)); }
};

class PndCAStationSTTSector {
 public:
  void Init()
  {
    fNHits = 0;
    fFirstHit = 0;
  }
  int fNHits;
  int fFirstHit;
};

class PndCAStationSTT {
 public:
  void Init()
  {
    fResolution = float_v(35697.3e-8);
    fHits1D.clear();
    fHits1D.reserve(100);
    for (int i = 0; i < fgNSectors; i++)
      fSectors[i].Init();
  }
  static const int fgNSectors = 6;
  float_v fResolution;
  float_v fSin, fCos;
  PndCAStationSTTSector fSectors[fgNSectors];
  vector<PndCAHitSTT> fHits1D;
};

#endif
