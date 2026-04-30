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
 * PndEmcWaveformRingSorter.cxx
 */

#include "PndEmcWaveformRingSorter.h"
#include "PndEmcWaveform.h"

ClassImp(PndEmcWaveformRingSorter);

PndEmcWaveformRingSorter::~PndEmcWaveformRingSorter()
{
  // TODO Auto-generated destructor stub
}

FairTimeStamp *PndEmcWaveformRingSorter::CreateElement(FairTimeStamp *data)
{
  return new PndEmcWaveform(*(PndEmcWaveform *)data);
}
