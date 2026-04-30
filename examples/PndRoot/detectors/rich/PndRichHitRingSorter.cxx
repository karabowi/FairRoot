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
 * PndRichHitRingSorter.cxx
 *
 */
// -------------------------------------------------------------------------
// -----                   PndRichHitRingSorter source file            -----
// -----         HARPHOOL KUMAWAT h.kumawat@gsi.de                     -----
// -----                                                               -----
// -------------------------------------------------------------------------

#include "PndRichHitRingSorter.h"
#include "PndRichDigi.h"

ClassImp(PndRichHitRingSorter);

PndRichHitRingSorter::~PndRichHitRingSorter()
{
  // TODO Auto-generated destructor stub
}

FairTimeStamp *PndRichHitRingSorter::CreateElement(FairTimeStamp *data)
{
  return new PndRichDigi(*(PndRichDigi *)data);
}
