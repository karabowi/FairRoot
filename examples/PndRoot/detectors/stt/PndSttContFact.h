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

#ifndef PNDSTTCONTFACT_H
#define PNDSTTCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class PndSttContFact : public FairContFact {
 private:
  void setAllContainers();

 public:
  PndSttContFact();
  ~PndSttContFact() {}
  FairParSet *createContainer(FairContainer *);
  //  void  activateParIo(FairParIo* io);
  ClassDef(PndSttContFact, 0) // Factory for all STT parameter containers
};

#endif /* !PNDSTTCONTFACT_H */
