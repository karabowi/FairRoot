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

#include "PndEmcPSAFPGAMLinFitter.h"

void PndEmcPSAFPGAMLinFitter::set(int newBufferSize)
{
  Np = newBufferSize;
  mx.set(newBufferSize);
  mxx.set(newBufferSize);
  mxy.set(newBufferSize);
  my.set(newBufferSize);
  return;
}

void PndEmcPSAFPGAMLinFitter::putPoint(double ix, double iy)
{
  x = mx.put(ix);
  xy = mxy.put(ix * iy);
  xx = mxx.put(ix * ix);
  y = my.put(iy);
}

void PndEmcPSAFPGAMLinFitter::fit()
{
  k = (Np * xy - y * x) / (Np * xx - x * x);
  a = (y - k * x) / Np;
}

double PndEmcPSAFPGAMLinFitter::offset()
{
  return a;
}

double PndEmcPSAFPGAMLinFitter::slope()
{
  return k;
}

double PndEmcPSAFPGAMLinFitter::average()
{
  if (Np > 0)
    return y / Np;
  return 0.0;
}
