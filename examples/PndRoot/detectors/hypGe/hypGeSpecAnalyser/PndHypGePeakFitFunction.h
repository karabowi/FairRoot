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

/*
 * THypGePeakFitFunction.h
 *
 * Copyright 2013 Marcell Steinen <steinen@kph.uni-mainz.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */

// support class for peak fitting, ROOT needs this external construction (?)
#ifndef PNDHYPGEPEAKFITFUNCTION_H
#define PNDHYPGEPEAKFITFUNCTION_H

#include "TMath.h"

class PndHypGePeakFitFunction {
 public:
  PndHypGePeakFitFunction();
  Double_t PeakFunc(Double_t *x, Double_t *par);
  Double_t PeakFuncGaussian(Double_t *x, Double_t *par);
  Double_t PeakFuncFreeSkewedPosition(Double_t *x, Double_t *par);
  Double_t PeakFuncFreeSkewedPositionWithoutFirstGausian(Double_t *x, Double_t *par);
  Double_t PeakFuncDoubleGausian(Double_t *x, Double_t *par);

  Double_t NewFunction(Double_t *x, Double_t *par);

  Double_t GausOnly(Double_t *x, Double_t *par);
  Double_t SmoothedStepOnly(Double_t *x, Double_t *par);
  Double_t LinearOnly(Double_t *x, Double_t *par);
  Double_t SkewedOnly(Double_t *x, Double_t *par);
  Double_t FreeSkewedOnly(Double_t *x, Double_t *par);
  Double_t SecondGausOnly(Double_t *x, Double_t *par);
  Double_t NewSkewedOnly(Double_t *x, Double_t *par);

 private:
  /* add your private declarations */

  ClassDef(PndHypGePeakFitFunction, 1) // This line must be used for Go4 and PANDAroot !!!!!!!!!!!   gives "IsA()" Error if compiled with g++
};

#endif /* PNDHYPGEPEAKFITFUNCTION_H */
