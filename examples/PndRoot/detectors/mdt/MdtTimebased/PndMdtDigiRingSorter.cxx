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
 * PndMdtDigiRingSorter.cxx
 */

#include "PndMdtDigiRingSorter.h"
#include "FairTimeStamp.h"

ClassImp(PndMdtDigiRingSorter);

PndMdtDigiRingSorter::~PndMdtDigiRingSorter()
{
  // TODO Auto-generated destructor stub
}

FairTimeStamp *PndMdtDigiRingSorter::CreateElement(FairTimeStamp *data)
{
  // preservation, or invoke copy construction of data, after converting data type
  return (FairTimeStamp *)data->Clone();
}
