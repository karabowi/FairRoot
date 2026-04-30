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
// PndDrcLutInfo.h
//
// Created on: 13.07.2013
// Author  : R.Dzhygadlo at gsi.de
// Author 2: M.Patsyuk at gsi.de
// -----------------------------------------

#include "PndDrcLutInfo.h"

ClassImp(PndDrcLutInfo)

  // -----   Default constructor   -------------------------------------------
  PndDrcLutInfo::PndDrcLutInfo()
  : TObject(), fAnglesSize(0), fPixelSize(0), fTimesSize(0), fLutTimesSize(0), fPathsSize(0), fChDiffsSize(0), fNOfBouncesSize(0), fNOfEVReflectionsSize(0), fLambdasSize(0),
    fHitTimesSize(0), fTruePathsSize(0), fAnglesArray(), fPixelEndArray(), fTimesArray(), fLutTimesArray(), fPathsArray(), fChDiffsArray(), fNOfBouncesArray(),
    fPhotonStartPosition(), fNOfEVReflectionsArray(), fLambdaArray(), fHitTimeArray(), fTruePathArray(), fReflected(), fLutReflected(), fChPartDir(TVector3()),
    fChPartDirInBar(TVector3()), fChPartDirInBar2(TVector3()), fChPartPdg(0), fCherenkovMC(0.), fCherenkovReal(0.)
{
}

void PndDrcLutInfo::AddAngle(Double_t val)
{

  fAnglesArray.push_back(val);
  fAnglesSize++;
}

void PndDrcLutInfo::AddPixelEnd(Double_t val)
{

  fPixelEndArray.push_back(val);
  fPixelSize++;
}

void PndDrcLutInfo::AddLutTime(Double_t val, Bool_t ref)
{

  fLutTimesArray.push_back(val);
  fLutReflected.push_back(ref);
  fLutTimesSize++;
}

void PndDrcLutInfo::AddTime(Double_t val)
{

  fTimesArray.push_back(val);
  fTimesSize++;
}

void PndDrcLutInfo::AddTime(Double_t val, Bool_t ref)
{

  fTimesArray.push_back(val);
  fReflected.push_back(ref);
  fTimesSize++;
}

void PndDrcLutInfo::AddPath(Double_t val)
{

  fPathsArray.push_back(val);
  fPathsSize++;
}
void PndDrcLutInfo::AddTruePath(Double_t val)
{

  fTruePathArray.push_back(val);
  fTruePathsSize++;
}
void PndDrcLutInfo::AddLambda(Double_t val)
{

  fLambdaArray.push_back(val);
  fLambdasSize++;
}
void PndDrcLutInfo::AddHitTime(Double_t val)
{

  fHitTimeArray.push_back(val);
  fHitTimesSize++;
}
void PndDrcLutInfo::AddChDiff(Double_t val)
{

  fChDiffsArray.push_back(val);
  fChDiffsSize++;
}
void PndDrcLutInfo::AddNOfBounces(Double_t val)
{

  fNOfBouncesArray.push_back(val);
  fNOfBouncesSize++;
}

void PndDrcLutInfo::AddNOfEVReflections(Int_t val)
{

  fNOfEVReflectionsArray.push_back(val);
  fNOfEVReflectionsSize++;
}

void PndDrcLutInfo::SetChPartDir(TVector3 val)
{
  fChPartDir = val;
}

void PndDrcLutInfo::SetChPartDirInBar(TVector3 val)
{
  fChPartDirInBar = val;
}

void PndDrcLutInfo::SetChPartDirInBar2(TVector3 val)
{
  fChPartDirInBar2 = val;
}

void PndDrcLutInfo::SetChPartPdg(Int_t val)
{
  fChPartPdg = val;
}
void PndDrcLutInfo::SetCherenkovMC(Double_t val)
{
  fCherenkovMC = val;
}
void PndDrcLutInfo::SetCherenkovReal(Double_t val)
{
  fCherenkovReal = val;
}

Double_t PndDrcLutInfo::GetAngle(Int_t entry)
{
  return fAnglesArray[entry];
}

Int_t PndDrcLutInfo::GetPixelEnd(Int_t entry)
{
  return fPixelEndArray[entry];
}

Double_t PndDrcLutInfo::GetTime(Int_t entry)
{
  return fTimesArray[entry];
}
Double_t PndDrcLutInfo::GetChDiff(Int_t entry)
{
  return fChDiffsArray[entry];
}
Int_t PndDrcLutInfo::GetNOfBounces(Int_t entry)
{
  return fNOfBouncesArray[entry];
}

Int_t PndDrcLutInfo::GetNOfEVReflections(Int_t entry)
{
  return fNOfEVReflectionsArray[entry];
}
