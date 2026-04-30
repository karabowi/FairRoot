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
//  PndLmdContFact
//
//  Factory for the parameter containers in libPndLmd
//
/////////////////////////////////////////////////////////////

#include "PndLmdContFact.h"
#include "FairRuntimeDb.h"
//#include "PndMvdGeoPar.h"
#include "PndSdsPixelDigiPar.h"
#include "PndSdsStripDigiPar.h"
#include "PndSdsTotDigiPar.h"
#include "PndLmdAlignPar.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"
#include "TList.h"
#include "TObjString.h"
#include <iostream>
#include <iomanip>

ClassImp(PndLmdContFact);

static PndLmdContFact gPndLmdContFact;

PndLmdContFact::PndLmdContFact()
{
  // Constructor (called when the library is loaded)
  fName = "PndLmdContFact";
  fTitle = "Factory for parameter containers in libPndLmd";
  fDigiParNames = new TList();
  fAlignParNames = new TList();
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}
PndLmdContFact::~PndLmdContFact()
{
  if (0 != fDigiParNames) {
    fDigiParNames->Delete();
    delete fDigiParNames;
  }
  if (0 != fAlignParNames) {
    fAlignParNames->Delete();
    delete fAlignParNames;
  }
}

void PndLmdContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the MVD library.*/
  //  FairContainer* p= new FairContainer("PndMvdGeoPar","PndMvd Geometry Parameters","TestDefaultContext");
  //  p->addContext("TestNonDefaultContext");
  //  fDigiParNames->Add(new TObjString(p->GetName()));
  //  containers->Add(p);

  FairContainer *p2 = new FairContainer("LMDPixelDigiPar", "PndSds Pixel Digitization Parameters", "TestDefaultContext");
  p2->addContext("TestNonDefaultContext");
  fDigiParNames->Add(new TObjString(p2->GetName()));
  containers->Add(p2);

  /*
  FairContainer* p3 = new FairContainer("LMDStripDigiParRect", "PndSds Strip Digitization Parameters (rectangular sensors)", "TestDefaultContext");
  p3->addContext("TestNonDefaultContext");
  fDigiParNames->Add(new TObjString(p3->GetName()));
  containers->Add(p3);

  FairContainer* p4 = new FairContainer("LMDStripDigiParTrap", "PndSds Strip Digitization Parameters (trapezoid sensors)", "TestDefaultContext");
  p4->addContext("TestNonDefaultContext");
  fDigiParNames->Add(new TObjString(p4->GetName()));
  containers->Add(p4);
  */

  FairContainer *p5 = new FairContainer("LMDAlignPar", "PndLmd Alignment Parameters", "TestDefaultContext");
  p5->addContext("TestNonDefaultContext");
  fAlignParNames->Add(new TObjString(p5->GetName()));
  containers->Add(p5);
}

FairParSet *PndLmdContFact::createContainer(FairContainer *c)
{
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char *name = c->GetName();
  FairParSet *p = nullptr;
  //  if (strcmp(name,"PndMvdGeoPar")==0) {
  //    p=new PndMvdGeoPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  //  }

  if (strcmp(name, "LMDPixelDigiPar") == 0) {
    p = new PndSdsPixelDigiPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
  }

  /*
  if (strcmp(name,"LMDStripDigiParRect")==0) {
  p=new PndSdsStripDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }

  if (strcmp(name,"LMDStripDigiParTrap")==0) {
    p=new PndSdsStripDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  */

  if (strcmp(name, "LMDAlignPar") == 0) {
    p = new PndLmdAlignPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
  }
  return p;
}
