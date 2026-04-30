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

#include "PndEmcPSAFPGALinFitter.h"
#include <iostream>
PndEmcPSAFPGALinFitter::PndEmcPSAFPGALinFitter()
{ 
   //  std::cout << "-I- FPGALinFitter::Reset" << std::endl;
  reset();
}

PndEmcPSAFPGALinFitter::~PndEmcPSAFPGALinFitter() {}
void PndEmcPSAFPGALinFitter::reset()
{
  Np = 0;
  x = 0.0;
  xx = 0.0;
  xy = 0.0;
  y = 0.0;
}

void PndEmcPSAFPGALinFitter::putPoint(double ix, double iy)
{
	// std::cout << "-I- FPGALinFitter::putPoint" << std::endl;
  x += ix;
  xy += ix * iy;
  xx += ix * ix;
  y += iy;
  Np++;
}

void PndEmcPSAFPGALinFitter::fit()
{ 
 // std::cout << "-I- FPGALinFitter::fit" << std::endl;
  if (Np >= 2) {
    k = (Np * xy - y * x) / (Np * xx - x * x);
    a = (y - k * x) / Np;
  } else {
    k = 0;
    a = 0;
  }
}

double PndEmcPSAFPGALinFitter::offset()
{
  return a;
}

double PndEmcPSAFPGALinFitter::slope()
{
  return k;
}

double PndEmcPSAFPGALinFitter::average()
{
  if (Np > 0)
    return y / Np;
  return 0.0;
}
double PndEmcPSAFPGALinFitter::averageX()
{
  if (Np > 0)
    return x / Np;
  return 0.0;
}
