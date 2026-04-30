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

#ifndef PNDDRCCONTFACT_H
#define PNDDRCCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class PndDrcContFact : public FairContFact {
 private:
  void setAllContainers();

 public:
  PndDrcContFact();
  ~PndDrcContFact() {}
  FairParSet *createContainer(FairContainer *);
  /*  void  activateParIo(FairParIo* io); */
  ClassDef(PndDrcContFact, 0) // Factory for all DRC parameter containers
};

#endif /* !PNDDRCCONTFACT_H */
