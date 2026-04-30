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

#include "PndEventDisplay.h"
#include "FairLogger.h"

ClassImp(PndEventDisplay);

PndEventDisplay::PndEventDisplay() : fHitListMap() {}

PndEventDisplay::PndEventDisplay(PndEventDisplay &display) : fHitListMap(display.GetHitListMap()) {}

PndEventDisplay::~PndEventDisplay() {}

void PndEventDisplay::AddNewGroup(TString groupName, PndGeoHitList *newList)
{
  fHitListMap[groupName] = newList;
}

void PndEventDisplay::AddHit(TString groupName, FairHit *hit, Bool_t vis)
{
  fHitListMap[groupName]->AddHit(hit, vis);
}

void PndEventDisplay::AddHit(TString groupName, Double_t x, Double_t y, Double_t z, Bool_t vis)
{
  fHitListMap[groupName]->AddHit(x, y, z, vis);
}

void PndEventDisplay::AddHit(TString groupName, TGeoMatrix *mat, Bool_t vis)
{
  fHitListMap[groupName]->AddHit(mat, vis);
}

void PndEventDisplay::AddHits(TString groupName, TClonesArray *hits, Bool_t vis)
{
  fHitListMap[groupName]->AddHits(hits, vis);
}

void PndEventDisplay::SetHits(TString groupName, TClonesArray *hits, Bool_t vis)
{
  fHitListMap[groupName]->SetHits(hits, vis);
}

void PndEventDisplay::ClearHits(TString groupName)
{
  fHitListMap[groupName]->RemoveGeoHits(0);
}

PndGeoHitList *PndEventDisplay::GetHitList(TString ListName)
{
  if (fHitListMap[ListName] == 0)
    LOG(error) << " PndEventDisplay: " << ListName << " does not exist!";
  return fHitListMap[ListName];
}
