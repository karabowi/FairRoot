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

#include "PndRichContFact.h"

#include "PndRichGeoPar.h"

#include "FairRuntimeDb.h"

#include <iostream>

ClassImp(PndRichContFact)

  static PndRichContFact gPndRichContFact;

PndRichContFact::PndRichContFact() : FairContFact()
{
  /** Constructor (called when the library is loaded) */
  fName = "PndRichContFact";
  fTitle = "Factory for parameter containers in libPndRich";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void PndRichContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted
      contexts and adds them to
      the list of containers for the PndRich library.
  */

  FairContainer *p = new FairContainer("PndRichGeoPar", "PndRich Geometry Parameters", "TestDefaultContext");
  p->addContext("TestNonDefaultContext");

  containers->Add(p);
}

FairParSet *PndRichContFact::createContainer(FairContainer *c)
{
  /** Calls the constructor of the corresponding parameter container.
      For an actual context, which is not an empty string and not
      the default context
      of this container, the name is concatinated with the context.
  */
  const char *name = c->GetName();
  FairParSet *p = nullptr;
  if (strcmp(name, "PndRichGeoPar") == 0) {
    p = new PndRichGeoPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
  }
  return p;
}
