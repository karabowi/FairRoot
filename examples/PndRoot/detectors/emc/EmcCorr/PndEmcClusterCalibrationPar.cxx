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
// PndEmcClusterCalibrationPar
//
// Container class for parametrization of EMC cluster energy correction
// class is inherited from FairParGenericSet
//
// Parametrization of EMC energy correction is a function of energy and polar angle and has different parameters in each of 4 regions:
// barrel, forward and backward endcap, shashlyk. Two different parameters set are used in barrel for energy below and above 1 GeV
// For the barrel , forward and backward endcap the following function is used with 10 parameters
// eout1=e* exp(factor1);
// factor1=
// p0
// +p1*log(e1)
// +p2*log(e1)*log(e1)
// +p3*log(e1)*log(e1)*log(e1)
// +p4*cos(theta1)
// +p5*cos(theta1)*cos(theta1)
// +p6*cos(theta1)*cos(theta1)*cos(theta1)
// +p7*cos(theta1)*cos(theta1)*cos(theta1)*cos(theta1)
// +p8*cos(theta1)*cos(theta1)*cos(theta1)*cos(theta1)*cos(theta1)
// +p9*log(e1)*cos(theta1);
// For shashlyk the following parametrization is used with 5 parameters:
// eout=(3.31694-0.0183379/sqrt(e1)+0.0327113/e1+0.00040156/(e1*e1)
// -0.00641305/(e1*sqrt(e1)))*e1;
//
// The index for parameter set within
// map<Int_t,std::vector<Double_t> > fClusterCalibrationPars
// are: 1 - barrel (below 1 GeV), 2 - barrel (above 1 GeV)
// 3 - forward endcap, 4 -backward endcap, 5 - shashlyk
/////////////////////////////////////////////////////////////

#include "PndEmcClusterCalibrationPar.h"
#include <iostream>
#include "assert.h"

ClassImp(PndEmcClusterCalibrationParObject);

void PndEmcClusterCalibrationParObject::SetCalibrationPar(Int_t iParSet, Double_t *pars)
{
  std::vector<Double_t> parvec;
  if (iParSet == 5)
    parvec.assign(pars, pars + 5);
  else
    parvec.assign(pars, pars + 10);

  fClusterCalibrationPars[iParSet] = parvec;
}

void PndEmcClusterCalibrationParObject::GetCalibrationPar(Int_t iParSet, Double_t *pars)
{
  pars[0] = fClusterCalibrationPars[iParSet][0];
  pars[1] = fClusterCalibrationPars[iParSet][1];
  pars[2] = fClusterCalibrationPars[iParSet][2];
  pars[3] = fClusterCalibrationPars[iParSet][3];
  pars[4] = fClusterCalibrationPars[iParSet][4];
  if (iParSet != 5) {
    pars[5] = fClusterCalibrationPars[iParSet][5];
    pars[6] = fClusterCalibrationPars[iParSet][6];
    pars[7] = fClusterCalibrationPars[iParSet][7];
    pars[8] = fClusterCalibrationPars[iParSet][8];
    pars[9] = fClusterCalibrationPars[iParSet][9];
  }
}

ClassImp(PndEmcClusterCalibrationPar)

  PndEmcClusterCalibrationPar::PndEmcClusterCalibrationPar(const char *name, const char *title, const char *context)
  : FairParGenericSet(name, title, context), fParObject(new PndEmcClusterCalibrationParObject())
{
  fParObject = nullptr;
}

Bool_t PndEmcClusterCalibrationPar::IsValid()
{
  if (fParObject == nullptr)
    return false;

  return true;
}

void PndEmcClusterCalibrationPar::GetClusterCalibrationParameters(Int_t iParSet, Double_t *pars)
{

  if (fParObject != nullptr) {
    fParObject->GetCalibrationPar(iParSet, pars);
  } else {
    std::cout << "Wrong parameter set in PndEmcClusterCalibrationPar" << std::endl;
    abort();
  }
}

void PndEmcClusterCalibrationPar::putParams(FairParamList *list)
{
  if (!list)
    return;
  list->addObject("PndEmcClusterCalibration", fParObject);
}

Bool_t PndEmcClusterCalibrationPar::getParams(FairParamList *list)
{
  if (!list)
    return kFALSE;
  if (!list->fillObject("PndEmcClusterCalibration", fParObject))
    return kFALSE;
  return kTRUE;
}
