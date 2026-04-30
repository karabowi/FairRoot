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

//*-- AUTHOR : Denis Bertini
//*-- Created : 20/06/2005

/////////////////////////////////////////////////////////////
//
//  PndSttContFact
//
//  Factory for the parameter containers in libStt
//
/////////////////////////////////////////////////////////////

#include "PndSttContFact.h"
#include "FairRuntimeDb.h"
#include "PndGeoSttPar.h"
#include "PndSttDigiPar.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"
#include "PndSttParRootFileIo.h"
#include "PndSttParAsciiFileIo.h"
#include <iostream>
#include <iomanip>

ClassImp(PndSttContFact)

  static PndSttContFact gPndSttContFact;

PndSttContFact::PndSttContFact()
{
  // Constructor (called when the library is loaded)
  fName = "PndSttContFact";
  fTitle = "Factory for parameter containers in libStt";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void PndSttContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the STT library.*/
  FairContainer *p2 = new FairContainer("PndGeoSttPar", "Stt Geometry Parameters", "TestDefaultContext");
  p2->addContext("TestNonDefaultContext");

  containers->Add(p2);
}

FairParSet *PndSttContFact::createContainer(FairContainer *c)
{
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char *name = c->GetName();
  std::cout << " -I container name " << name << std::endl;
  FairParSet *p = nullptr;

  if (strcmp(name, "PndGeoSttPar") == 0) {
    p = new PndGeoSttPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
  }
  return p;
}
/*
void  PndSttContFact::activateParIo(FairParIo* io) {
  // activates the input/output class for the parameters
  // needed by the Stt
  if (strcmp(io->IsA()->GetName(),"FairParRootFileIo")==0) {
    PndSttParRootFileIo* p=new PndSttParRootFileIo(((FairParRootFileIo*)io)->getParRootFile());
    io->setDetParIo(p);
  }
  if (strcmp(io->IsA()->GetName(),"FairParAsciiFileIo")==0) {
    PndSttParAsciiFileIo* p=new PndSttParAsciiFileIo(((FairParAsciiFileIo*)io)->getFile());
    io->setDetParIo(p);
  }
}

*/
