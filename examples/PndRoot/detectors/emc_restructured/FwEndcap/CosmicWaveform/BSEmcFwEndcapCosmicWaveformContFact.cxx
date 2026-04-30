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

#include "BSEmcFwEndcapCosmicWaveformContFact.h"

#include <string.h>
#include <string>

#include "TList.h"
#include "TString.h"

#include "FairRuntimeDb.h"
#include "fairlogger/Logger.h"

#include "BSEmcFwEndcapCosmicWaveformGenPar.h"

class FairParSet;

ClassImp(BSEmcFwEndcapCosmicWaveformContFact);

static BSEmcFwEndcapCosmicWaveformContFact gBSEmcFwEndcapCosmicWaveformContFact;

BSEmcFwEndcapCosmicWaveformContFact::BSEmcFwEndcapCosmicWaveformContFact()
{
  // Constructor (called when the library is loaded)
  fName = "BSEmcFwEndcapCosmicWaveformContFact";
  fTitle = "Factory for parameter containers in libNewEmcFwEndcap";
  LOG(info) << "BSEmcFwEndcapCosmicWaveformContFact::BSEmcFwEndcapCosmicWaveformContFact(): Setting all containers for " << fName << " with title: " << fTitle;
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void BSEmcFwEndcapCosmicWaveformContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the BSEmc library.*/

  FairContainer *cosmicwfgenAPDPar = new FairContainer(TString{BSEmcFwEndcapCosmicWaveformGenPar::fgParameterName + "APD"}.Data(),
                                                       "Cosmic Waveform generation par for Emc FwEndcap APD readout", "TestDefaultContext");
  cosmicwfgenAPDPar->addContext("TestDefaultContext");
  containers->Add(cosmicwfgenAPDPar);
  LOG(info) << "BSEmcFwEndcapCosmicWaveformContFact::setAllContainers: set container " << BSEmcFwEndcapCosmicWaveformGenPar::fgParameterName << "APD";

  FairContainer *cosmicwfgenVPTTPar = new FairContainer(TString{BSEmcFwEndcapCosmicWaveformGenPar::fgParameterName + "VPTT"}.Data(),
                                                        "Cosmic Waveform generation par for Emc FwEndcap VPTT readout", "TestDefaultContext");
  cosmicwfgenVPTTPar->addContext("TestDefaultContext");
  containers->Add(cosmicwfgenVPTTPar);
  LOG(info) << "BSEmcFwEndcapCosmicWaveformContFact::setAllContainers: set container " << BSEmcFwEndcapCosmicWaveformGenPar::fgParameterName << "VPTT";
}

FairParSet *BSEmcFwEndcapCosmicWaveformContFact::createContainer(FairContainer *t_c)
{
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char *name = t_c->GetName();
  LOG(info) << "BSEmcFwEndcapCosmicWaveformContFact::createContainer - Trying to create Container " << name;
  FairParSet *p = nullptr;

  TString cosmicwfgenAPD = BSEmcFwEndcapCosmicWaveformGenPar::fgParameterName + "APD";
  if (strcmp(name, cosmicwfgenAPD.Data()) == 0) {
    p = new BSEmcFwEndcapCosmicWaveformGenPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
    LOG(debug) << "Created EmcFWEndcapCosmicWaveformGenPar - " << t_c->getConcatName().Data() << " - ";
  }

  TString cosmicwfgenVPTT = BSEmcFwEndcapCosmicWaveformGenPar::fgParameterName + "VPTT";
  if (strcmp(name, cosmicwfgenVPTT.Data()) == 0) {
    p = new BSEmcFwEndcapCosmicWaveformGenPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
    LOG(debug) << "Created EmcFWEndcapCosmicWaveformGenPar - " << t_c->getConcatName().Data() << " - ";
  }
  return p;
}
