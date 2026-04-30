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

#include "PndFtsTube.h"

#include <iostream>

PndFtsTube::PndFtsTube() : fCenPosition(TVector3(0, 0, 0)), fRotationMatrix(TMatrixT<double>(3, 3)), fRadIn(0), fRadOut(0), fHalfLength(0)
{
  fRotationMatrix[0][0] = -1.;
  fRotationMatrix[0][1] = -1.;
  fRotationMatrix[0][2] = -1.;

  fRotationMatrix[1][0] = -1.;
  fRotationMatrix[1][1] = -1.;
  fRotationMatrix[1][2] = -1.;

  fRotationMatrix[2][0] = -1.;
  fRotationMatrix[2][1] = -1.;
  fRotationMatrix[2][2] = -1.;
}

PndFtsTube::PndFtsTube(PndFtsTube &tube)
  : TObject(tube), fCenPosition(tube.GetPosition()), fRotationMatrix(TMatrixT<double>(3, 3)), fRadIn(tube.GetRadIn()), fRadOut(tube.GetRadOut()), fHalfLength(tube.GetHalfLength())
{
  fRotationMatrix.ResizeTo(3, 3);
  fRotationMatrix = tube.GetRotationMatrix();
}

PndFtsTube::PndFtsTube(Double_t x, Double_t y, Double_t z, Double_t r11, Double_t r12, Double_t r13, Double_t r21, Double_t r22, Double_t r23, Double_t r31, Double_t r32,
                       Double_t r33, Double_t radin, Double_t radout, Double_t hl)
  : fCenPosition(TVector3(x, y, z)), fRotationMatrix(TMatrixT<double>(3, 3)), fRadIn(radin), fRadOut(radout), fHalfLength(hl)
{

  fRotationMatrix[0][0] = r11;
  fRotationMatrix[0][1] = r12;
  fRotationMatrix[0][2] = r13;

  fRotationMatrix[1][0] = r21;
  fRotationMatrix[1][1] = r22;
  fRotationMatrix[1][2] = r23;

  fRotationMatrix[2][0] = r31;
  fRotationMatrix[2][1] = r32;
  fRotationMatrix[2][2] = r33;
}

PndFtsTube::~PndFtsTube()
{
  //  fCenPosition.Delete();
  //  fRotationMatrix.Delete();
}

TVector3 PndFtsTube::GetPosition() const
{
  return fCenPosition;
}

TMatrixT<Double_t> PndFtsTube::GetRotationMatrix() const
{
  return fRotationMatrix;
}

Double_t PndFtsTube::GetRadIn() const
{
  return fRadIn;
}

Double_t PndFtsTube::GetRadOut() const
{
  return fRadOut;
}

Double_t PndFtsTube::GetHalfLength() const
{
  return fHalfLength;
}

TVector3 PndFtsTube::GetWireDirection() const
{
  return TVector3(fRotationMatrix[0][2], fRotationMatrix[1][2], fRotationMatrix[2][2]);
}

ClassImp(PndFtsTube)
