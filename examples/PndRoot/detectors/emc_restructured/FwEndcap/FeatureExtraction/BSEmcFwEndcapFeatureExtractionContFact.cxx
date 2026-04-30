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

#include "BSEmcFwEndcapFeatureExtractionContFact.h"

#include <string.h>
#include <string>

#include "TList.h"
#include "TString.h"

#include "FairRuntimeDb.h"
#include "fairlogger/Logger.h"

#include "BSEmcFwEndcapDigiPar.h"

class FairParSet;

ClassImp(BSEmcFwEndcapFeatureExtractionContFact);

static BSEmcFwEndcapFeatureExtractionContFact gBSEmcFwEndcapFeatureExtractionContFact;

BSEmcFwEndcapFeatureExtractionContFact::BSEmcFwEndcapFeatureExtractionContFact()
{
  // Constructor (called when the library is loaded)
  fName = "BSEmcFwEndcapFeatureExtractionContFact";
  fTitle = "Factory for parameter containers in libNewEmcFwEndcap";
  LOG(info) << "BSEmcFwEndcapFeatureExtractionContFact::BSEmcFwEndcapFeatureExtractionContFact(): Setting all containers for " << fName << " with title: " << fTitle;
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void BSEmcFwEndcapFeatureExtractionContFact::setAllContainers()
{

  FairContainer *digiPar = new FairContainer(BSEmcFwEndcapDigiPar::fgParameterName.c_str(), "Timebased digi par for Emc FWEndcap", "TestDefaultContext");
  digiPar->addContext("TestDefaultContext");
  containers->Add(digiPar);
  LOG(info) << "BSEmcFwEndcapFeatureExtractionContFact::setAllContainers: set container " << BSEmcFwEndcapDigiPar::fgParameterName;
}

FairParSet *BSEmcFwEndcapFeatureExtractionContFact::createContainer(FairContainer *t_c)
{
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char *name = t_c->GetName();
  LOG(info) << "BSEmcFwEndcapFeatureExtractionContFact::createContainer - Trying to create Container " << name;
  FairParSet *p = nullptr;
  // Waveformbased digitizing ////////////////////////////////////////
  TString digipar = BSEmcFwEndcapDigiPar::fgParameterName;
  if (strcmp(name, digipar.Data()) == 0) {
    p = new BSEmcFwEndcapDigiPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
    LOG(debug) << "Created BSEmcFwEndcapDigiPar - " << t_c->getConcatName().Data() << " - ";
  }

  return p;
}
