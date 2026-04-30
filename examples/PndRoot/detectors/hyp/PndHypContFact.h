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
// -----               Created A.Sanchez                  -----
// -------------------------------------------------------------------------

#ifndef PNDHYPCONTFACT_H
#define PNDHYPCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class PndHypContFact : public FairContFact {
 private:
  void setAllContainers();

 public:
  PndHypContFact();
  ~PndHypContFact() {}
  FairParSet *createContainer(FairContainer *);

  ClassDef(PndHypContFact, 2) // Factory for all HYP parameter containers
};

#endif /* !PNDHYPCONTFACT_H */
