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

#ifndef Target_H
#define Target_H

#include "FairDetector.h"
#include "FairModule.h"

class PndTarget : public FairModule {
 public:
  PndTarget(const char *name, const char *Title = "PND Target");
  PndTarget();
  virtual ~PndTarget();
  virtual void ConstructGeometry();
  ClassDef(PndTarget, 1)
};

#endif // Target_H
