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

#ifndef PNDEMCFPGAMLINFITTER_HH
#define PNDEMCFPGAMLINFITTER_HH

#include "PndEmcPSAFPGAFilterMA.h"

class PndEmcPSAFPGAMLinFitter {
 public:
  PndEmcPSAFPGAMLinFitter(){};
  ~PndEmcPSAFPGAMLinFitter(){};
  void set(int newBufferSize);
  void putPoint(double ix, double iy);
  void fit();
  double offset();
  double slope();
  double average();

 private:
  PndEmcPSAFPGAFilterMA mx;
  PndEmcPSAFPGAFilterMA mxx;
  PndEmcPSAFPGAFilterMA mxy;
  PndEmcPSAFPGAFilterMA my;
  double x;
  double xx;
  double xy;
  double y;
  double a, k;
  int Np;
};

#endif
