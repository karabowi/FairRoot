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

#include "BSEmcRecoContFact.h"

#include <string.h>

#include "TList.h"
#include "TString.h"

#include "FairRuntimeDb.h"
#include "fairlogger/Logger.h"

#include "BSEmcErrorCalculationPar.h"
#include "BSEmcSplitOffPar.h"

class FairParSet;

ClassImp(BSEmcRecoContFact);

static BSEmcRecoContFact gBSEmcRecoContFact;

BSEmcRecoContFact::BSEmcRecoContFact()
{
  // Constructor (called when the library is loaded)
  fName = "EmcRecoContFact";
  fTitle = "Factory for parameter containers in libNewEmcBase";
  LOG(info) << "BSEmcRecoContFact::BSEmcRecoContFact(): Setting all containers for " << fName << " with title: " << fTitle;
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void BSEmcRecoContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the BSEmc library.*/

  // ERRORCALCULATION ////////////////////////////////////////
  FairContainer *errorBarrel = new FairContainer("EmcErrorCalculationParBarrel", "Emc Error Calculation Parameters Barrel", "TestDefaultContext");
  errorBarrel->addContext("TestNonDefaultContext");

  FairContainer *errorBWEC = new FairContainer("EmcErrorCalculationParBwEndcap", "Emc Error Calculation Parameters BWEC", "TestDefaultContext");
  errorBWEC->addContext("TestNonDefaultContext");

  FairContainer *errorFWEC = new FairContainer("EmcErrorCalculationParFwEndcap", "Emc Error Calculation Parameters FWEC", "TestDefaultContext");
  errorFWEC->addContext("TestNonDefaultContext");

  FairContainer *errorShashlik = new FairContainer("EmcErrorCalculationParShashlik", "Emc Error Calculation Parameters Shashlik", "TestDefaultContext");
  errorShashlik->addContext("TestNonDefaultContext");

  // SPLITOFFRECOGNITION /////////////////////////////////////
  FairContainer *splitoffFWEC = new FairContainer("EmcSplitOffParFwEndcap", "Emc SplitOff Recognition Parameters FWEC", "TestDefaultContext");
  errorBarrel->addContext("TestNonDefaultContext");

  FairContainer *splitoffBWEC = new FairContainer("EmcSplitOffParBwEndcap", "Emc SplitOff Recognition Parameters BWEC", "TestDefaultContext");
  errorBWEC->addContext("TestNonDefaultContext");

  FairContainer *splitoffBarrel = new FairContainer("EmcSplitOffParBarrel", "Emc Error Calculation Parameters Barrel", "TestDefaultContext");
  errorFWEC->addContext("TestNonDefaultContext");

  containers->Add(errorBarrel);
  containers->Add(errorBWEC);
  containers->Add(errorFWEC);
  containers->Add(errorShashlik);
  containers->Add(splitoffFWEC);
  containers->Add(splitoffBWEC);
  containers->Add(splitoffBarrel);
}

FairParSet *BSEmcRecoContFact::createContainer(FairContainer *t_c)
{
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char *name = t_c->GetName();
  LOG(debug) << " -I container name " << name;
  FairParSet *p = nullptr;

  // ERRORCALCULATION ////////////////////////////////////////
  if (strcmp(name, "EmcErrorCalculationParFwEndcap") == 0) {
    p = new BSEmcErrorCalculationPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
  }
  if (strcmp(name, "EmcErrorCalculationParBwEndcap") == 0) {
    p = new BSEmcErrorCalculationPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
  }
  if (strcmp(name, "EmcErrorCalculationParBarrel") == 0) {
    p = new BSEmcErrorCalculationPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
  }
  if (strcmp(name, "EmcErrorCalculationParShashlik") == 0) {
    p = new BSEmcErrorCalculationPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
  }

  // SPLITOFFRECOGNITION /////////////////////////////////////
  if (strcmp(name, "EmcSplitOffParFwEndcap") == 0) {
    p = new BSEmcSplitOffPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
  }
  if (strcmp(name, "EmcSplitOffParBwEndcap") == 0) {
    p = new BSEmcSplitOffPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
  }
  if (strcmp(name, "EmcSplitOffParBarrel") == 0) {
    p = new BSEmcSplitOffPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
  }


  return p;
}
