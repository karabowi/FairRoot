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

/////////////////////////////////////////////////////////////
// PndEmcDigiNonuniformityPar
//
// Container class for Digitisation parameters
// class is inherited from FairParGenericSet
//
/////////////////////////////////////////////////////////////

#include "PndEmcDigiNonuniformityPar.h"
#include <iostream>

ClassImp(PndEmcDigiNonuniParObject);

void PndEmcDigiNonuniParObject::SetNonuniformity(Int_t crystaltype, Double_t *pars)
{
  std::vector<Double_t> parvec;
  parvec.assign(pars, pars + 3);
  fNonUniPars[crystaltype] = parvec;
}

void PndEmcDigiNonuniParObject::GetNonuniformity(Int_t crystaltype, Double_t *pars)
{
  if (fNonUniPars.find(crystaltype) != fNonUniPars.end()) {
    pars[0] = fNonUniPars[crystaltype][0];
    pars[1] = fNonUniPars[crystaltype][1];
    pars[2] = fNonUniPars[crystaltype][2];
  } else {
    pars[0] = 1;
    pars[1] = 0;
    pars[2] = 0;
  }
}

ClassImp(PndEmcDigiNonuniformityPar)

  PndEmcDigiNonuniformityPar::PndEmcDigiNonuniformityPar(const char *name, const char *title, const char *context)
  : FairParGenericSet(name, title, context), fParObject()
{
  fParObject = nullptr;
}

void PndEmcDigiNonuniformityPar::GetNonuniformityParameters(Int_t DetId, Double_t *pars)
{
  //    if(pars != nullptr){
  //        pars[0]=1/0.89497;
  //        pars[1]=-0.01582/0.89497;
  //        pars[2]=3.9881e-4/0.89497;
  //    }
  //
  Int_t crystaltype; //= 6;
  Int_t Module = DetId / 100000000;
  Int_t Row;
  switch (Module) {
  case 1:
  case 2:
    Row = (DetId / 1000000) % 100;
    crystaltype = (Row - 1) / 4 + 1;
    // calculate based on row number;
    break;
  case 3:
    crystaltype = 12; // fwendcap
    break;
  case 4:
    crystaltype = 13; // bwendcap
    break;
  case 5:
    crystaltype = 0; // shashlyk
    break;
  case 7:
    crystaltype = 6; // Proto60
    break;
  default: crystaltype = 0;
  }

  if (fParObject != nullptr) {
    fParObject->GetNonuniformity(crystaltype, pars);
  } else {
    pars[0] = 1;
    pars[1] = 0;
    pars[2] = 0;
  }
}

void PndEmcDigiNonuniformityPar::putParams(FairParamList *list)
{
  if (!list)
    return;
  list->addObject("PndEmcDigiNonuniformity", fParObject);
}

Bool_t PndEmcDigiNonuniformityPar::getParams(FairParamList *list)
{
  if (!list)
    return kFALSE;
  if (!list->fillObject("PndEmcDigiNonuniformity", fParObject))
    return kFALSE;
  return kTRUE;
}
