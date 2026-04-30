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


/////////////////////////////////////////////////////////////
//
//  PndMdtContFact
//
//  Factory for the parameter containers in libMdt
//
//  update: George Serbanut @2008
//
/////////////////////////////////////////////////////////////

#include "PndMdtContFact.h"
#include "FairRuntimeDb.h"

#include "PndGeoMdtPar.h"
#include "PndMdtRecoPar.h"

#include <iostream>
#include <iomanip>

ClassImp(PndMdtContFact)

  static PndMdtContFact gPndMdtContFact;

PndMdtContFact::PndMdtContFact()
{
  // Constructor (called when the library is loaded)
  fName = "PndMdtContFact";
  fTitle = "Factory for parameter containers in libMdt";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void PndMdtContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the Mdt library.*/

  FairContainer *p1 = new FairContainer("PndGeoMdtPar", "MDT Geometry Parameters", "TestDefaultContext");
  p1->addContext("TestDefaultContext");

  FairContainer *p2 = new FairContainer("PndMdtRecoPar", "MDT Reconstruction Parameters", "TestDefaultContext");
  p2->addContext("TestDefaultContext");

  containers->Add(p1);
  containers->Add(p2);
}

FairParSet *PndMdtContFact::createContainer(FairContainer *c)
{
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char *name = c->GetName();
  FairParSet *p = nullptr;

  if (strcmp(name, "PndGeoMdtPar") == 0) {
    p = new PndGeoMdtPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
  };

  if (strcmp(name, "PndMdtRecoPar") == 0) {
    p = new PndMdtRecoPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
  };

  return p;
}
