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
// -----                    PndSdsCell source file -----
// ----- -----
// -------------------------------------------------------------------------

#include "PndSdsCell.h"
#include <iostream>
#include "stdlib.h"

// -----   Default constructor   -------------------------------------------
PndSdsCell::PndSdsCell() {}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndSdsCell::PndSdsCell(unsigned int hitUp, unsigned int hitDw)
{
  fhit_upstream = hitUp;
  fhit_downstream = hitDw;
  fpv = -1;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndSdsCell::~PndSdsCell() {}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
ClassImp(PndSdsCell);
