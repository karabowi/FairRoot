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
//  PndSensorNameContFact
//
//  Factory for the parameter containers in libPndMvd
//
/////////////////////////////////////////////////////////////

#include "PndSensorNameContFact.h"
#include "FairRuntimeDb.h"
#include "PndSensorNamePar.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"
#include "TList.h"
#include "TObjString.h"
#include <iostream>
#include <iomanip>
#include "FairLogger.h"

ClassImp(PndSensorNameContFact);

static PndSensorNameContFact gPndSensorNameContFact;

PndSensorNameContFact::PndSensorNameContFact() : fSensorParNames(new TList()), fContainerNames()
{
  // Constructor (called when the library is loaded)
  fName = "PndSensorNameContFact";
  fTitle = "Factory for parameter containers of the PndGeoHandler";
  // fSensorParNames = new TList();
  fContainerNames.push_back("PndSensorNamePar");
  LOG(info) << "PndSensorNameContFact::PndSensorNameContFact()";

  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}
PndSensorNameContFact::~PndSensorNameContFact()
{
  if (0 != fSensorParNames) {
    fSensorParNames->Delete();
    delete fSensorParNames;
  }
}

void PndSensorNameContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the MVD library.*/
  for (unsigned int i = 0; i < fContainerNames.size(); i++) {
    std::string description = "Match between GeoManager path and SensorId";
    FairContainer *p = new FairContainer(fContainerNames[i].c_str(), description.c_str(), "TestDefaultContext");
    fSensorParNames->Add(new TObjString(p->getConcatName()));
    containers->Add(p);
    LOG(info) << "PndSensorNameContFact::setAllContainers(): "
              << "Added " << fContainerNames[i] << ", " << description << " with context: "
              << "TestDefaultContext";
  }
}

FairParSet *PndSensorNameContFact::createContainer(FairContainer *c)
{
  LOG(info) << "PndSensorNameContFact::createContainer()";
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char *name = c->GetName();
  FairParSet *p = nullptr;

  for (unsigned int i = 0; i < fContainerNames.size(); i++) {
    if (strcmp(name, fContainerNames[i].c_str()) == 0) {
      p = new PndSensorNamePar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
      LOG(info) << "Created PndSensorNamePar - " << fContainerNames[i] << " - ";
      return p;
    }
  }
  return p;
}
