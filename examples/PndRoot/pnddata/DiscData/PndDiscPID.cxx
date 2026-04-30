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
// Description: Particle Identification
//-------------------------------------------------------------------------

#include "PndDiscPID.h"

#include <iostream>

ClassImp(PndDiscPID)

  PndDiscPID::PndDiscPID()
  : TObject(), loglikepion(0.), loglikekaon(0.), loglikeproton(0.), pion(0.), kaon(0.), proton(0.)
{
}

PndDiscPID::~PndDiscPID() {}
