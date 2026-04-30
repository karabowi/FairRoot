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
 * PndLmdDigiStrips.cxx
 *
 *  Created on: Aug 10, 2009
 *      Author: huagen
 */
#include "PndLmdDigiStrips.h"

PndLmdDigiStrips ::PndLmdDigiStrips()
{ // fIndex = 0;
  fdetID = 0;
  // fdetName = 0;
  fFE = 0;
  fChannel = 0;
  fCharge = 0;
}

PndLmdDigiStrips ::PndLmdDigiStrips(Int_t index, Int_t detID, TString detName, Int_t fe, Int_t channel, Double_t charge)
{
  fIndex.push_back(index);
  fdetID = detID;
  fdetName = detName;
  fFE = fe;
  fChannel = channel;
  fCharge = charge;
}

ClassImp(PndLmdDigiStrips);
