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

#ifndef PNDEMCFPGALINFITTER_HH
#define PNDEMCFPGALINFITTER_HH

class PndEmcPSAFPGALinFitter {
 public:
  PndEmcPSAFPGALinFitter();
  ~PndEmcPSAFPGALinFitter();
  void reset();
  void putPoint(double ix, double iy);
  void fit();
  double offset();
  double slope();
  double average();
  double averageX();

 private:
  double x;
  double xx;
  double xy;
  double y;
  double a, k;
  unsigned int Np;
};

#endif
