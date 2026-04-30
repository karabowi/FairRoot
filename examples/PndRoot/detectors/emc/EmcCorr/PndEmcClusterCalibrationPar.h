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

//#pragma once
#ifndef PNDEMCCLUSTERCALIBRATIONPAR_H
#define PNDEMCCLUSTERCALIBRATIONPAR_H

#include "FairParGenericSet.h"
#include "FairParamList.h"
#include <cstdlib>
#include <map>
#include <vector>

class PndEmcClusterCalibrationParObject : public TObject {
 public:
  void SetCalibrationPar(Int_t iParSet, Double_t *pars);
  void GetCalibrationPar(Int_t iParSet, Double_t *pars);

  PndEmcClusterCalibrationParObject() : fClusterCalibrationPars() { fClusterCalibrationPars.clear(); };
  ~PndEmcClusterCalibrationParObject(){};

 private:
  std::map<Int_t, std::vector<Double_t>> fClusterCalibrationPars;
  ClassDef(PndEmcClusterCalibrationParObject, 1);
};

class PndEmcClusterCalibrationPar : public FairParGenericSet {
 public:
  void GetClusterCalibrationParameters(Int_t iParSet, Double_t *pars);
  void SetClusterCalibrationObject(PndEmcClusterCalibrationParObject *ParObject) { fParObject = ParObject; };
  PndEmcClusterCalibrationParObject *GetParObject() { return fParObject; };
  PndEmcClusterCalibrationPar(const char *name = "PndEmcClusterCalibrationPar", const char *title = "Emc cluster calibration parameter",
                              const char *context = "TestDefaultContext");
  ~PndEmcClusterCalibrationPar(void){};

  Bool_t IsValid();

  void clear(void){};

  void putParams(FairParamList *list);
  Bool_t getParams(FairParamList *list);

 private:
  PndEmcClusterCalibrationParObject *fParObject;
  PndEmcClusterCalibrationPar(const PndEmcClusterCalibrationPar &L);
  PndEmcClusterCalibrationPar &operator=(const PndEmcClusterCalibrationPar &) { return *this; };
  ClassDef(PndEmcClusterCalibrationPar, 1);
};

#endif
