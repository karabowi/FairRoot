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
 * PndGeoHypGeBase.cxx
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

#include "PndGeoHypGeBase.h"

using namespace std;

ClassImp(PndGeoHypGeBase)

  PndGeoHypGeBase::PndGeoHypGeBase()
{
}

PndGeoHypGeBase::~PndGeoHypGeBase() {}

void PndGeoHypGeBase::BuildTArb8Array(Double_t *OutputArray, Int_t Point1, Int_t Point2, Int_t Offset1, Int_t Offset2, Int_t ShiftValue, Int_t NumberOfPoints)
{
  // Point1(2) are running with every single fragment of the cryostat.
  // Offset11(2) are increased with every new layer (lower, middle, upper, ...) of the cryostat
  Int_t Offset3 = Offset1 + ShiftValue;
  Int_t Offset4 = Offset2 + ShiftValue;
  OutputArray[0] = CoordinatesInputArray[(Point2 - 1) * NumberOfPoints + 2 * Offset1];
  OutputArray[1] = CoordinatesInputArray[(Point2 - 1) * NumberOfPoints + 2 * Offset1 + 1];
  OutputArray[2] = CoordinatesInputArray[(Point2 - 1) * NumberOfPoints + 2 * Offset3];
  OutputArray[3] = CoordinatesInputArray[(Point2 - 1) * NumberOfPoints + 2 * Offset3 + 1];
  OutputArray[4] = CoordinatesInputArray[(Point1 - 1) * NumberOfPoints + 2 * Offset3];
  OutputArray[5] = CoordinatesInputArray[(Point1 - 1) * NumberOfPoints + 2 * Offset3 + 1];
  OutputArray[6] = CoordinatesInputArray[(Point1 - 1) * NumberOfPoints + 2 * Offset1];
  OutputArray[7] = CoordinatesInputArray[(Point1 - 1) * NumberOfPoints + 2 * Offset1 + 1];
  OutputArray[8] = CoordinatesInputArray[(Point2 - 1) * NumberOfPoints + 2 * Offset2];
  OutputArray[9] = CoordinatesInputArray[(Point2 - 1) * NumberOfPoints + 2 * Offset2 + 1];
  OutputArray[10] = CoordinatesInputArray[(Point2 - 1) * NumberOfPoints + 2 * Offset4];
  OutputArray[11] = CoordinatesInputArray[(Point2 - 1) * NumberOfPoints + 2 * Offset4 + 1];
  OutputArray[12] = CoordinatesInputArray[(Point1 - 1) * NumberOfPoints + 2 * Offset4];
  OutputArray[13] = CoordinatesInputArray[(Point1 - 1) * NumberOfPoints + 2 * Offset4 + 1];
  OutputArray[14] = CoordinatesInputArray[(Point1 - 1) * NumberOfPoints + 2 * Offset2];
  OutputArray[15] = CoordinatesInputArray[(Point1 - 1) * NumberOfPoints + 2 * Offset2 + 1];
}

void PndGeoHypGeBase::BuildTArb8Array(Double_t *OutputArray, Int_t Point1, Int_t Point2, Int_t Offset1, Int_t Offset2, Int_t Offset3, Int_t Offset4, Int_t NumberOfPoints)
{
  OutputArray[0] = CoordinatesInputArray[(Point1 - 1) * NumberOfPoints + 2 * Offset1];
  OutputArray[1] = CoordinatesInputArray[(Point1 - 1) * NumberOfPoints + 2 * Offset1 + 1];
  OutputArray[2] = CoordinatesInputArray[(Point1 - 1) * NumberOfPoints + 2 * Offset3];
  OutputArray[3] = CoordinatesInputArray[(Point1 - 1) * NumberOfPoints + 2 * Offset3 + 1];
  OutputArray[4] = CoordinatesInputArray[(Point2 - 1) * NumberOfPoints + 2 * Offset3];
  OutputArray[5] = CoordinatesInputArray[(Point2 - 1) * NumberOfPoints + 2 * Offset3 + 1];
  OutputArray[6] = CoordinatesInputArray[(Point2 - 1) * NumberOfPoints + 2 * Offset1];
  OutputArray[7] = CoordinatesInputArray[(Point2 - 1) * NumberOfPoints + 2 * Offset1 + 1];
  OutputArray[8] = CoordinatesInputArray[(Point1 - 1) * NumberOfPoints + 2 * Offset2];
  OutputArray[9] = CoordinatesInputArray[(Point1 - 1) * NumberOfPoints + 2 * Offset2 + 1];
  OutputArray[10] = CoordinatesInputArray[(Point1 - 1) * NumberOfPoints + 2 * Offset4];
  OutputArray[11] = CoordinatesInputArray[(Point1 - 1) * NumberOfPoints + 2 * Offset4 + 1];
  OutputArray[12] = CoordinatesInputArray[(Point2 - 1) * NumberOfPoints + 2 * Offset4];
  OutputArray[13] = CoordinatesInputArray[(Point2 - 1) * NumberOfPoints + 2 * Offset4 + 1];
  OutputArray[14] = CoordinatesInputArray[(Point2 - 1) * NumberOfPoints + 2 * Offset2];
  OutputArray[15] = CoordinatesInputArray[(Point2 - 1) * NumberOfPoints + 2 * Offset2 + 1];
}

void PndGeoHypGeBase::BuildTArb8FrontArray(Double_t *OutputArray, Int_t Point1, Int_t Point2, Int_t Point3, Int_t Point4, Int_t NumberOfPoints)
{
  OutputArray[0] = CoordinatesInputArray[(Point1 - 1) * NumberOfPoints];
  OutputArray[1] = CoordinatesInputArray[(Point1 - 1) * NumberOfPoints + 1];
  OutputArray[2] = CoordinatesInputArray[(Point2 - 1) * NumberOfPoints];
  OutputArray[3] = CoordinatesInputArray[(Point2 - 1) * NumberOfPoints + 1];
  OutputArray[4] = CoordinatesInputArray[(Point3 - 1) * NumberOfPoints];
  OutputArray[5] = CoordinatesInputArray[(Point3 - 1) * NumberOfPoints + 1];
  OutputArray[6] = CoordinatesInputArray[(Point4 - 1) * NumberOfPoints];
  OutputArray[7] = CoordinatesInputArray[(Point4 - 1) * NumberOfPoints + 1];
  OutputArray[8] = CoordinatesInputArray[(Point1 - 1) * NumberOfPoints + 2];
  OutputArray[9] = CoordinatesInputArray[(Point1 - 1) * NumberOfPoints + 2 + 1];
  OutputArray[10] = CoordinatesInputArray[(Point2 - 1) * NumberOfPoints + 2];
  OutputArray[11] = CoordinatesInputArray[(Point2 - 1) * NumberOfPoints + 2 + 1];
  OutputArray[12] = CoordinatesInputArray[(Point3 - 1) * NumberOfPoints + 2];
  OutputArray[13] = CoordinatesInputArray[(Point3 - 1) * NumberOfPoints + 2 + 1];
  OutputArray[14] = CoordinatesInputArray[(Point4 - 1) * NumberOfPoints + 2];
  OutputArray[15] = CoordinatesInputArray[(Point4 - 1) * NumberOfPoints + 2 + 1];
  // for( Int_t i = 0; i < 16; i++)
  // cout << "Top_" << i <<": " << OutputArray[i] << endl;
}

void PndGeoHypGeBase::BuildTArb8TopArray(Double_t *OutputArray, Int_t Point1, Int_t Point2, Int_t Point3, Int_t Point4, Int_t Offset, Int_t NumberOfPoints)
{
  OutputArray[0] = CoordinatesInputArray[(Point1 - 1) * NumberOfPoints + 2 * Offset];
  OutputArray[1] = CoordinatesInputArray[(Point1 - 1) * NumberOfPoints + 2 * Offset + 1];
  OutputArray[2] = CoordinatesInputArray[(Point2 - 1) * NumberOfPoints + 2 * Offset];
  OutputArray[3] = CoordinatesInputArray[(Point2 - 1) * NumberOfPoints + 2 * Offset + 1];
  OutputArray[4] = CoordinatesInputArray[(Point3 - 1) * NumberOfPoints + 2 * Offset];
  OutputArray[5] = CoordinatesInputArray[(Point3 - 1) * NumberOfPoints + 2 * Offset + 1];
  OutputArray[6] = CoordinatesInputArray[(Point4 - 1) * NumberOfPoints + 2 * Offset];
  OutputArray[7] = CoordinatesInputArray[(Point4 - 1) * NumberOfPoints + 2 * Offset + 1];
  OutputArray[8] = CoordinatesInputArray[(Point1 - 1) * NumberOfPoints + 2 * Offset];
  OutputArray[9] = CoordinatesInputArray[(Point1 - 1) * NumberOfPoints + 2 * Offset + 1];
  OutputArray[10] = CoordinatesInputArray[(Point2 - 1) * NumberOfPoints + 2 * Offset];
  OutputArray[11] = CoordinatesInputArray[(Point2 - 1) * NumberOfPoints + 2 * Offset + 1];
  OutputArray[12] = CoordinatesInputArray[(Point3 - 1) * NumberOfPoints + 2 * Offset];
  OutputArray[13] = CoordinatesInputArray[(Point3 - 1) * NumberOfPoints + 2 * Offset + 1];
  OutputArray[14] = CoordinatesInputArray[(Point4 - 1) * NumberOfPoints + 2 * Offset];
  OutputArray[15] = CoordinatesInputArray[(Point4 - 1) * NumberOfPoints + 2 * Offset + 1];
  // for( Int_t i = 0; i < 16; i++)
  //	cout << "Top_" << i <<": " << OutputArray[i] << endl;
}
void PndGeoHypGeBase::FillInputArray(Int_t NumberOfPoints, Int_t NumberOfXYInput)
{
  for (Int_t iPoint = 0; iPoint < NumberOfPoints; iPoint++) {
    if (!ParametersInputFile) {
      cout << "Inputfile does not exist!" << endl;
    } else if (ParametersInputFile.good()) {
      string InputBuffer;
      char *pEnd;
      getline(ParametersInputFile, InputBuffer);
      // cout << InputBuffer << endl;

      CoordinatesInputArray[iPoint * NumberOfXYInput] = strtod(InputBuffer.data(), &pEnd);
      // cout << "Value_0" <<": " <<CoordinatesInputArray[iPoint*NumberOfXYInput] << endl;
      for (Int_t iValues = 1; iValues < NumberOfXYInput; iValues++) {
        CoordinatesInputArray[iPoint * NumberOfXYInput + iValues] = strtod(pEnd, &pEnd);
        // cout << "Value_" << iValues <<": " <<CoordinatesInputArray[iPoint*NumberOfXYInput+iValues] << endl;
      }
    }
  }
}

void PndGeoHypGeBase::FillZValues(Int_t NumberofDifferentZValues)
{
  if (!ParametersInputFile) {
    cout << "Inputfile does not exist!" << endl;
  } else if (ParametersInputFile.good()) {
    string InputBuffer;
    char *pEnd;
    getline(ParametersInputFile, InputBuffer);
    // cout << InputBuffer << endl;

    ZValuesInputArray[0] = strtod(InputBuffer.data(), &pEnd);
    // cout << "Value_0" <<": " <<ZValuesInputArray[0] << endl;
    for (Int_t iValues = 1; iValues < NumberofDifferentZValues; iValues++) {
      ZValuesInputArray[iValues] = strtod(pEnd, &pEnd);
      // cout << "Value_" << iValues <<": " <<ZValuesInputArray[iValues] << endl;
    }
  }
}

void PndGeoHypGeBase::ReadCoordinatesAndAngles(string Filename, Int_t NumberOfCluster, Double_t *CoordinatesAndAngles)
{
  string InputBuffer;
  char *pEnd;
  ifstream InputFile;
  InputFile.open(Filename.data());
  if (!InputFile) {
    cout << "Inputfile does not exist!" << endl;
  } else {
    cout << "Reading coordinates and angles from file" << endl;
    for (Int_t iCluster = 0; iCluster < NumberOfCluster; iCluster++) {
      if (InputFile.good()) {

        getline(InputFile, InputBuffer);
        // cout << InputBuffer << endl;
        CoordinatesAndAngles[iCluster * 6] = strtod(InputBuffer.data(), &pEnd);
        // cout << "Value_0" <<": " <<CoordinatesAndAngles[iCluster*6] << endl;
        for (Int_t iValues = 1; iValues < 6; iValues++) {
          CoordinatesAndAngles[iCluster * 6 + iValues] = strtod(pEnd, &pEnd);
          // cout << "Value_" << iValues <<": " <<CoordinatesAndAngles[iCluster*6+iValues] << endl;
        }
      }
    }
    cout << "All read" << endl;
  }
}
