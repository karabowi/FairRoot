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

/*
 * PndLmdContFact.h
 *
 *  Created on: May 29, 2009
 *      Author: huagen
 */

#ifndef PNDLMDCONTFACT_H_
#define PNDLMDCONTFACT_H_

#include "FairContFact.h"

class FairContainer;

class PndLmdContFact : public FairContFact {
 public:
  PndLmdContFact();
  ~PndLmdContFact();
  FairParSet *createContainer(FairContainer *c);
  TList *GetDigiParNames() { return fDigiParNames; };
  TList *GetAlignParNames() { return fAlignParNames; };

 private:
  void setAllContainers();
  TList *fDigiParNames;
  TList *fAlignParNames;
  ClassDef(PndLmdContFact, 3)
};

#endif /* PNDLMDCONTFACT_H_ */
