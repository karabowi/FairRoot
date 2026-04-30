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

#include "BSEmcPSAFPGAFilterMWD.h"

BSEmcPSAFPGAFilterMWD::BSEmcPSAFPGAFilterMWD() : fBuffer(0), fBPointer(0) {}

BSEmcPSAFPGAFilterMWD::~BSEmcPSAFPGAFilterMWD()
{
  fBuffer.resize(0);
}

void BSEmcPSAFPGAFilterMWD::set(Float_t t_tau, UInt_t t_newBufferSize)
{
  fBuffer.resize(t_newBufferSize);
  for (UInt_t i = 0; i < t_newBufferSize; i++) {
    fBuffer[i] = 0;
  }
  fBPointer = 0;
  fSum = 0.0;
  fLambda = 1.0 / t_tau;
}

Float_t BSEmcPSAFPGAFilterMWD::put(Float_t t_valueToStore)
{
  fBPointer %= fBuffer.size();
  Float_t diff = t_valueToStore - fBuffer[fBPointer];
  Float_t mwd = diff + fLambda * fSum;
  fSum += diff;
  fBuffer[fBPointer] = t_valueToStore;
  fBPointer++;
  return mwd;
}

void BSEmcPSAFPGAFilterMWD::resetToZero()
{
  for (float &i : fBuffer) {
    i = 0;
  }
  fBPointer = 0.;
  fSum = 0.;
}
