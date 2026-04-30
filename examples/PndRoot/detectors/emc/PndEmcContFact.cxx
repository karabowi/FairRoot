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
//  PndEmcContFact
//
//  Factory for the parameter containers in libEmc
//
//  Created 14/08/06  by S.Spataro
//
/////////////////////////////////////////////////////////////

#include "PndEmcContFact.h"
#include "FairRuntimeDb.h"
#include "PndEmcGeoPar.h"
#include "PndEmcDigiPar.h"
#include "PndEmcFWEndcapDigiPar.h"
#include "PndEmcBWEndcapDigiPar.h"
#include "PndEmcShashlykDigiPar.h"
#include "PndEmcFpgaPar.h"
#include "PndEmcRecoPar.h"
#include "PndEmcDigiNonuniformityPar.h"
#include "PndEmcErrorMatrixPar.h"
#include "FairParAsciiFileIo.h"
#include "TClass.h"
#include <iostream>
#include <iomanip>

ClassImp(PndEmcContFact)

  static PndEmcContFact gPndEmcContFact;

PndEmcContFact::PndEmcContFact()
{
  // Constructor (called when the library is loaded)
  fName = "PndEmcContFact";
  fTitle = "Factory for parameter containers in libEmc";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void PndEmcContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the Emc library.*/
	
	FairContainer* p1= new FairContainer("PndEmcDigiPar",
														"Emc Digitalization Parameters",
														"TestDefaultContext");
	p1->addContext("TestNonDefaultContext");
	
	FairContainer* p2= new FairContainer("PndEmcRecoPar",
														"Emc Reconstruction Parameters",
														"TestDefaultContext");
	p2->addContext("TestNonDefaultContext");
	
	FairContainer* p3= new FairContainer("PndEmcGeoPar",
														"Emc Geometry Parameters",
														"TestDefaultContext");
	p3->addContext("TestNonDefaultContext");
	
	FairContainer* p4= new FairContainer("PndEmcDigiNonuniformityPar",
														"Emc Nonuniformity Parameters",
														"TestDefaultContext");
	p4->addContext("TestNonDefaultContext");

	FairContainer* p5= new FairContainer("PndEmcErrorMatrixPar",
													 "Emc Error matrix Parameters",
													 "TestDefaultContext");
	p5->addContext("TestNonDefaultContext");
	
	FairContainer* p6= new FairContainer("PndEmcFpgaPar",
													 "Emc FPGA Parameters",
													 "TestDefaultContext");
	p6->addContext("TestNonDefaultContext");

	FairContainer* p7= new FairContainer("PndEmcFWEndcapDigiPar",
													 "Emc Parameters for forward endcap",
													 "TestDefaultContext");
	p7->addContext("TestNonDefaultContext");

	FairContainer* p8= new FairContainer("PndEmcBWEndcapDigiPar",
													 "Emc Parameters for backward endcap",
													 "TestDefaultContext");
	p8->addContext("TestNonDefaultContext");

	FairContainer* p9= new FairContainer("PndEmcShashlykDigiPar",
													 "Emc Parameters for shashlyk",
													 "TestDefaultContext");
	p9->addContext("TestNonDefaultContext");

	containers->Add(p1);
	containers->Add(p2);
	containers->Add(p3);
	containers->Add(p4);
	containers->Add(p5);
	containers->Add(p6);
	containers->Add(p7);
	containers->Add(p8);
	containers->Add(p9);
}

FairParSet *PndEmcContFact::createContainer(FairContainer *c)
{
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  const char *name = c->GetName();
  std::cout << " -I container name " << name << std::endl;
  FairParSet *p = nullptr;

  if (strcmp(name, "PndEmcDigiPar") == 0) {
    p = new PndEmcDigiPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
  }

  if (strcmp(name, "PndEmcRecoPar") == 0) {
    p = new PndEmcRecoPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
  }

  if (strcmp(name, "PndEmcGeoPar") == 0) {
    p = new PndEmcGeoPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
  }

  if (strcmp(name, "PndEmcDigiNonuniformityPar") == 0) {
    p = new PndEmcDigiNonuniformityPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
  }

  if (strcmp(name, "PndEmcErrorMatrixPar") == 0) {
    p = new PndEmcErrorMatrixPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
  }
  if (strcmp(name, "PndEmcFpgaPar") == 0) {
    p = new PndEmcFpgaPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
  }
  if (strcmp(name, "PndEmcFWEndcapDigiPar") == 0) {
    p = new PndEmcFWEndcapDigiPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
  }
  if (strcmp(name,"PndEmcBWEndcapDigiPar")==0) {
    p=new PndEmcBWEndcapDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  if (strcmp(name,"PndEmcShashlykDigiPar")==0) {
    p=new PndEmcShashlykDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}
