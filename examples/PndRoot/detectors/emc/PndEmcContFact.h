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

// -------------------------------------------------------------------------
// -----                     PndEmcContFact header file                -----
// -----               Created 14/08/06  by S.Spataro                  -----
// -------------------------------------------------------------------------

#ifndef PNDEMCCONTFACT_H
#define PNDEMCCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class PndEmcContFact : public FairContFact {
 private:
  void setAllContainers();

 public:
  PndEmcContFact();
  ~PndEmcContFact() {}
  FairParSet *createContainer(FairContainer *);
  // void  activateParIo(FairParIo* io);

  ClassDef(PndEmcContFact, 0) // Factory for all EMC parameter containers
};

#endif /* !PNDEMCCONTFACT_H */
