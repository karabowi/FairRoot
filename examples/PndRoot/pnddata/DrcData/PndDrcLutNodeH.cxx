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

// -----------------------------------------
// PndDrcLutNodeH.h
//
// Created on: 09.07.2013
// Author: R.Dzhygadlo at gsi.de
// Author: M.Patsyuk at gsi.de
// -----------------------------------------

#include "PndDrcLutNodeH.h"

ClassImp(PndDrcLutNodeH)

  // -----   Default constructor   -------------------------------------------
  PndDrcLutNodeH::PndDrcLutNodeH()
{
  fSize = 0;
}

// -----   Standard constructors   -----------------------------------------
PndDrcLutNodeH::PndDrcLutNodeH(Int_t nodeId)
{
  fNodeId = nodeId;
  fSize = 0;
}

PndDrcLutNodeH::PndDrcLutNodeH(Int_t nodeId, TVector3 dir, Double_t time)
{
  fNodeId = nodeId;
  fSize = 0;
}

void PndDrcLutNodeH::AddEntry(TVector3 dir, Int_t amb, Double_t time)
{

  fNodeArray.push_back(dir);
  fAmbiguity.push_back(amb);
  fTimeArray.push_back(time);
  fSize++;
}

TVector3 PndDrcLutNodeH::GetDirection(Int_t entry)
{
  return fNodeArray[entry];
}

Int_t PndDrcLutNodeH::GetAmbiguity(Int_t entry)
{
  return fAmbiguity[entry];
}

Double_t PndDrcLutNodeH::GetTime(Int_t entry)
{
  return fTimeArray[entry];
}
