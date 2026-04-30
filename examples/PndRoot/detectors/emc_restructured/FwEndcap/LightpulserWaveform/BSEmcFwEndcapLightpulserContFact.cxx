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

#include "BSEmcFwEndcapLightpulserContFact.h"

#include <string.h>
#include <string>

#include "TList.h"
#include "TString.h"

#include "FairRuntimeDb.h"
#include "fairlogger/Logger.h"

#include "BSEmcFwEndcapWaveformGenPar.h"

class FairParSet;

ClassImp(BSEmcFwEndcapLightpulserContFact);

static BSEmcFwEndcapLightpulserContFact gBSEmcFwEndcapLightpulserContFact;

BSEmcFwEndcapLightpulserContFact::BSEmcFwEndcapLightpulserContFact()
{
  // Constructor (called when the library is loaded)
  fName = "BSEmcFwEndcapLightpulserContFact";
  fTitle = "Factory for parameter containers in libNewEmcFwEndcap";
  LOG(info) << "BSEmcFwEndcapLightpulserContFact::BSEmcFwEndcapLightpulserContFact(): Setting all containers for " << fName << " with title: " << fTitle;
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void BSEmcFwEndcapLightpulserContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the BSEmc library.*/
  FairContainer *wfgenPar = new FairContainer(BSEmcFwEndcapWaveformGenPar::fgParameterName.c_str(), "Waveform generation par for Emc FWEndcap", "TestDefaultContext");
  wfgenPar->addContext("TestDefaultContext");
  containers->Add(wfgenPar);
  LOG(info) << "BSEmcFwEndcapLightpulserContFact::setAllContainers: set container " << BSEmcFwEndcapWaveformGenPar::fgParameterName;
}

FairParSet *BSEmcFwEndcapLightpulserContFact::createContainer(FairContainer *t_c)
{
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char *name = t_c->GetName();
  LOG(info) << "BSEmcFwEndcapLightpulserContFact::createContainer - Trying to create Container " << name;
  FairParSet *p = nullptr;

  // Waveform generation ////////////////////////////////////////
  TString wfgen = BSEmcFwEndcapWaveformGenPar::fgParameterName;
  if (strcmp(name, wfgen.Data()) == 0) {
    p = new BSEmcFwEndcapWaveformGenPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
    LOG(debug) << "Created BSEmcFwEndcapWaveformGenPar - " << t_c->getConcatName().Data() << " - ";
  }
  return p;
}
