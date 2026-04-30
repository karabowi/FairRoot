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

#ifndef PNDPASSIVE_HH
#define PNDPASSIVE_HH

#include <string>

#include "FairModule.h"

/**
 * @brief Simply implements ConstructGeometry to call FairModule::ConstructRootGeometry()
 * Useful to include passive material (e.g. the Central Space Frame, mechanical detector holding structures, etc.) via .root-files
 * @code {.c++}
 * PndPassive *csf = new PndPassive();
 * csf->SetGeometryFileName("CentralSpaceFrame.root");
 * fRun->AddModule(csf);
 * @endcode
 *
 */
class PndPassive : public FairModule {
 public:
  virtual void ConstructGeometry();

  PndPassive(const char *name, const char *Title = "PndPassive");
  PndPassive();
  virtual ~PndPassive();
  ClassDef(PndPassive, 1) // PndPassive
};

#endif /*PNDPASSIVE_HH*/
