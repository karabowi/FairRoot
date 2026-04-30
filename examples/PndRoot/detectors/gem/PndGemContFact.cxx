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
//  PndGemContFact
//
//  Factory for the parameter containers in libPndGem
//
/////////////////////////////////////////////////////////////

#include "PndGemContFact.h"
#include "FairRuntimeDb.h"
#include "PndGemGeoPar.h"
#include "PndGemDigiPar.h"

static PndGemContFact gPndGemContFact;

ClassImp(PndGemContFact);

PndGemContFact::PndGemContFact()
{
  // Constructor (called when the library is loaded)
  fName = "PndGemContFact";
  fTitle = "Factory for parameter containers in libPndGem";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void PndGemContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the GEM library.*/

  FairContainer *p = new FairContainer("PndGemGeoPar", "PndGem Geometry Parameters", "TestDefaultContext");
  p->addContext("TestNonDefaultContext");

  containers->Add(p);

  FairContainer *p2 = new FairContainer("PndGemDetectors", "PndGem Digitization Parameters", "TestDefaultContext");
  p2->addContext("TestNonDefaultContext");
  containers->Add(p2);

  //     FairContainer* p3 = new FairContainer("GEMStripDigiParRect", "PndGem Strip Digitization Parameters (rectangular sensors)", "TestDefaultContext");
  //     p3->addContext("TestNonDefaultContext");
  //     containers->Add(p3);

  //     FairContainer* p4 = new FairContainer("GEMStripDigiParTrap", "PndGem Strip Digitization Parameters (trapezoid sensors)", "TestDefaultContext");
  //     p4->addContext("TestNonDefaultContext");
  //     containers->Add(p4);
}

FairParSet *PndGemContFact::createContainer(FairContainer *c)
{
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char *name = c->GetName();
  FairParSet *p = nullptr;

  if (strcmp(name, "PndGemGeoPar") == 0) {
    p = new PndGemGeoPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
  }

  if (strcmp(name, "PndGemDetectors") == 0) {
    p = new PndGemDigiPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
  }

  //   if (strcmp(name,"GEMStripDigiParRect")==0) {
  //   p=new PndGemStripDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  //   }

  //   if (strcmp(name,"GEMStripDigiParTrap")==0) {
  //   p=new PndGemStripDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  //   }

  return p;
}
