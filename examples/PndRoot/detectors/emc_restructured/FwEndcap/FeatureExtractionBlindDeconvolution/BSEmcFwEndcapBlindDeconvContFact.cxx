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

#include "BSEmcFwEndcapBlindDeconvContFact.h"

#include <string.h>
#include <string>

#include "TList.h"
#include "TString.h"

#include "FairRuntimeDb.h"
#include "fairlogger/Logger.h"

#include "BSEmcFwEndcapBlindDeconvPar.h"

class FairParSet;

ClassImp(BSEmcFwEndcapBlindDeconvContFact);

static BSEmcFwEndcapBlindDeconvContFact gBSEmcFwEndcapBlindDeconvContFact;

BSEmcFwEndcapBlindDeconvContFact::BSEmcFwEndcapBlindDeconvContFact()
{
  // Constructor (called when the library is loaded)
  fName = "BSEmcFwEndcapBlindDeconvContFact";
  fTitle = "Factory for parameter containers in libNewEmcFwEndcap";
  LOG(info) << "BSEmcFwEndcapBlindDeconvContFact::BSEmcFwEndcapBlindDeconvContFact(): Setting all containers for " << fName << " with title: " << fTitle;
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void BSEmcFwEndcapBlindDeconvContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the BSEmc library.*/
  {
    TString parname = BSEmcFwEndcapBlindDeconvPar::fgParameterName + "APD";
    FairContainer *cosmicwfgenPar = new FairContainer(parname.Data(), "Blind deconvolution generation par for Emc FwEndcap APD readout", "TestDefaultContext");
    cosmicwfgenPar->addContext("TestDefaultContext");
    containers->Add(cosmicwfgenPar);
    LOG(info) << "BSEmcFwEndcapBlindDeconvContFact::setAllContainers: set container " << parname;
  }
  {
    TString parname = BSEmcFwEndcapBlindDeconvPar::fgParameterName + "VPTT";
    FairContainer *cosmicwfgenPar = new FairContainer(parname.Data(), "Blind deconvolution par for Emc FwEndcap VPTT readout", "TestDefaultContext");
    cosmicwfgenPar->addContext("TestDefaultContext");
    containers->Add(cosmicwfgenPar);
    LOG(info) << "BSEmcFwEndcapBlindDeconvContFact::setAllContainers: set container " << parname;
  }
}

FairParSet *BSEmcFwEndcapBlindDeconvContFact::createContainer(FairContainer *t_c)
{
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char *name = t_c->GetName();
  LOG(info) << "BSEmcFwEndcapBlindDeconvContFact::createContainer - Trying to create Container " << name;
  FairParSet *p = nullptr;

  TString parname = BSEmcFwEndcapBlindDeconvPar::fgParameterName + "APD";
  if (strcmp(name, parname.Data()) == 0) {
    p = new BSEmcFwEndcapBlindDeconvPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
    LOG(debug) << "Created BSEmcFwEndcapBlindDeconvPar - " << t_c->getConcatName().Data() << " - ";
  }

  parname = BSEmcFwEndcapBlindDeconvPar::fgParameterName + "VPTT";
  if (strcmp(name, parname.Data()) == 0) {
    p = new BSEmcFwEndcapBlindDeconvPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
    LOG(debug) << "Created BSEmcFwEndcapBlindDeconvPar - " << t_c->getConcatName().Data() << " - ";
  }
  return p;
}
