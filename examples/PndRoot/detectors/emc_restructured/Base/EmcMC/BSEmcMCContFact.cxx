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

#include "BSEmcMCContFact.h"

#include <string.h>

#include "TList.h"
#include "TString.h"

#include "FairRuntimeDb.h"
#include "fairlogger/Logger.h"

#include "BSEmcCalibrationPar.h"
#include "BSEmcDigiPar.h"
#include "BSEmcMCPar.h"

class FairParSet;

ClassImp(BSEmcMCContFact);

static BSEmcMCContFact gBSEmcMCContFact;

BSEmcMCContFact::BSEmcMCContFact()
{
  // Constructor (called when the library is loaded)
  fName = "EmcMCContFact";
  fTitle = "Factory for parameter containers in libNewEmcBase";
  LOG(info) << "BSEmcMCContFact::BSEmcMCContFact(): Setting all containers for " << fName << " with title: " << fTitle;
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void BSEmcMCContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the BSEmc library.*/

  // HIT ////////////////////////////////////////
  FairContainer *p1fw = new FairContainer("EmcMCParFwEndcap", "Emc Hit Parameters FWEndcap", "TestDefaultContext");
  p1fw->addContext("TestNonDefaultContext");

  FairContainer *p1bw = new FairContainer("EmcMCParBwEndcap", "Emc Hit Parameters BwEndcap", "TestDefaultContext");
  p1bw->addContext("TestNonDefaultContext");

  FairContainer *p1ba = new FairContainer("EmcMCParBarrel", "Emc Hit Parameters Barrel", "TestDefaultContext");
  p1ba->addContext("TestNonDefaultContext");

  FairContainer *p1sh = new FairContainer("EmcMCParShashlik", "Emc Hit Parameters Shashlik", "TestDefaultContext");
  p1sh->addContext("TestNonDefaultContext");

  containers->Add(p1fw);
  containers->Add(p1bw);
  containers->Add(p1ba);
  containers->Add(p1sh);

  // DIGI ////////////////////////////////////////
  FairContainer *pdigiBaAPD = new FairContainer("EmcDigiParBarrel", "Emc Digitization Parameters Barrel", "TestDefaultContext");
  pdigiBaAPD->addContext("TestNonDefaultContext");

  FairContainer *pBWECAPD = new FairContainer("EmcDigiParBwEndcap", "Emc Digitization Parameters BWEC", "TestDefaultContext");
  pBWECAPD->addContext("TestNonDefaultContext");

  FairContainer *pFWECAPD = new FairContainer("EmcDigiParFwEndcapAPD", "Emc Digitization Parameters FWEC APD", "TestDefaultContext");
  pFWECAPD->addContext("TestNonDefaultContext");

  FairContainer *pFWECVPT = new FairContainer("EmcDigiParFwEndcapVPT", "Emc Digitization Parameters FWEC VPT", "TestDefaultContext");
  pFWECVPT->addContext("TestNonDefaultContext");

  FairContainer *pShashlik = new FairContainer("EmcDigiParShashlik", "Emc Digitization Parameters Shashlik", "TestDefaultContext");
  pShashlik->addContext("TestNonDefaultContext");

  containers->Add(pdigiBaAPD);
  containers->Add(pBWECAPD);
  containers->Add(pFWECAPD);
  containers->Add(pFWECVPT);
  containers->Add(pShashlik);

  // Calibration ////////////////////////////////////////
  FairContainer *pcalibBa = new FairContainer("EmcCalibrationParBarrel", "Emc Calibration Parameters Barrel", "TestDefaultContext");
  pcalibBa->addContext("TestNonDefaultContext");

  FairContainer *pcalibBWEC = new FairContainer("EmcCalibrationParBwEndcap", "Emc Calibration Parameters BWEC", "TestDefaultContext");
  pcalibBWEC->addContext("TestNonDefaultContext");

  FairContainer *pcalibFWEC = new FairContainer("EmcCalibrationParFwEndcap", "Emc Calibration Parameters FWEC", "TestDefaultContext");
  pcalibFWEC->addContext("TestNonDefaultContext");

  FairContainer *pcalibShashlik = new FairContainer("EmcCalibrationParShashlik", "Emc Calibration Parameters Shashlik", "TestDefaultContext");
  pcalibShashlik->addContext("TestNonDefaultContext");

  containers->Add(pcalibBa);
  containers->Add(pcalibBWEC);
  containers->Add(pcalibFWEC);
  containers->Add(pcalibShashlik);
}

FairParSet *BSEmcMCContFact::createContainer(FairContainer *t_c)
{
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char *name = t_c->GetName();
  LOG(debug) << " -I container name " << name;
  FairParSet *p = nullptr;

  // HIT ////////////////////////////////////////
  if (strcmp(name, "EmcMCParFwEndcap") == 0) {
    p = new BSEmcMCPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
  }
  if (strcmp(name, "EmcMCParBwEndcap") == 0) {
    p = new BSEmcMCPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
  }
  if (strcmp(name, "EmcMCParBarrel") == 0) {
    p = new BSEmcMCPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
  }
  if (strcmp(name, "EmcMCParShashlik") == 0) {
    p = new BSEmcMCPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
  }

  // DIGI ////////////////////////////////////////
  if (strcmp(name, "EmcDigiParBarrel") == 0) {
    p = new BSEmcDigiPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
  }

  if (strcmp(name, "EmcDigiParBwEndcap") == 0) {
    p = new BSEmcDigiPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
  }

  if (strcmp(name, "EmcDigiParFwEndcapAPD") == 0) {
    p = new BSEmcDigiPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
  }

  if (strcmp(name, "EmcDigiParFwEndcapVPT") == 0) {
    p = new BSEmcDigiPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
  }

  if (strcmp(name, "EmcDigiParShashlik") == 0) {
    p = new BSEmcDigiPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
  }

  // CALIBRATION ////////////////////////////////////////
  if (strcmp(name, "EmcCalibrationParFwEndcap") == 0) {
    p = new BSEmcCalibrationPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
  }
  if (strcmp(name, "EmcCalibrationParBwEndcap") == 0) {
    p = new BSEmcCalibrationPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
  }
  if (strcmp(name, "EmcCalibrationParBarrel") == 0) {
    p = new BSEmcCalibrationPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
  }
  if (strcmp(name, "EmcCalibrationParShashlik") == 0) {
    p = new BSEmcCalibrationPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
  }

  return p;
}
