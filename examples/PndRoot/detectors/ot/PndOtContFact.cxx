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

////////////////////////////////////////////////////////////////////////////
// PndOtContFact source file
//
// Factory for the parameter containers in libOt
//
// authors: Radoslaw Karabowicz, GSI, 2024
//
// modified from PndOtContFact by Nafija Ibrišimović in 2023
////////////////////////////////////////////////////////////////////////////

#include "PndOtContFact.h"
// from PandaRoot, this library
#include "PndGeoOtPar.h"
// from FairRoot
#include <FairRuntimeDb.h>
#include <FairParRootFileIo.h>
#include <FairParAsciiFileIo.h>
// standard
#include <iostream>
#include <iomanip>

ClassImp(PndOtContFact)

  static PndOtContFact gPndOtContFact;

PndOtContFact::PndOtContFact()
{
  // Constructor (called when the library is loaded)
  fName = "PndOtContFact";
  fTitle = "Factory for parameter containers in libOt";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void PndOtContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the OT library.*/
  FairContainer *p1 = new FairContainer("PndGeoOtPar", "Ot Geometry Parameters", "TestDefaultContext");
  p1->addContext("TestNonDefaultContext");
  containers->Add(p1);
}

FairParSet *PndOtContFact::createContainer(FairContainer *c)
{
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char *name = c->GetName();
  std::cout << "[OT] PndOtContFact createContainer name " << name << std::endl;
  FairParSet *p = 0;

  if (strcmp(name, "PndGeoOtPar") == 0) {
    p = new PndGeoOtPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
  }
  return p;
}
