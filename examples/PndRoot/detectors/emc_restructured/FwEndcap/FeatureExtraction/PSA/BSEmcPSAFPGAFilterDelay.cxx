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

#include "BSEmcPSAFPGAFilterDelay.h"

#include "RtypesCore.h"

BSEmcPSAFPGAFilterDelay::BSEmcPSAFPGAFilterDelay() : fBuffer(0), fBPointer(0) {}

BSEmcPSAFPGAFilterDelay::~BSEmcPSAFPGAFilterDelay()
{
  fBuffer.resize(0);
}

void BSEmcPSAFPGAFilterDelay::set(UInt_t t_newBufferSize)
{
  fBuffer.resize(t_newBufferSize);
  for (UInt_t i = 0; i < t_newBufferSize; i++) {
    fBuffer[i] = 0;
  }
  fBPointer = 0;
}

Float_t BSEmcPSAFPGAFilterDelay::put(Float_t t_valueToStore)
{
  fBPointer %= fBuffer.size();
  Float_t out = fBuffer[fBPointer];
  fBuffer[fBPointer] = t_valueToStore;
  fBPointer++;
  return out;
}

void BSEmcPSAFPGAFilterDelay::resetToZero()
{
  for (float &i : fBuffer) {
    i = 0;
  }
  fBPointer = 0;
}
