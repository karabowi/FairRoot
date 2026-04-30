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

// -------------------------------------------------------------------------
// -----                    PndFieldCreator header file                  -----
// -----                Created 15/01/07  by M. Al-Turany              -----
// -------------------------------------------------------------------------

#ifndef PNDFIELDCREATOR_H
#define PNDFIELDCREATOR_H

#include "FairField.h"
#include "FairFieldFactory.h"
#include "PndFieldPar.h"

class PndSolenoidPar;
class PndDipolePar;
class PndTransPar;
class PndConstPar;
class PndMultiFieldPar;

class PndFieldCreator : public FairFieldFactory {
 public:
  PndFieldCreator();
  virtual ~PndFieldCreator();
  virtual FairField *createFairField();
  virtual void SetParm();

 private:
  PndFieldCreator(const PndFieldCreator &L);
  PndFieldCreator &operator=(const PndFieldCreator &) { return *this; };

  ClassDef(PndFieldCreator, 1);

 protected:
  PndFieldPar *fFieldPar;
  PndSolenoidPar *fSPar;
  PndDipolePar *fDPar;
  PndTransPar *fTPar;
  PndConstPar *fCPar;
  PndMultiFieldPar *fMPar;
};
#endif // PNDFIELDCREATOR_H
