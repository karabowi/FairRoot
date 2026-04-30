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
// -----                     CbmHypContFact header file                -----
// -----               Created 14/08/06  by S.Spataro                  -----
// -------------------------------------------------------------------------

#ifndef PNDSCITCONTFACT_H
#define PNDSCITCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class PndSciTContFact : public FairContFact {
 private:
  void setAllContainers();

 public:
  PndSciTContFact();
  ~PndSciTContFact() {}
  FairParSet *createContainer(FairContainer *);

  ClassDef(PndSciTContFact, 0) // Factory for all HYP parameter containers
};

#endif /* !PNDSCITCONTFACT_H */
