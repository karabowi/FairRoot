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

#include "BSEmcPSAFPGALinFitter.h"

#include "RtypesCore.h"

BSEmcPSAFPGALinFitter::BSEmcPSAFPGALinFitter()
{
  reset();
}

BSEmcPSAFPGALinFitter::~BSEmcPSAFPGALinFitter() {}
void BSEmcPSAFPGALinFitter::reset()
{
  Np = 0;
  x = 0.0;
  xx = 0.0;
  xy = 0.0;
  y = 0.0;
}

void BSEmcPSAFPGALinFitter::putPoint(Double_t t_x, Double_t t_y)
{
  x += t_x;
  xy += t_x * t_y;
  xx += t_x * t_x;
  y += t_y;
  Np++;
}

void BSEmcPSAFPGALinFitter::fit()
{
  if (Np >= 2) {
    k = (Np * xy - y * x) / (Np * xx - x * x);
    a = (y - k * x) / Np;
  } else {
    k = 0;
    a = 0;
  }
}

Double_t BSEmcPSAFPGALinFitter::offset()
{
  return a;
}

Double_t BSEmcPSAFPGALinFitter::slope()
{
  return k;
}

Double_t BSEmcPSAFPGALinFitter::average()
{
  if (Np > 0) {
    return y / Np;
  }
  return 0.0;
}
Double_t BSEmcPSAFPGALinFitter::averageX()
{
  if (Np > 0) {
    return x / Np;
  }
  return 0.0;
}
