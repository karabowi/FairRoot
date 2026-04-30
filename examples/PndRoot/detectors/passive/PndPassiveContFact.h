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

#ifndef PNDPASSIVECONTFACT_H
#define PNDPASSIVECONTFACT_H

#include "FairContFact.h"

class FairContainer;

class PndPassiveContFact : public FairContFact {
 private:
  void setAllContainers();

 public:
  PndPassiveContFact();
  ~PndPassiveContFact() {}
  FairParSet *createContainer(FairContainer *);
  ClassDef(PndPassiveContFact, 0) // Factory for all Passive parameter containers
};

#endif /* !PNDPASSIVECONTFACT_H */
