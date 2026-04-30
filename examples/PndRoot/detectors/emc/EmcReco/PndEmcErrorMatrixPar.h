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

//#pragma once
#ifndef PNDEMCERRORMATRIXPAR_H
#define PNDEMCERRORMATRIXPAR_H

#include "FairParGenericSet.h"
#include "FairParamList.h"
#include <cstdlib>
#include <map>
#include <vector>

/**
 * @brief Container class for EMC error matrix parameter class is inherited from FairParGenericSet
 *
 * Parametrization of EMC error matrix
 * functions used for parameterization
 * Energy: Delta(E)/E = (a^2/E^power) + const^2 + (quadr/E)^2
 * position: Delta(x)=(a*a/E^power) + const^2
 * There are 10 parameters (engParA, engPower, engConst, engQuadr, pos1ParA, pos1Power,
 * pos1Const, pos2ParA, pos2Power, pos2Const) for each component of EMC (barrel,
 * fwd endcap, bwd endcap, shashlyk)
 * @ingroup PndEmc
 */
class PndEmcErrorMatrixParObject : public TObject {
 public:
  void SetErrorMatrix(Int_t detectorComponent, Double_t *pars);
  void GetErrorMatrix(Int_t detectorComponent, Double_t *pars);

  PndEmcErrorMatrixParObject() : fErrorMatrixPars() { fErrorMatrixPars.clear(); };
  ~PndEmcErrorMatrixParObject(){};

 private:
  std::map<Int_t, std::vector<Double_t>> fErrorMatrixPars;
  ClassDef(PndEmcErrorMatrixParObject, 1);
};

class PndEmcErrorMatrixPar : public FairParGenericSet {
 public:
  void GetErrorMatrixParameters(Int_t detectorComponent, Double_t *pars);
  void SetErrorMatrixObject(PndEmcErrorMatrixParObject *ParObject) { fParObject = ParObject; };
  PndEmcErrorMatrixParObject *GetParObject() { return fParObject; };
  PndEmcErrorMatrixPar(const char *name = "PndEmcErrorMatrixPar", const char *title = "Emc error matrix parameter", const char *context = "TestDefaultContext");
  ~PndEmcErrorMatrixPar(void){};

  Bool_t IsValid();

  void clear(void){};

  void putParams(FairParamList *list);
  Bool_t getParams(FairParamList *list);

 private:
  PndEmcErrorMatrixParObject *fParObject;
  ClassDef(PndEmcErrorMatrixPar, 1);

  PndEmcErrorMatrixPar(const PndEmcErrorMatrixPar &L) : FairParGenericSet(L){};
  PndEmcErrorMatrixPar &operator=(const PndEmcErrorMatrixPar &) { return *this; };
};

#endif
