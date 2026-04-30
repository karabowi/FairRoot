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
//  PndMvdContFact
//
//  Factory for the parameter containers in libPndMvd
//
/////////////////////////////////////////////////////////////

#include "PndMvdContFact.h"
#include "FairRuntimeDb.h"
//#include "PndMvdGeoPar.h"
#include "PndSdsPixelDigiPar.h"
#include "PndSdsStripDigiPar.h"
#include "PndSdsTotDigiPar.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"
#include "TList.h"
#include "TObjString.h"
#include <iostream>
#include <iomanip>

ClassImp(PndMvdContFact);

static PndMvdContFact gPndMvdContFact;

PndMvdContFact::PndMvdContFact() : fDigiParNames(new TList())
{
  // Constructor (called when the library is loaded)
  fName = "PndMvdContFact";
  fTitle = "Factory for parameter containers in libPndMvd";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}
PndMvdContFact::~PndMvdContFact()
{
  if (0 != fDigiParNames) {
    fDigiParNames->Delete();
    delete fDigiParNames;
  }
}

void PndMvdContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the MVD library.*/
  //  FairContainer* p= new FairContainer("PndMvdGeoPar","PndMvd Geometry Parameters","TestDefaultContext");
  //  p->addContext("TestNonDefaultContext");
  //  fDigiParNames->Add(new TObjString(p->GetName()));
  //  containers->Add(p);

  FairContainer *p2 = new FairContainer("MVDPixelDigiPar", "PndMvd Pixel Digitization Parameters", "TestDefaultContext");
  p2->addContext("TestNonDefaultContext");
  fDigiParNames->Add(new TObjString(p2->GetName()));
  containers->Add(p2);

  FairContainer *p3 = new FairContainer("MVDStripDigiParRect", "PndMvd Strip Digitization Parameters (rectangular sensors)", "TestDefaultContext");
  p3->addContext("TestNonDefaultContext");
  fDigiParNames->Add(new TObjString(p3->GetName()));
  containers->Add(p3);

  FairContainer *p4 = new FairContainer("MVDStripDigiParTrap", "PndMvd Strip Digitization Parameters (trapezoid sensors)", "TestDefaultContext");
  p4->addContext("TestNonDefaultContext");
  fDigiParNames->Add(new TObjString(p4->GetName()));
  containers->Add(p4);

  FairContainer *p5 = new FairContainer("MVDStripDigiParTD", "PndMvd Strip Digitization Parameters (rectangular double sided test sensors)", "TestDefaultContext");
  p5->addContext("TestNonDefaultContext");
  fDigiParNames->Add(new TObjString(p5->GetName()));
  containers->Add(p5);

  FairContainer *p6 = new FairContainer("MVDStripDigiParTS", "PndMvd Strip Digitization Parameters (rectangular single sided test sensors)", "TestDefaultContext");
  p6->addContext("TestNonDefaultContext");
  fDigiParNames->Add(new TObjString(p6->GetName()));
  containers->Add(p6);

  FairContainer *p7 = new FairContainer("MVDPixelTotDigiPar", "PndMvd Pixel Charge to Tot Digitization Parameters", "TestDefaultContext");
  p7->addContext("TestNonDefaultContext");
  fDigiParNames->Add(new TObjString(p7->GetName()));
  containers->Add(p7);

  FairContainer *p8 = new FairContainer("MVDStripTotDigiParRect", "PndMvd Strip Charge to Tot Digitization Parameters (rectangular sensors)", "TestDefaultContext");
  p8->addContext("TestNonDefaultContext");
  fDigiParNames->Add(new TObjString(p8->GetName()));
  containers->Add(p8);

  FairContainer *p9 = new FairContainer("MVDStripTotDigiParTrap", "PndMvd Strip Charge to Tot Digitization Parameters (trapezoid sensors)", "TestDefaultContext");
  p9->addContext("TestNonDefaultContext");
  fDigiParNames->Add(new TObjString(p9->GetName()));
  containers->Add(p9);

  FairContainer *p10 =
    new FairContainer("MVDStripTotDigiParTD", "PndMvd Strip Charge to Tot Digitization Parameters (rectangular double sided test sensors)", "TestDefaultContext");
  p10->addContext("TestNonDefaultContext");
  fDigiParNames->Add(new TObjString(p10->GetName()));
  containers->Add(p10);

  FairContainer *p11 =
    new FairContainer("MVDStripTotDigiParTS", "PndMvd Strip Charge to Tot Digitization Parameters (rectangular single sided test sensors)", "TestDefaultContext");
  p11->addContext("TestNonDefaultContext");
  fDigiParNames->Add(new TObjString(p11->GetName()));
  containers->Add(p11);
}

FairParSet *PndMvdContFact::createContainer(FairContainer *c)
{
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char *name = c->GetName();
  FairParSet *p = nullptr;
  //  if (strcmp(name,"PndMvdGeoPar")==0) {
  //    p=new PndMvdGeoPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  //  }

  if (strcmp(name, "MVDPixelDigiPar") == 0) {
    p = new PndSdsPixelDigiPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
  }

  if (strcmp(name, "MVDStripDigiParRect") == 0) {
    p = new PndSdsStripDigiPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
  }

  if (strcmp(name, "MVDStripDigiParTrap") == 0) {
    p = new PndSdsStripDigiPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
  }

  if (strcmp(name, "MVDStripDigiParTD") == 0) {
    p = new PndSdsStripDigiPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
  }

  if (strcmp(name, "MVDStripDigiParTS") == 0) {
    p = new PndSdsStripDigiPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
  }

  if (strcmp(name, "MVDPixelTotDigiPar") == 0) {
    p = new PndSdsTotDigiPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
  }

  if (strcmp(name, "MVDStripTotDigiParRect") == 0) {
    p = new PndSdsTotDigiPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
  }

  if (strcmp(name, "MVDStripTotDigiParTrap") == 0) {
    p = new PndSdsTotDigiPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
  }

  if (strcmp(name, "MVDStripTotDigiParTD") == 0) {
    p = new PndSdsTotDigiPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
  }

  if (strcmp(name, "MVDStripTotDigiParTS") == 0) {
    p = new PndSdsTotDigiPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
  }

  return p;
}
