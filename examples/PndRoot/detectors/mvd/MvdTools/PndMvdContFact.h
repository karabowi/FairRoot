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

#ifndef PNDMVDCONTFACT_H
#define PNDMVDCONTFACT_H 1

#include "FairContFact.h"

class FairContainer;
// class FairParIo;

class PndMvdContFact : public FairContFact {
 public:
  PndMvdContFact();
  ~PndMvdContFact();
  PndMvdContFact(const PndMvdContFact &) = delete;
  PndMvdContFact &operator=(const PndMvdContFact &) = delete;
  FairParSet *createContainer(FairContainer *c);
  TList *GetDigiParNames() { return fDigiParNames; };

 private:
  void setAllContainers();
  TList *fDigiParNames;        //!
  ClassDef(PndMvdContFact, 3); // Factory for all MVD parameter containers
};

#endif /* !MVDCONTFACT_H */
