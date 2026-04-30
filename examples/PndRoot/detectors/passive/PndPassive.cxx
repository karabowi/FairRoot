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

#include "PndPassive.h"

#include "FairLogger.h"

PndPassive::PndPassive() : FairModule() {}
PndPassive::PndPassive(const char *name, const char *title) : FairModule(name, title) {}
PndPassive::~PndPassive() {}

void PndPassive::ConstructGeometry()
{

  TString fileName = GetGeometryFileName();
  if (fileName.EndsWith(".root")) {
    LOG(info) << "PndPassive - constructing geometry " << fileName;
    ConstructRootGeometry();
  } else if (fileName.EndsWith(".geo")) {
    LOG(warning) << "PndPassive - ASCII geometry in " << fileName << " can not be constructed.";
    ConstructASCIIGeometry();
  } else {
    LOG(warning) << "PndPassive - Geometry in " << fileName << " can not be constructed";
  }
}

ClassImp(PndPassive)
