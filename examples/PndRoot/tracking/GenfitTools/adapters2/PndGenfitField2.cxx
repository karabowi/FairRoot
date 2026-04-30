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

// modified by Elisabetta Prencipe 19/5/2014
#include "PndGenfitField2.h"
#include "FairRunAna.h"
#include "FairField.h"

PndGenfitField2::PndGenfitField2() : AbsBField() {}

TVector3 PndGenfitField2::get(const TVector3 &pos) const
{
  double x[3] = {pos.X(), pos.Y(), pos.Z()};
  double B[3] = {0, 0, 0};
  FairRunAna::Instance()->GetField()->Field(x, B);
  return TVector3(B[0], B[1], B[2]);
}

void PndGenfitField2::get(const double &posX, const double &posY, const double &posZ, double &Bx, double &By, double &Bz) const
{
  double x[3] = {posX, posY, posZ};
  double B[3] = {0, 0, 0};
  FairRunAna::Instance()->GetField()->Field(x, B);
  Bx = B[0];
  By = B[1];
  Bz = B[2];
}
