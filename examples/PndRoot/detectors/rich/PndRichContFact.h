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

#ifndef PNDRICHCONTFACT_H
#define PNDRICHCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class PndRichContFact : public FairContFact {
 private:
  void setAllContainers();

 public:
  PndRichContFact();
  ~PndRichContFact() {}
  FairParSet *createContainer(FairContainer *);
  ClassDef(PndRichContFact, 0) // Factory for all PndRich parameter containers
};

#endif
