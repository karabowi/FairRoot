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


#ifndef PNDMDTCONTFACT_H
#define PNDMDTCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class PndMdtContFact : public FairContFact {
 private:
  void setAllContainers();

 public:
  PndMdtContFact();
  ~PndMdtContFact() {}
  FairParSet *createContainer(FairContainer *);
  /*  void  activateParIo(FairParIo* io); */
  ClassDef(PndMdtContFact, 0) // Factory for all MDT parameter containers
};

#endif /* !PNDMDTCONTFACT_H */
