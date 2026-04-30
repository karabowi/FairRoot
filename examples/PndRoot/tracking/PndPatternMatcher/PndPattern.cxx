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
 * PndPattern.cxx
 *
 *  Created on: Nov 8, 2017
 *      Author: Michael Papenbrock
 */

#include "PndPattern.h"

ClassImp(PndPattern)

  PndPattern::PndPattern()
{
  Clear();
}

PndPattern::~PndPattern() {}

void PndPattern::Clear()
{
  ftubeIDs.clear();
  fmomenta.clear();
  fsectorID = -1;
  fcount = 0;
}
bool PndPattern::IsEmpty()
{
  if (ftubeIDs.size() == 0) {
    return true;
  } else {
    return false;
  }
}

void PndPattern::AddMomenta(std::vector<TVector3> momentaVector)
{
  fmomenta.insert(std::end(fmomenta), std::begin(momentaVector), std::end(momentaVector));
}
