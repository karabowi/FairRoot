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

/**
 ** Parameter container factory for field parameter container
 **/

#ifndef PNDFIELDCONTFACT_H
#define PNDFIELDCONTFACT_H 1

#include "FairContFact.h"

class FairContainer;
class FairParSet;

class PndFieldContFact : public FairContFact {

 public:
  /** Constructor **/
  PndFieldContFact();

  /** Destructor **/
  ~PndFieldContFact();

  /** Create containers
   ** Creates the requested parameter sets (PndFieldPar)
   **/
  FairParSet *createContainer(FairContainer *container);

 private:
  /** Set all containers
   ** Creates container objects with all accepted contexts and adds them
   ** to the list of containers for the field library.
   **/
  void SetAllContainers();

  ClassDef(PndFieldContFact, 1);
};

#endif
