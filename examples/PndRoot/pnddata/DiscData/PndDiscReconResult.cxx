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

//-------------------------------------------------------------------------
// Author:      Mustafa Schmidt (Mustafa.A.Schmidt@physik.uni-giessen.de)
// Changes:
// Date:        30.11.2015
// Description: Reconstruction
//-------------------------------------------------------------------------

#include "PndDiscReconResult.h"

#include <iostream>

ClassImp(PndDiscReconResult)

  PndDiscReconResult::PndDiscReconResult()
  : TObject(), cherenkov_angle(0.), particle(0), hypothesis(0), sensor(0), pixel(0), time(0.)
{
}

PndDiscReconResult::~PndDiscReconResult() {}
