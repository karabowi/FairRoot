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
#include "BSEmcErrorCalculationPar.h"

#include "FairParamList.h"

std::string BSEmcErrorCalculationPar::fgParameterName = "EmcErrorCalculationPar";
BSEmcErrorCalculationPar::BSEmcErrorCalculationPar(const char *t_name, const char *t_title, const char *t_context) : FairParGenericSet(t_name, t_title, t_context) {}

BSEmcErrorCalculationPar::~BSEmcErrorCalculationPar(void) {}

void BSEmcErrorCalculationPar::clear(void) {}

void BSEmcErrorCalculationPar::putParams(FairParamList *t_list)
{
  if (t_list == nullptr) {
    return;
  }
  t_list->add("ScaleFactor", fScaleFactor);
  t_list->add("MinEnergyCutOff", fMinEnergyCutOff);
  t_list->add("MaxEnergyCutOff", fMaxEnergyCutOff);
  t_list->add("DetectorPosition", fDetectorPosition);

  t_list->add("EnergyParA", fEnergyParA);
  t_list->add("EnergyPower", fEnergyPower);
  t_list->add("EnergyConst", fEnergyConst);
  t_list->add("EnergyQuadr", fEnergyQuadr);
  t_list->add("Position1ParA", fPosition1ParA);
  t_list->add("Position1Power", fPosition1Power);
  t_list->add("Position1Const", fPosition1Const);
  t_list->add("Position2ParA", fPosition2ParA);
  t_list->add("Position2Power", fPosition2Power);
  t_list->add("Position2Const", fPosition2Const);
}

Bool_t BSEmcErrorCalculationPar::getParams(FairParamList *t_list)
{
  if (t_list == nullptr) {
    return kFALSE;
  }
  if (!t_list->fill("ScaleFactor", &fScaleFactor)) {
    return kFALSE;
  }
  if (!t_list->fill("MinEnergyCutOff", &fMinEnergyCutOff)) {
    return kFALSE;
  }
  if (!t_list->fill("MaxEnergyCutOff", &fMaxEnergyCutOff)) {
    return kFALSE;
  }
  if (!t_list->fill("DetectorPosition", &fDetectorPosition)) {
    return kFALSE;
  }

  if (!t_list->fill("EnergyParA", &fEnergyParA)) {
    return kFALSE;
  }
  if (!t_list->fill("EnergyPower", &fEnergyPower)) {
    return kFALSE;
  }
  if (!t_list->fill("EnergyConst", &fEnergyConst)) {
    return kFALSE;
  }
  if (!t_list->fill("EnergyQuadr", &fEnergyQuadr)) {
    return kFALSE;
  }
  if (!t_list->fill("Position1ParA", &fPosition1ParA)) {
    return kFALSE;
  }
  if (!t_list->fill("Position1Power", &fPosition1Power)) {
    return kFALSE;
  }
  if (!t_list->fill("Position1Const", &fPosition1Const)) {
    return kFALSE;
  }
  if (!t_list->fill("Position2ParA", &fPosition2ParA)) {
    return kFALSE;
  }
  if (!t_list->fill("Position2Power", &fPosition2Power)) {
    return kFALSE;
  }
  if (!t_list->fill("Position2Const", &fPosition2Const)) {
    return kFALSE;
  }

  return kTRUE;
}
