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
 * PndMvdRadDamList.h
 *
 *  Created on: Dec 16, 2008
 *      Author: stockman
 */

#ifndef PNDMVDRADDAMLIST_H
#define PNDMVDRADDAMLIST_H

#include <vector>
#include <utility>

#include "TString.h"

class PndMvdRadDamList {
 public:
  PndMvdRadDamList(TString fileName);
  virtual ~PndMvdRadDamList();

  Double_t GetWeight(Double_t energy);

 private:
  std::vector<std::pair<Double_t, Double_t>> fList;

  Int_t FindClosestEnergyIndex(Double_t energy);
  Double_t Interpolate(Double_t energy, Int_t index);

  ClassDef(PndMvdRadDamList, 1);
};

#endif /* PNDMVDRADDAMLIST_H */
