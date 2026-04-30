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
//  PndHypContFact
//
//  Factory for the parameter containers in libHyp
//
//
//
/////////////////////////////////////////////////////////////

#include "PndHypContFact.h"
#include "FairRuntimeDb.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"
//#include "PndGeoHypPar.h"
#include "PndHypDigiPar.h"
#include "PndHypStripDigiPar.h"
#include <iostream>
#include <iomanip>

ClassImp(PndHypContFact)

  static PndHypContFact gPndHypContFact;

PndHypContFact::PndHypContFact()
{
  // Constructor (called when the library is loaded)
  fName = "PndHypContFact";
  fTitle = "Factory for parameter containers in libHyp";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void PndHypContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the Hyp library.*/

  /* FairContainer* p= new FairContainer("PndGeoHypPar",
                                          "Hyp Geometry Parameters",
                                          "TestDefaultContext");
    p->addContext("TestNonDefaultContext");

    containers->Add(p);
  */

  FairContainer *p2 = new FairContainer("PndHypDigiPar", "Hyp Pixel Digitization Parameters", "TestDefaultContext");
  p2->addContext("TestNonDefaultContext");
  containers->Add(p2);

  FairContainer *p3 = new FairContainer("PndHypStripDigiPar", "Hyp Strip Digitization Parameters", "TestDefaultContext");
  p3->addContext("TestNonDefaultContext");
  containers->Add(p3);
}

FairParSet *PndHypContFact::createContainer(FairContainer *c)
{
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char *name = c->GetName();
  FairParSet *p = nullptr;

  /*  if (strcmp(name,"PndGeoHypPar")==0) {
    p=new PndGeoHypPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  */

  if (strcmp(name, "PndHypDigiPar") == 0) {
    p = new PndHypDigiPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
  }

  if (strcmp(name, "PndHypStripDigiPar") == 0) {
    p = new PndHypStripDigiPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
  }

  return p;
}
