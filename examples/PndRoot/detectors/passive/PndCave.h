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

#ifndef Cave_H
#define Cave_H

#include "FairDetector.h"
#include "FairModule.h"

class PndCave : public FairModule {
 public:
  PndCave(const char *name, const char *Title = "Exp Cave");
  PndCave();
  virtual ~PndCave();
  virtual void ConstructGeometry();

    virtual FairModule* CloneModule() const;

 private:
  Double_t world[3];
  ClassDef(PndCave, 1) // PNDCaveSD
};

#endif // Cave_H
