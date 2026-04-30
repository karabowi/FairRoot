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

////////////////////////////////////////////////////////////////////////////
// PndOtContFact header file
//
// Factory for the parameter containers in libOt
//
// authors: Radoslaw Karabowicz, GSI, 2024
//
// modified from PndOtContFact by Nafija Ibrišimović in 2023
////////////////////////////////////////////////////////////////////////////

#ifndef PNDOTCONTFACT_H
#define PNDOTCONTFACT_H

// from FairRoot
#include <FairContFact.h>

class FairContainer;

class PndOtContFact : public FairContFact {
 private:
  void setAllContainers();

 public:
  PndOtContFact();
  ~PndOtContFact() {}
  FairParSet *createContainer(FairContainer *);
  //  void  activateParIo(FairParIo* io);
  ClassDef(PndOtContFact, 0) // Factory for all OT parameter containers
};

#endif /* !PNDOTCONTFACT_H */
