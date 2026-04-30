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
//  CbmHypContFact
//
//  Factory for the parameter containers in libHyp
//
//
//
/////////////////////////////////////////////////////////////

#include "PndFtofContFact.h"
#include "FairRuntimeDb.h"
#include "iostream"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"
#include "PndGeoFtofPar.h"
#include <iostream>
#include <iomanip>

ClassImp(PndFtofContFact)

  static PndFtofContFact gPndFtofContFact;

PndFtofContFact::PndFtofContFact()
{
  // Constructor (called when the library is loaded)
  fName = "PndFtofContFact";
  fTitle = "Factory for parameter containers in libFtof";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void PndFtofContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the Hyp library.*/

  FairContainer *p = new FairContainer("PndGeoFtofPar", "tof Geometry Parameters", "TestDefaultContext");
  p->addContext("TestNonDefaultContext");

  containers->Add(p);
}

FairParSet *PndFtofContFact::createContainer(FairContainer *c)
{
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char *name = c->GetName();
  FairParSet *p = nullptr;
  if (strcmp(name, "PndGeoFtofPar") == 0) {
    p = new PndGeoFtofPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
  }
  return p;
}
