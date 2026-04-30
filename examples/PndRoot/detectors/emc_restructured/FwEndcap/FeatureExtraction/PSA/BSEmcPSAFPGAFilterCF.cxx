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

#include "BSEmcPSAFPGAFilterCF.h"

#include "RtypesCore.h"

BSEmcPSAFPGAFilterCF::BSEmcPSAFPGAFilterCF() : fBuffer(0), fBPointer(0) {}

BSEmcPSAFPGAFilterCF::~BSEmcPSAFPGAFilterCF()
{
  fBuffer.resize(0);
}

void BSEmcPSAFPGAFilterCF::set(UInt_t t_newBufferSize, Float_t t_newRatio)
{
  fBuffer.resize(t_newBufferSize);
  for (UInt_t i = 0; i < t_newBufferSize; i++) {
    fBuffer[i] = 0;
  }
  fRatio = t_newRatio;
  fBPointer = 0;
}

void BSEmcPSAFPGAFilterCF::resetToZero()
{
  fBPointer = 0;
  for (float &i : fBuffer) {
    i = 0;
  }
}

Float_t BSEmcPSAFPGAFilterCF::put(Float_t t_valueToStore)
{
  fBPointer %= fBuffer.size();
  Float_t cfd = fBuffer[fBPointer] - fRatio * t_valueToStore;
  fBuffer[fBPointer] = t_valueToStore;
  fBPointer++;
  return cfd;
};
