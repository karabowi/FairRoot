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
// PndEmcErrorMatrixPar
//
// Container class for EMC error matrix parameter
// class is inherited from FairParGenericSet
//
// Parametrization of EMC error matrix
// functions used for parameterization
// Energy: Delta(E)/E = (a^2/E^power) + const^2 + (quadr/E)^2
// position: Delta(x)=(a*a/E^power) + const^2
// There are 10 parameters (engParA, engPower, engConst, engQuadr, pos1ParA, pos1Power,
// pos1Const, pos2ParA, pos2Power, pos2Const) for each component of EMC (barrel,
// fwd endcap, bwd endcap, shashlyk)
/////////////////////////////////////////////////////////////

#include "PndEmcErrorMatrixPar.h"
#include <iostream>
#include "assert.h"

ClassImp(PndEmcErrorMatrixParObject);

void PndEmcErrorMatrixParObject::SetErrorMatrix(Int_t detectorComponent, Double_t *pars)
{
  std::vector<Double_t> parvec;
  parvec.assign(pars, pars + 10);
  fErrorMatrixPars[detectorComponent] = parvec;
}

void PndEmcErrorMatrixParObject::GetErrorMatrix(Int_t detectorComponent, Double_t *pars)
{
  pars[0] = fErrorMatrixPars[detectorComponent][0];
  pars[1] = fErrorMatrixPars[detectorComponent][1];
  pars[2] = fErrorMatrixPars[detectorComponent][2];
  pars[3] = fErrorMatrixPars[detectorComponent][3];
  pars[4] = fErrorMatrixPars[detectorComponent][4];
  pars[5] = fErrorMatrixPars[detectorComponent][5];
  pars[6] = fErrorMatrixPars[detectorComponent][6];
  pars[7] = fErrorMatrixPars[detectorComponent][7];
  pars[8] = fErrorMatrixPars[detectorComponent][8];
  pars[9] = fErrorMatrixPars[detectorComponent][9];
}

ClassImp(PndEmcErrorMatrixPar)

  PndEmcErrorMatrixPar::PndEmcErrorMatrixPar(const char *name, const char *title, const char *context)
  : FairParGenericSet(name, title, context), fParObject(new PndEmcErrorMatrixParObject())
{
  fParObject = nullptr;
}
Bool_t PndEmcErrorMatrixPar::IsValid()
{
  if (fParObject == nullptr)
    return false;

  return true;
}

void PndEmcErrorMatrixPar::GetErrorMatrixParameters(Int_t detectorComponent, Double_t *pars)
{

  if (fParObject != nullptr) {
    fParObject->GetErrorMatrix(detectorComponent, pars);
  } else {
    std::cout << "Wrong detector component in PndEmcErrorMatrixPar" << std::endl;
    abort();
  }
}

void PndEmcErrorMatrixPar::putParams(FairParamList *list)
{
  if (!list)
    return;
  list->addObject("PndEmcErrorMatrix", fParObject);
}

Bool_t PndEmcErrorMatrixPar::getParams(FairParamList *list)
{
  if (!list)
    return kFALSE;
  if (!list->fillObject("PndEmcErrorMatrix", fParObject))
    return kFALSE;
  return kTRUE;
}
