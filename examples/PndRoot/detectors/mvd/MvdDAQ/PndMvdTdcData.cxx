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

#include <stdlib.h>
#include <TObject.h>

#include "PndMvdTdcData.h"

PndMvdTdcData::PndMvdTdcData() : fChannel(0), fTdc(0), fTrailing(0) {}

PndMvdTdcData::PndMvdTdcData(UShort_t channel, Int_t tdcValue, Bool_t trailing) : fChannel(channel), fTdc(tdcValue), fTrailing(trailing) {}

PndMvdTdcData::~PndMvdTdcData() {}

ClassImp(PndMvdTdcData);
