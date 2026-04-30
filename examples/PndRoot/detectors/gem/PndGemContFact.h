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

#ifndef PNDGEMCONTFACT_H
#define PNDGEMCONTFACT_H

#include "FairContFact.h"

class FairContainer;
// class FairParIo;

class PndGemContFact : public FairContFact {
 public:
  PndGemContFact();
  ~PndGemContFact() {}
  FairParSet *createContainer(FairContainer *c);

 private:
  void setAllContainers();

  ClassDef(PndGemContFact, 2); // Factory for all GEM parameter containers
};

#endif /* !GEMCONTFACT_H */
