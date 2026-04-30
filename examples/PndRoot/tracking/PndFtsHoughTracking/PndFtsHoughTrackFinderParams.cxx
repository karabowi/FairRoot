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

#include "PndFtsHoughTrackFinderParams.h"

ClassImp(PndFtsHoughTrackFinderParams)
  // -----   Default constructor   -------------------------------------------

  PndFtsHoughTrackFinderParams::PndFtsHoughTrackFinderParams()
  : fParabolaStepsPerThetaDeg(4.), fParabolaHwScan(20.),

    fParabolaNBinsPzxInv(600), fParabolaQDivPzxArgMax(0.015),

    fZLineParabola(342.75), // 368. seemed fine
    fZParabolaLine(605.),

    fUnused(0)
{
}

// -------------------------------------------------------------------------

// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndFtsHoughTrackFinderParams::~PndFtsHoughTrackFinderParams() {}
// -------------------------------------------------------------------------
