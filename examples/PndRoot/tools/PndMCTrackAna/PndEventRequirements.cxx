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

// -------------------------------------------------------------------------
// -----                      PndEventRequirements source file               -----
// -----                  Created 16/09/19  by T.Stockmanns            -----
// -------------------------------------------------------------------------

#include <FairRootManager.h>

#include <iostream>

#include "PndEventRequirements.h"
using namespace std;

// -----   Default constructor   -------------------------------------------
PndEventRequirements::PndEventRequirements() {}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndEventRequirements::~PndEventRequirements() {}

// -------------------------------------------------------------------------
void PndEventRequirements::InitRequirements()
{
  for (auto &req : fTrackRequirements)
    req.Init();
}

ClassImp(PndEventRequirements);
