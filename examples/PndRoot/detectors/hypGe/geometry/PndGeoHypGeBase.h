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
 * PndGeoHypGeBase.h
 *
 * Copyright 2012 Marcell Steinen <steinen@kph.uni-mainz.de>
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

#ifndef PNDGEOHYPGEBASE_H
#define PNDGEOHYPGEBASE_H

#include "TGeoArb8.h"
#include "TGeoXtru.h"
#include "fstream"
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

class PndGeoHypGeBase {
 protected:
  Double_t *CoordinatesInputArray;
  Double_t *ZValuesInputArray;

  ifstream ParametersInputFile;

 public:
  PndGeoHypGeBase();
  virtual ~PndGeoHypGeBase();

  // function to build Arrays for TGeoArb8 for the side parts of the cryostat. Point1 and Point2 give the points (in xy-projection) between which the TGeoArb8 is build. The
  // diffenent Offset give the height: 1 = 0.15 cm; 2 = 11.5 cm; 3 = 15.6493 cm; 4 = 18.5 cm
  void BuildTArb8Array(Double_t *OutputArray, Int_t Point1, Int_t Point2, Int_t Offset1, Int_t Offset2, Int_t ShiftValue, Int_t NumberOfPoints);
  void BuildTArb8Array(Double_t *OutputArray, Int_t Point1, Int_t Point2, Int_t Offset1, Int_t Offset2, Int_t Offset3, Int_t Offset4, Int_t NumberOfPoints);
  void BuildTArb8FrontArray(Double_t *OutputArray, Int_t Point1, Int_t Point2, Int_t Point3, Int_t Point4, Int_t NumberOfPoints);
  void BuildTArb8TopArray(Double_t *OutputArray, Int_t Point1, Int_t Point2, Int_t Point3, Int_t Point4, Int_t Offset, Int_t NumberOfPoints);
  void FillInputArray(Int_t NumberOfPoints, Int_t NumberOfXYInput);
  void FillZValues(Int_t NumberofDifferentZValues);

  void ReadCoordinatesAndAngles(string Filename, Int_t NumberOfCluster, Double_t *CoordinatesAndAngles);

  ClassDef(PndGeoHypGeBase, 0); // Class for GeoHypGeBase
};

#endif /* PNDGEOHYPGEBASE_H */
