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
 * PndMvdRadDamList.cpp
 *
 *  Created on: Dec 16, 2008
 *      Author: stockman
 */

#include "PndMvdRadDamList.h"

#include "TFile.h"
#include "TTree.h"
#include "FairLogger.h"

#include <iostream>

PndMvdRadDamList::PndMvdRadDamList(TString fileName) : fList()
{
  Float_t E, W;
  TFile f(fileName);
  TTree *t = (TTree *)f.Get("data");
  t->SetBranchAddress("E_GeV", &E);
  t->SetBranchAddress("Weight", &W);

  for (int i = 0; i < t->GetEntriesFast(); i++) {
    t->GetEntry(i);
    fList.push_back(std::pair<Double_t, Double_t>(E, W));
  }
}

PndMvdRadDamList::~PndMvdRadDamList()
{
  // TODO Auto-generated destructor stub
}

Double_t PndMvdRadDamList::GetWeight(Double_t energy)
{
  Int_t index = FindClosestEnergyIndex(energy);
  return Interpolate(energy, index);
}

Int_t PndMvdRadDamList::FindClosestEnergyIndex(Double_t energy)
{

  for (unsigned int i = 0; i < fList.size(); i++) {
    if (fList[i].first > energy)
      return i - 1;
  }
  return -2; // energy is bigger than the latest entry in the list
}

Double_t PndMvdRadDamList::Interpolate(Double_t energy, Int_t index)
{
  std::pair<Double_t, Double_t> ew1, ew2;
  if (index == -1) { // energy is lower than the smallest energy entry in the list
    ew1 = fList[0];
    ew2 = fList[1];
    LOG(warn) << " PndMvdRadDamList::Interpolate: Energy is lower than list values";
  } else if (index < -1) {
    ew1 = fList[fList.size() - 2];
    ew2 = fList[fList.size() - 1];
    LOG(warn) << " PndMvdRadDamList::Interpolate: Energy is bigger than list values";
  } else {
    ew1 = fList[index];
    ew2 = fList[index + 1];
  }
  return ew1.second + (ew2.second - ew1.second) / (ew2.first - ew1.first) * (energy - ew1.first);
}

ClassImp(PndMvdRadDamList);
