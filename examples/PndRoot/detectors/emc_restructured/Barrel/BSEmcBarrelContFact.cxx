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
//  PndEmcContFact
//
//  Factory for the parameter containers in libEmc
//
//  Created 14/08/06  by S.Spataro
//
/////////////////////////////////////////////////////////////

#include "BSEmcBarrelContFact.h"

#include <string.h>

#include "TList.h"
#include "TString.h"

#include "FairRuntimeDb.h"
#include "fairlogger/Logger.h"

#include "BSEmcBarrelDigiNonuniformityPar.h"

class FairParSet;

ClassImp(BSEmcBarrelContFact);

static BSEmcBarrelContFact gBarrelContFact;

BSEmcBarrelContFact::BSEmcBarrelContFact() : FairContFact()
{
  // Constructor (called when the library is loaded)
  fName = "BSEmcBarrelContFact";
  fTitle = "Factory for parameter containers in libEmc";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void BSEmcBarrelContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the Emc library.*/

  FairContainer *p1 = new FairContainer("BarrelDigiNonuniformityPar", "Emc Nonuniformity Parameters", "TestDefaultContext");
  p1->addContext("TestNonDefaultContext");

  containers->Add(p1);
}

FairParSet *BSEmcBarrelContFact::createContainer(FairContainer *t_c)
{
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char *name = t_c->GetName();
  LOG(debug) << "BSEmcBarrelContFact::createContainer container name " << name;
  FairParSet *p = nullptr;

  if (strcmp(name, "BarrelDigiNonuniformityPar") == 0) {
    p = new BSEmcBarrelDigiNonuniformityPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
  }

  return p;
}
