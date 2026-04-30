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

////////////////////////////////////////////////////////////////////////////
// PndGeoOtPar source file
//
// Class for geometry parameters of OT
//
// authors: Radoslaw Karabowicz, GSI, 2024
//
// modified from PndGeoFtsPar by Nafija Ibrišimović in 2023
////////////////////////////////////////////////////////////////////////////

#include "PndGeoOtPar.h"
// from FairRoot
#include <FairParamList.h>
// standard
#include <iostream>
#include <iomanip>

ClassImp(PndGeoOtPar)

  PndGeoOtPar::PndGeoOtPar(const char *name, const char *title, const char *context)
  : FairParGenericSet(name, title, context), fGeoSensNodes(new TObjArray()), fGeoPassNodes(new TObjArray()), fGeoType(-1), fTubeInRad(0), fTubeOutRad(0)
{
}

PndGeoOtPar::~PndGeoOtPar(void) {}

void PndGeoOtPar::clear(void)
{
  if (fGeoSensNodes) {
    fGeoSensNodes->Delete();
    delete fGeoSensNodes;
  }
  if (fGeoPassNodes) {
    fGeoPassNodes->Delete();
    delete fGeoPassNodes;
  }
}

void PndGeoOtPar::putParams(FairParamList *l)
{
  if (!l)
    return;
  l->addObject("FairGeoNodes Sensitive List", fGeoSensNodes);
  l->addObject("FairGeoNodes Passive List", fGeoPassNodes);
  l->add("Tube Innen Radius", fTubeInRad);
  l->add("Tube Outer Radius", fTubeOutRad);
  l->add("Geometry Type", fGeoType);
}

Bool_t PndGeoOtPar::getParams(FairParamList *l)
{
  if (!l)
    return kFALSE;
  if (!l->fillObject("FairGeoNodes Sensitive List", fGeoSensNodes))
    return kFALSE;
  if (!l->fillObject("FairGeoNodes Passive List", fGeoPassNodes))
    return kFALSE;
  if (!l->fill("Tube Innen Radius", &fTubeInRad))
    return kFALSE;
  if (!l->fill("Tube Outer Radius", &fTubeOutRad))
    return kFALSE;
  if (!l->fill("Geometry Type", &fGeoType))
    return kFALSE;

  return kTRUE;
}
