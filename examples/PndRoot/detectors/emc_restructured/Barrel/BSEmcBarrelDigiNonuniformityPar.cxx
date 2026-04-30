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
// BSEmcBarrelDigiNonuniformityPar
//
// Container class for Digitisation parameters
// class is inherited from FairParGenericSet
//
/////////////////////////////////////////////////////////////

#include "BSEmcBarrelDigiNonuniformityPar.h"

#include "FairParamList.h"

ClassImp(BSEmcBarrelDigiNonuniParObject);

void BSEmcBarrelDigiNonuniParObject::SetNonuniformity(Int_t t_crystaltype, Double_t *t_pars)
{
  std::vector<Double_t> parvec;
  parvec.assign(t_pars, t_pars + 3);
  fNonUniPars[t_crystaltype] = parvec;
}

void BSEmcBarrelDigiNonuniParObject::GetNonuniformity(Int_t t_crystaltype, Double_t *t_pars)
{
  if (fNonUniPars.find(t_crystaltype) != fNonUniPars.end()) {
    t_pars[0] = fNonUniPars[t_crystaltype][0];
    t_pars[1] = fNonUniPars[t_crystaltype][1];
    t_pars[2] = fNonUniPars[t_crystaltype][2];
  } else {
    t_pars[0] = 1;
    t_pars[1] = 0;
    t_pars[2] = 0;
  }
}

ClassImp(BSEmcBarrelDigiNonuniformityPar)

  BSEmcBarrelDigiNonuniformityPar::BSEmcBarrelDigiNonuniformityPar(const char *t_name, const char *t_title, const char *t_context)
  : FairParGenericSet(t_name, t_title, t_context), fParObject(nullptr)
{
}

void BSEmcBarrelDigiNonuniformityPar::GetNonuniformityParameters(Int_t t_DetId, Double_t *t_pars)
{
  //    if(pars != nullptr){
  //        pars[0]=1/0.89497;
  //        pars[1]=-0.01582/0.89497;
  //        pars[2]=3.9881e-4/0.89497;
  //    }
  //
  Int_t crystaltype{0}; //= 6;
  const Int_t Module = t_DetId / 100000000;
  const Int_t Row = (t_DetId / 1000000) % 100;
  switch (Module) {
  case 1:
  case 2:
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
    fParObject->GetNonuniformity(crystaltype, t_pars);
  } else {
    t_pars[0] = 1;
    t_pars[1] = 0;
    t_pars[2] = 0;
  }
}

void BSEmcBarrelDigiNonuniformityPar::putParams(FairParamList *t_list)
{
  if (t_list == nullptr) {
    return;
  }
  t_list->addObject("PndEmcDigiNonuniformity", fParObject);
}

Bool_t BSEmcBarrelDigiNonuniformityPar::getParams(FairParamList *t_list)
{
  if (t_list == nullptr) {
    return kFALSE;
  }
  if (!t_list->fillObject("PndEmcDigiNonuniformity", fParObject)) {
    return kFALSE;
  }
  return kTRUE;
}
