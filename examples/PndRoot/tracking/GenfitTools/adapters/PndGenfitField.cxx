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

#include "PndGenfitField.h"
#include "FairRun.h"
#include "FairRunSim.h"
#include "FairRunAna.h"
#include "FairField.h"

PndGenfitField::PndGenfitField() : GFAbsBField() {}

TVector3 PndGenfitField::get(const TVector3 &pos) const
{
  double x[3] = {pos.X(), pos.Y(), pos.Z()};
  double B[3] = {0, 0, 0};
  if (FairRun::Instance()->IsAna()) {
    FairRunAna::Instance()->GetField()->Field(x, B);
  } else {
    FairRunSim::Instance()->GetField()->Field(x, B);
  }
  return TVector3(B[0], B[1], B[2]);
}
