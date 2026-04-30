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

#include "BSEmcShashlykDigiContFact.h"

#include <string.h>

#include "TList.h"
#include "TString.h"

#include "FairRuntimeDb.h"
#include "fairlogger/Logger.h"

#include "BSEmcShashlykDigiPar.h"

class FairParSet;

ClassImp(BSEmcShashlykDigiContFact);

static BSEmcShashlykDigiContFact gBarrelContFact;

BSEmcShashlykDigiContFact::BSEmcShashlykDigiContFact() : FairContFact()
{
  // Constructor (called when the library is loaded)
  fName = "BSEmcShashlykDigiContFact";
  fTitle = "Factory for parameter containers in ShashlykTimebasedDigi";
  LOG(info) << "BSEmcShashlykDigiContFact::BSEmcShashlykDigiContFact(): Setting all containers for " << fName << " with title: " << fTitle;

  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void BSEmcShashlykDigiContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the Emc library.*/
  std::string parname = BSEmcShashlykDigiPar::fgParameterName;
  FairContainer *p1 = new FairContainer(parname.c_str(), "Emc Shashlyk Waveform digi Parameters", "TestDefaultContext");
  p1->addContext("TestNonDefaultContext");

  containers->Add(p1);
}

FairParSet *BSEmcShashlykDigiContFact::createContainer(FairContainer *t_c)
{
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char *name = t_c->GetName();
  LOG(debug) << "BSEmcShashlykDigiContFact::createContainer container name " << name;
  FairParSet *p = nullptr;
  std::string parname = BSEmcShashlykDigiPar::fgParameterName;
  if (strcmp(name, parname.c_str()) == 0) {
    p = new BSEmcShashlykDigiPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
  }

  return p;
}
