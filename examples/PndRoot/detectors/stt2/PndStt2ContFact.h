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
 
#ifndef PNDSTT2CONTFACT_H
#define PNDSTT2CONTFACT_H

#include "FairContFact.h"

class FairContainer;

class PndStt2ContFact : public FairContFact
{
  public:
    PndStt2ContFact();
    ~PndStt2ContFact() {}
    FairParSet* createContainer(FairContainer*);
    ClassDef(PndStt2ContFact, 0);   // Factory for all PndStt2 parameter containers
};

#endif
