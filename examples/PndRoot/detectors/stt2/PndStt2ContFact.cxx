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

#include "PndStt2ContFact.h"

#include "FairRuntimeDb.h"
//#include "PndStt2GeoPar.h"
#include "PndStt2DigiPar.h"

#include <iostream>

ClassImp(PndStt2ContFact);

static PndStt2ContFact gPndStt2ContFact;

PndStt2ContFact::PndStt2ContFact()
{
  fName = "PndStt2ContFact";
  fTitle = "Factory for parameter containers in libPndStt2";
  auto p2 = new FairContainer("PndStt2DigiPar", "PndStt2 DigiReco Parameters", "TestDefaultContext");
  AddContainer(p2);
  FairRuntimeDb::instance()->addContFactory(this);
}

FairParSet* PndStt2ContFact::createContainer(FairContainer* c)
{
    /** Calls the constructor of the corresponding parameter container.
      For an actual context, which is not an empty string and not
      the default context
      of this container, the name is concatinated with the context.
  */
  const char *name = c->GetName();
  FairParSet *p = nullptr;

  //if (strcmp(name, "PndStt2GeoPar") == 0) {
    //p = new PndStt2GeoPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    //cout <<"PndStt2GeoPar created"<<endl;
  //}

  if (strcmp(name, "PndStt2DigiPar") == 0) {
    p = new PndStt2DigiPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    cout <<"PndStt2DigiPar created"<<endl;
  }

  return p;
}
