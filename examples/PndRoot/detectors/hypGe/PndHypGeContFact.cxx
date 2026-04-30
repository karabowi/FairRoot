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
//  PndHypGeContFact
//
//  Factory for the parameter containers in libHypGe
//

//
/////////////////////////////////////////////////////////////

using namespace std;
#include "PndHypGeContFact.h"
#include "FairRuntimeDb.h"
//#include "PndGeoHypGePar.h"
#include <iostream>
#include <iomanip>

ClassImp(PndHypGeContFact)

  static PndHypGeContFact gPndHypGeContFact;

PndHypGeContFact::PndHypGeContFact()
{
  // Constructor (called when the library is loaded)
  fName = "PndHypGeContFact";
  fTitle = "Factory for parameter containers in libHypGe";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void PndHypGeContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the HypGe library.*/

  // FairContainer* p= new FairContainer("PndGeoHypGePar",
  //                                     "HypGe Geometry Parameters",
  //                                     "TestDefaultContext");
  // p->addContext("TestNonDefaultContext");

  //  containers->Add(p);
}

FairParSet *PndHypGeContFact::createContainer(FairContainer *c)
{
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char *name = c->GetName();
  FairParSet *p = nullptr;
  // if (strcmp(name,"PndGeoHypGePar")==0) {
  //  p=new PndGeoHypGePar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  //}
  return p;
}
