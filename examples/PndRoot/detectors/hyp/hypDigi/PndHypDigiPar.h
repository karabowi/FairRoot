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

//
// C++ Interface: MvdDigiPar
//
// Description:
//
//
// Author: t.stockmanns <stockman@ikp455>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//// slightly modified by A. Sanchez for hyp purpose

#ifndef PNDHYPDIGIPAR_H
#define PNDHYPDIGIPAR_H

#include "FairParGenericSet.h"
#include "FairParamList.h"

class PndHypDigiPar : public FairParGenericSet {
 public:
  Double_t dimX; // PixelDimension in x
  Double_t dimY; // PixelDimension in y
  // Double_t skew;      // skewing angle if one exists
  Double_t threshold; // Discriminator threshold
  Double_t noise;     // Complete noise including threshold dispersion
                      //     Text_t   sensName;  // Sensor name (Strip, Pixel, etc...)
                      //     Text_t   feName;    // Frontend name (APV25, nyxiter, ...)

  PndHypDigiPar(const char *name = "PndHypParTest", const char *title = "Hyp digi parameter", const char *context = "TestDefaultContext");
  ~PndHypDigiPar(void){};
  void clear(void){};
  void putParams(FairParamList *list);
  Bool_t getParams(FairParamList *list);
  ClassDef(PndHypDigiPar, 2);
};

#endif
