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

#include "PndSTESettings.h"
#include "FairRunAna.h"

void PndSTESettings::SetMagneticField()
{
  FairField *Field = FairRunAna::Instance()->GetField();
  Double_t po[3], BB[3];
  po[0] = 0.;
  po[1] = 0.;
  po[2] = 0.;
  Field->GetFieldValue(po, BB);
  fBz = BB[2] / 10;
}

ClassImp(PndSTESettings);
