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

#ifndef PNDFTSTUBE_H
#define PNDFTSTUBE_H 1

#include "TVector3.h"
#include "TMatrixT.h"
#include "TObject.h"

class PndFtsTube : public TObject {

 public:
  /** Default constructor **/
  PndFtsTube();
  PndFtsTube(PndFtsTube &tube);

  ~PndFtsTube();
  PndFtsTube(Double_t x, Double_t y, Double_t z, Double_t r11, Double_t r12, Double_t r13, Double_t r21, Double_t r22, Double_t r23, Double_t r31, Double_t r32, Double_t r33,
             Double_t radin, Double_t radout, Double_t hl);

  TVector3 GetPosition() const;
  TMatrixT<Double_t> GetRotationMatrix() const;
  Double_t GetRadIn() const;
  Double_t GetRadOut() const;
  Double_t GetHalfLength() const;
  TVector3 GetWireDirection() const;

  bool IsSkew() const { return (0. != GetWireDirection().X()); }

 private:
  TVector3 fCenPosition;
  TMatrixT<double> fRotationMatrix;
  Double_t fRadIn, fRadOut, fHalfLength;

  ClassDef(PndFtsTube, 1);
};

#endif
