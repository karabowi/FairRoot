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

#include "PndMvdSiHit.h"

PndMvdSiHit::PndMvdSiHit() : fBox(0), fChannel(0), fAdc(0), fNumFrames(0) {}

PndMvdSiHit::PndMvdSiHit(UShort_t box, UShort_t channel, Int_t adc, UShort_t numFrames) : fBox(box), fChannel(channel), fAdc(adc), fNumFrames(numFrames) {}

PndMvdSiHit::~PndMvdSiHit() {}

ClassImp(PndMvdSiHit);
