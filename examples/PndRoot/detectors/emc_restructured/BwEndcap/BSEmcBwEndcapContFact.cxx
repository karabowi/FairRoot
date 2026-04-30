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

#include "BSEmcBwEndcapContFact.h"

#include <string.h>

#include "TList.h"
#include "TString.h"

#include "FairRuntimeDb.h"
#include "fairlogger/Logger.h"

#include "BSEmcBwEndcapDigiPar.h"

class FairParSet;

ClassImp(BSEmcBwEndcapContFact);

static BSEmcBwEndcapContFact gBSEmcBwEndcapContFact;

BSEmcBwEndcapContFact::BSEmcBwEndcapContFact()
{
  // Constructor (called when the library is loaded)
  fName = "BSEmcBwEndcapContFact";
  fTitle = "Factory for parameter containers in libNewEmcBwEndcap";
  LOG(debug) << "BSEmcBwEndcapContFact::BSEmcBwEndcapContFact(): Setting all containers for " << fName << " with title: " << fTitle;
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void BSEmcBwEndcapContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the BSEmc library.*/

  // Neighbouring ////////////////////////////////////////
  TString name = "EmcBWEndcapDigiPar";
  FairContainer *digiPar = new FairContainer(name, "Timebased digi par for Emc BWEndcap", "TestDefaultContext");
  digiPar->addContext("TestDefaultContext");
  containers->Add(digiPar);
  LOG(debug) << "BSEmcBwEndcapContFact::setAllContainers: set container " << name;
}

FairParSet *BSEmcBwEndcapContFact::createContainer(FairContainer *t_c)
{
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char *name = t_c->GetName();
  LOG(debug) << " -I container name " << name;
  FairParSet *p = nullptr;
  // Neighbouring ////////////////////////////////////////
  TString bwecname = "EmcBWEndcapDigiPar";
  if (strcmp(name, bwecname.Data()) == 0) {
    p = new BSEmcBwEndcapDigiPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
    LOG(debug) << "Created BSEmcBwEndcapDigiPar - " << t_c->getConcatName().Data() << " - ";
  }
  return p;
}
