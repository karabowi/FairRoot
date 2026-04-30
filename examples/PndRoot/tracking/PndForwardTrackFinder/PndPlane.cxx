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
 * PndPlane.cpp
 *
 *  Created on: Jun 14, 2016
 *      Author: kibellus
 */

#include "PndPlane.h"

PndPlane::PndPlane(PndLine line, Int_t layer)
{
  base = line.getP1();
  dir1 = line.getP2() - line.getP1();
  if (layer % 4 == 1) {
    dir2 = TVector3(TMath::Sin(-5 * TMath::DegToRad()), TMath::Cos(-5 * TMath::DegToRad()), 0);
  } else if (layer % 4 == 2) {
    dir2 = TVector3(TMath::Sin(5 * TMath::DegToRad()), TMath::Cos(5 * TMath::DegToRad()), 0);
  } else {
    dir2 = TVector3(0, 1, 0);
  }
}

PndPlane::~PndPlane()
{
  // TODO Auto-generated destructor stub
}

PndLine PndPlane::getIntersection(PndPlane &p)
{
  // create the direction vector
  TVector3 norm1 = dir1.Cross(dir2);
  Double_t d1 = norm1.Dot(base);
  TVector3 norm2 = p.dir1.Cross(p.dir2);
  Double_t d2 = norm2.Dot(p.base);
  TVector3 dirLine = norm1.Cross(norm2);
  // create the base vector

  // init array
  Double_t arr[2][3];
  arr[0][0] = norm1[0];
  arr[0][1] = norm1[1];
  arr[0][2] = d1 - norm1[2] * base[2];
  arr[1][0] = norm2[0];
  arr[1][1] = norm2[1];
  arr[1][2] = d2 - norm2[2] * base[2];

  // swap rows
  if (arr[0][0] == 0) {
    for (int i = 0; i < 3; i++) {
      // Double_t temp = arr[0][i]; //[R.K.03/2017] unused variable
      arr[0][i] = arr[1][i];
      arr[1][i] = arr[0][i];
    }
  }

  Double_t fac = -arr[1][0] / arr[0][0];
  for (int i = 0; i < 3; i++) {
    arr[1][i] += fac * arr[0][i];
  }

  Double_t x2 = arr[1][2] / arr[1][1];
  Double_t x1 = (arr[0][2] - x2 * arr[0][1]) / arr[0][0];

  // create the line
  TVector3 lineBase;
  lineBase.SetXYZ(x1, x2, base[2]);
  PndLine l(lineBase, dirLine);
  return l;
}

void PndPlane::Print()
{
  cout << "Plane:";
  cout << "(" << base[0] << "," << base[1] << "," << base[2] << ")";
  cout << "+a*(" << dir1[0] << "," << dir1[1] << "," << dir1[2] << ")";
  cout << "+b*(" << dir2[0] << "," << dir2[1] << "," << dir2[2] << ")";
}

TVector3 PndPlane::getIntersection(PndLine l)
{
  // create the Matrix
  TVector3 lineDir = l.getDir();
  TVector3 lineBase = l.getP1();
  Double_t matrix[3][4];
  for (int i = 0; i < 3; i++) {
    matrix[i][0] = dir1[i];
    matrix[i][1] = dir2[i];
    matrix[i][2] = -lineDir[i];
    matrix[i][3] = lineBase[i] - base[i];
  }
  // solve the Matrix
  for (int i = 0; i < 3; i++) {
    if (TMath::Abs(matrix[i][i]) < 0.000000001) { // equal 0
      // search max element
      Int_t max = i;
      for (int j = i + 1; j < 3; j++) {
        if (TMath::Abs(matrix[j][i]) > TMath::Abs(matrix[max][i]))
          max = j;
      }
      // no greater element found
      if (max == i) {
        cout << "\033[1;31mNo Intersection from plane and line\033[0m" << endl;
        return TVector3(0, 0, 0);
      }
      // swap lines
      for (int j = i; j < 4; j++) {
        Double_t temp = matrix[i][j];
        matrix[i][j] = matrix[max][j];
        matrix[max][j] = temp;
      }
    }
    for (int j = i + 1; j < 3; j++) {
      Double_t fac = -matrix[j][i] / matrix[i][i];
      for (int k = i; k < 4; k++) {
        matrix[j][k] += (fac * matrix[i][k]);
      }
    }
  }
  // create the result
  Double_t lamp = matrix[2][3] / matrix[2][2];
  return lineBase + lamp * lineDir;
}
