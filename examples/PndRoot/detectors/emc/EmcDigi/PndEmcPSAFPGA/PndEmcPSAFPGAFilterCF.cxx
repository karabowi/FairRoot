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

#include "PndEmcPSAFPGAFilterCF.h"

PndEmcPSAFPGAFilterCF::PndEmcPSAFPGAFilterCF() : buffer(0), bpointer(0) {}

PndEmcPSAFPGAFilterCF::~PndEmcPSAFPGAFilterCF()
{
  buffer.resize(0);
}

void PndEmcPSAFPGAFilterCF::set(unsigned int newBufferSize, float newRatio)
{
  buffer.resize(newBufferSize);
  for (unsigned int i = 0; i < newBufferSize; i++)
    buffer[i] = 0;
  ratio = newRatio;
  bpointer = 0;
}

void PndEmcPSAFPGAFilterCF::resetToZero()
{
  bpointer = 0;
  for (unsigned int i = 0; i < buffer.size(); i++)
    buffer[i] = 0;
}

float PndEmcPSAFPGAFilterCF::put(float valueToStore)
{
  bpointer %= buffer.size();
  float cfd = buffer[bpointer] - ratio * valueToStore;
  buffer[bpointer] = valueToStore;
  bpointer++;
  return cfd;
};
