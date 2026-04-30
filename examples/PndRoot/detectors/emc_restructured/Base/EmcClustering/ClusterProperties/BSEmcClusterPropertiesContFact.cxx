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

#include "BSEmcClusterPropertiesContFact.h"

#include <string.h>

#include "TList.h"
#include "TString.h"

#include "FairRuntimeDb.h"
#include "fairlogger/Logger.h"

#include "BSEmcClusterPropertiesPar.h"

class FairParSet;

ClassImp(BSEmcClusterPropertiesContFact);

static BSEmcClusterPropertiesContFact gBSEmcClusterPropertiesContFact;

BSEmcClusterPropertiesContFact::BSEmcClusterPropertiesContFact()
{
  // Constructor (called when the library is loaded)
  fName = "EmcClusterPropertiesContFact";
  fTitle = "Factory for parameter containers in libNewEmcBase";
  LOG(info) << "BSEmcClusterPropertiesContFact::BSEmcClusterPropertiesContFact(): Setting all containers for " << fName << " with title: " << fTitle;
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void BSEmcClusterPropertiesContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the BSEmc library.*/

  // CLUSTERING ////////////////////////////////////////
  FairContainer *pclusterBa = new FairContainer("EmcClusterPropertiesParBarrel", "Emc Clustering Parameters Barrel", "TestDefaultContext");
  pclusterBa->addContext("TestNonDefaultContext");

  FairContainer *pclusterBWEC = new FairContainer("EmcClusterPropertiesParBwEndcap", "Emc Clustering Parameters BWEC", "TestDefaultContext");
  pclusterBWEC->addContext("TestNonDefaultContext");

  FairContainer *pclusterFWEC = new FairContainer("EmcClusterPropertiesParFwEndcap", "Emc Clustering Parameters FWEC", "TestDefaultContext");
  pclusterFWEC->addContext("TestNonDefaultContext");

  FairContainer *pclusterShashlik = new FairContainer("EmcClusterPropertiesParShashlik", "Emc Clustering Parameters Shashlik", "TestDefaultContext");
  pclusterShashlik->addContext("TestNonDefaultContext");

  containers->Add(pclusterBa);
  containers->Add(pclusterBWEC);
  containers->Add(pclusterFWEC);
  containers->Add(pclusterShashlik);
}

FairParSet *BSEmcClusterPropertiesContFact::createContainer(FairContainer *t_c)
{
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char *name = t_c->GetName();
  LOG(debug) << " -I container name " << name;
  FairParSet *p = nullptr;

  // CLUSTERING ////////////////////////////////////////
  if (strcmp(name, "EmcClusterPropertiesParFwEndcap") == 0) {
    p = new BSEmcClusterPropertiesPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
  }
  if (strcmp(name, "EmcClusterPropertiesParBwEndcap") == 0) {
    p = new BSEmcClusterPropertiesPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
  }
  if (strcmp(name, "EmcClusterPropertiesParBarrel") == 0) {
    p = new BSEmcClusterPropertiesPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
  }
  if (strcmp(name, "EmcClusterPropertiesParShashlik") == 0) {
    p = new BSEmcClusterPropertiesPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
  }

  return p;
}
