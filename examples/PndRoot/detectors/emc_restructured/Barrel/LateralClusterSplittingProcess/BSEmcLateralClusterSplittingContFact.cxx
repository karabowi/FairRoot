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

#include "BSEmcLateralClusterSplittingContFact.h"

#include <string.h>

#include "TList.h"
#include "TString.h"

#include "FairRuntimeDb.h"
#include "fairlogger/Logger.h"

#include "BSEmcLateralClusterSplittingPar.h"

class FairParSet;

ClassImp(BSEmcLateralClusterSplittingContFact);

static BSEmcLateralClusterSplittingContFact gEmcLateralClusterSplittingContFact;

BSEmcLateralClusterSplittingContFact::BSEmcLateralClusterSplittingContFact() : FairContFact()
{
  // Constructor (called when the library is loaded)
  fName = "BSEmcLateralClusterSplittingContFact";
  fTitle = "Factory for parameter containers in libLateralClusterSplittingProcess";
  LOG(info) << "BSEmcLateralClusterSplittingContFact::BSEmcLateralClusterSplittingContFact(): Setting all containers for " << fName << " with title: " << fTitle;

  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void BSEmcLateralClusterSplittingContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the Emc library.*/
  std::string parname = BSEmcLateralClusterSplittingPar::fgParameterName + "Barrel";
  FairContainer *p1 = new FairContainer(parname.c_str(), "Emc Lateral Cluster Splitting Parameters", "TestDefaultContext");
  p1->addContext("TestNonDefaultContext");

  containers->Add(p1);
}

FairParSet *BSEmcLateralClusterSplittingContFact::createContainer(FairContainer *t_c)
{
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char *name = t_c->GetName();
  LOG(debug) << "BSEmcLateralClusterSplittingContFact::createContainer container name " << name;
  FairParSet *p = nullptr;
  std::string parname = BSEmcLateralClusterSplittingPar::fgParameterName + "Barrel";
  if (strcmp(name, parname.c_str()) == 0) {
    p = new BSEmcLateralClusterSplittingPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
  }

  return p;
}
