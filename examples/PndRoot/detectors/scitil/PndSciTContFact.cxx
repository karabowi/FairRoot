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
//  PndSciTContFact
//
//  Factory for the parameter containers in libSciT
//
//
//
/////////////////////////////////////////////////////////////

#include "PndSciTContFact.h"
#include "FairRuntimeDb.h"
#include "iostream"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"
#include "PndGeoSciTPar.h"
#include <iostream>
#include <iomanip>

ClassImp(PndSciTContFact)

  static PndSciTContFact gPndSciTContFact;

PndSciTContFact::PndSciTContFact()
{
  // Constructor (called when the library is loaded)
  fName = "PndSciTContFact";
  fTitle = "Factory for parameter containers in libSciT";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void PndSciTContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the Hyp library.*/

  FairContainer *p = new FairContainer("PndGeoSciTPar", "tof Geometry Parameters", "TestDefaultContext");
  p->addContext("TestNonDefaultContext");

  containers->Add(p);
}

FairParSet *PndSciTContFact::createContainer(FairContainer *c)
{
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char *name = c->GetName();
  FairParSet *p = nullptr;
  if (strcmp(name, "PndGeoSciTPar") == 0) {
    p = new PndGeoSciTPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
  }
  return p;
}
