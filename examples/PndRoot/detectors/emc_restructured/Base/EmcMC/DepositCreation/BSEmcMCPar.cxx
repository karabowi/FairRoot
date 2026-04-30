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


#include "BSEmcMCPar.h"

#include "FairParamList.h"

ClassImp(BSEmcMCPar);
const std::string BSEmcMCPar::fgParameterName = "EmcMCPar";
BSEmcMCPar::BSEmcMCPar(const char *t_name, const char *t_title, const char *t_context) : FairParGenericSet(t_name, t_title, t_context) {}

// -----------------------------------------------------------------------------
BSEmcMCPar::~BSEmcMCPar(void) {}

// -----------------------------------------------------------------------------
void BSEmcMCPar::clear(void) {}

// -----------------------------------------------------------------------------
void BSEmcMCPar::putParams(FairParamList *t_list)
{
  if (t_list == nullptr) {
    return;
  }
  t_list->add("EnergyDepositThreshold", fEnergyDepositThreshold);
  t_list->add("CutMotherParticle", fCutMotherParticle);
  t_list->add("CutSameTrack", fCutSameTrack);
  t_list->add("NonUniformityFile", fNonUniformityFile);
}

// -----------------------------------------------------------------------------
Bool_t BSEmcMCPar::getParams(FairParamList *t_list)
{
  if (t_list == nullptr) {
    return kFALSE;
  }
  if (!t_list->fill("EnergyDepositThreshold", &fEnergyDepositThreshold)) {
    return kFALSE;
  }
  if (!t_list->fill("CutMotherParticle", &fCutMotherParticle)) {
    return kFALSE;
  }
  if (!t_list->fill("CutSameTrack", &fCutSameTrack)) {
    return kFALSE;
  }
  if (!t_list->fill("NonUniformityFile", fNonUniformityFile, 100)) {
    return kFALSE;
  }
  return kTRUE;
}
