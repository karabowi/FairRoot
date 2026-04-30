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

//#pragma once
#ifndef PNDEMCDIGINONUNIFORMITYPAR_H
#define PNDEMCDIGINONUNIFORMITYPAR_H

#include "FairParGenericSet.h"
#include "FairParamList.h"
#include <map>
#include <vector>

class PndEmcDigiNonuniParObject : public TObject {
 public:
  void SetNonuniformity(Int_t crystaltype, Double_t *pars);
  void GetNonuniformity(Int_t crystaltype, Double_t *pars);

  PndEmcDigiNonuniParObject() : fNonUniPars() { fNonUniPars.clear(); };
  ~PndEmcDigiNonuniParObject(){};

 private:
  std::map<Int_t, std::vector<Double_t>> fNonUniPars;
  ClassDef(PndEmcDigiNonuniParObject, 1);
};

class PndEmcDigiNonuniformityPar : public FairParGenericSet {
 public:
  void GetNonuniformityParameters(Int_t DetId, Double_t *pars);
  void SetNonuniParObject(PndEmcDigiNonuniParObject *ParObject) { fParObject = ParObject; };
  PndEmcDigiNonuniformityPar(const char *name = "PndEmcDigiPar", const char *title = "Emc digi parameter", const char *context = "TestDefaultContext");
  ~PndEmcDigiNonuniformityPar(void){};

  void clear(void){};

  void putParams(FairParamList *list);
  Bool_t getParams(FairParamList *list);

 private:
  PndEmcDigiNonuniParObject *fParObject;

  PndEmcDigiNonuniformityPar(const PndEmcDigiNonuniformityPar &L);
  PndEmcDigiNonuniformityPar &operator=(const PndEmcDigiNonuniformityPar &) { return *this; };

  ClassDef(PndEmcDigiNonuniformityPar, 1);
};

#endif
