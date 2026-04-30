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

#include "BSEmcPositionContFact.h"

#include <string.h>
#include <string>

#include "TList.h"
#include "TString.h"

#include "FairRuntimeDb.h"
#include "fairlogger/Logger.h"

#include "BSEmcCrystalPositionPar.h"
#include "BSEmcGeoNeighbouringRelationPar.h"
#include "BSEmcPositionPar.h"

class FairParSet;

ClassImp(BSEmcPositionContFact);

static BSEmcPositionContFact gBSEmcPositionContFact;

BSEmcPositionContFact::BSEmcPositionContFact()
{
  // Constructor (called when the library is loaded)
  fName = "EmcPositionContFact";
  fTitle = "Factory for parameter containers in EmcGeometry";
  LOG(debug) << "BSEmcPositionContFact::BSEmcPositionContFact(): Setting all containers for " << fName << " with title: " << fTitle;
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void BSEmcPositionContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the BSEmc library.*/

  // Neighbouring ////////////////////////////////////////
  TString barrelname = BSEmcGeoNeighbouringRelationPar::fgParameterName + "Barrel";
  FairContainer *barrelNeighbouringPar = new FairContainer(barrelname, "Neighbouring relation of Emc Barrel crystals", "TestDefaultContext");
  barrelNeighbouringPar->addContext("TestDefaultContext");
  containers->Add(barrelNeighbouringPar);

  TString fwendcapname = BSEmcGeoNeighbouringRelationPar::fgParameterName + "FwEndcap";
  FairContainer *fwendcapNeighbouringPar = new FairContainer(fwendcapname, "Neighbouring relation of Emc FwEndcap crystals", "TestDefaultContext");
  fwendcapNeighbouringPar->addContext("TestDefaultContext");
  containers->Add(fwendcapNeighbouringPar);

  TString bwendcapname = BSEmcGeoNeighbouringRelationPar::fgParameterName + "BwEndcap";
  FairContainer *bwendcapNeighbouringPar = new FairContainer(bwendcapname, "Neighbouring relation of Emc BwEndcap crystals", "TestDefaultContext");
  bwendcapNeighbouringPar->addContext("TestDefaultContext");
  containers->Add(bwendcapNeighbouringPar);

  TString shashlikname = BSEmcGeoNeighbouringRelationPar::fgParameterName + "Shashlik";
  FairContainer *shashlikNeighbouringPar = new FairContainer(shashlikname, "Neighbouring relation of Emc Shashlik crystals", "TestDefaultContext");
  shashlikNeighbouringPar->addContext("TestDefaultContext");
  containers->Add(shashlikNeighbouringPar);
  // CRYSTALPOSITION ////////////////////////////////////////
  FairContainer *posfw = new FairContainer("EmcCrystalPositionParFwEndcap", "Emc CrystalPosition Parameters FWEndcap", "TestDefaultContext");
  posfw->addContext("TestNonDefaultContext");

  FairContainer *posbw = new FairContainer("EmcCrystalPositionParBwEndcap", "Emc CrystalPosition Parameters BwEndcap", "TestDefaultContext");
  posbw->addContext("TestNonDefaultContext");

  FairContainer *posba = new FairContainer("EmcCrystalPositionParBarrel", "Emc CrystalPosition Parameters Barrel", "TestDefaultContext");
  posba->addContext("TestNonDefaultContext");

  FairContainer *possh = new FairContainer("EmcCrystalPositionParShashlik", "Emc BranchName Parameters Shashlik", "TestDefaultContext");
  possh->addContext("TestNonDefaultContext");

  containers->Add(posfw);
  containers->Add(posbw);
  containers->Add(posba);
  containers->Add(possh);
  // HIT ////////////////////////////////////////
  FairContainer *p1fw = new FairContainer("EmcPositionParFwEndcap", "Emc Hit Parameters FWEndcap", "TestDefaultContext");
  p1fw->addContext("TestNonDefaultContext");

  FairContainer *p1bw = new FairContainer("EmcPositionParBwEndcap", "Emc Hit Parameters BwEndcap", "TestDefaultContext");
  p1bw->addContext("TestNonDefaultContext");

  FairContainer *p1ba = new FairContainer("EmcPositionParBarrel", "Emc Hit Parameters Barrel", "TestDefaultContext");
  p1ba->addContext("TestNonDefaultContext");

  FairContainer *p1sh = new FairContainer("EmcPositionParShashlik", "Emc Hit Parameters Shashlik", "TestDefaultContext");
  p1sh->addContext("TestNonDefaultContext");

  containers->Add(p1fw);
  containers->Add(p1bw);
  containers->Add(p1ba);
  containers->Add(p1sh);
}

FairParSet *BSEmcPositionContFact::createContainer(FairContainer *t_c)
{
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char *name = t_c->GetName();
  LOG(debug) << " -I container name " << name;
  FairParSet *p = nullptr;

  // Neighbouring ////////////////////////////////////////
  TString barrelname = BSEmcGeoNeighbouringRelationPar::fgParameterName + "Barrel";
  if (strcmp(name, barrelname.Data()) == 0) {
    p = new BSEmcGeoNeighbouringRelationPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
    LOG(debug) << "Created BSEmcGeoNeighbouringRelationPar - " << t_c->getConcatName().Data() << " - ";
  }

  TString fwendcapname = BSEmcGeoNeighbouringRelationPar::fgParameterName + "FwEndcap";
  if (strcmp(name, fwendcapname.Data()) == 0) {
    p = new BSEmcGeoNeighbouringRelationPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
    LOG(debug) << "Created BSEmcGeoNeighbouringRelationPar - " << t_c->getConcatName().Data() << " - ";
  }

  TString bwendcapname = BSEmcGeoNeighbouringRelationPar::fgParameterName + "BwEndcap";
  if (strcmp(name, bwendcapname.Data()) == 0) {
    p = new BSEmcGeoNeighbouringRelationPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
    LOG(debug) << "Created BSEmcGeoNeighbouringRelationPar - " << t_c->getConcatName().Data() << " - ";
  }

  TString shashlikname = BSEmcGeoNeighbouringRelationPar::fgParameterName + "Shashlik";
  if (strcmp(name, shashlikname.Data()) == 0) {
    p = new BSEmcGeoNeighbouringRelationPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
    LOG(debug) << "Created BSEmcGeoNeighbouringRelationPar - " << t_c->getConcatName().Data() << " - ";
  }
  // CRYSTALPOSITION ////////////////////////////////////////
  if (strcmp(name, "EmcCrystalPositionParFwEndcap") == 0) {
    p = new BSEmcCrystalPositionPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
  }
  if (strcmp(name, "EmcCrystalPositionParBwEndcap") == 0) {
    p = new BSEmcCrystalPositionPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
  }
  if (strcmp(name, "EmcCrystalPositionParBarrel") == 0) {
    p = new BSEmcCrystalPositionPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
  }
  if (strcmp(name, "EmcCrystalPositionParShashlik") == 0) {
    p = new BSEmcCrystalPositionPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
  }
  // HIT ////////////////////////////////////////
  if (strcmp(name, "EmcPositionParFwEndcap") == 0) {
    p = new BSEmcPositionPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
  }
  if (strcmp(name, "EmcPositionParBwEndcap") == 0) {
    p = new BSEmcPositionPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
  }
  if (strcmp(name, "EmcPositionParBarrel") == 0) {
    p = new BSEmcPositionPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
  }
  if (strcmp(name, "EmcPositionParShashlik") == 0) {
    p = new BSEmcPositionPar(t_c->getConcatName().Data(), t_c->GetTitle(), t_c->getContext());
  }

  return p;
}
