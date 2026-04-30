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

#ifndef PNDSENSORNAMECONTFACT_H
#define PNDSENSORNAMECONTFACT_H

#include "FairContFact.h"

#include <vector>
#include <string>

class FairContainer;
// class FairParIo;

class PndSensorNameContFact : public FairContFact {
 public:
  PndSensorNameContFact();
  ~PndSensorNameContFact();
  FairParSet *createContainer(FairContainer *c);
  TList *GetSensorParNames() { return fSensorParNames; };

 private:
  void setAllContainers();
  TList *fSensorParNames{nullptr}; //!
  std::vector<std::string> fContainerNames;

  PndSensorNameContFact(const PndSensorNameContFact &L);
  PndSensorNameContFact &operator=(const PndSensorNameContFact &) { return *this; }

  ClassDef(PndSensorNameContFact, 1); // Factory for all SensorName parameter containers
};

#endif /* !MVDCONTFACT_H */
