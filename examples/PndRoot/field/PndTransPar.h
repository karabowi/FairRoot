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

/** PndTransPar.h
 ** @author M.Al-Turany
 ** @since 30.01.2007
 ** @version 1.0
 **
 ** Parameter set for the region between Solenoid and dipole. For the runtime database.
 **/

#ifndef PNDTRANSPAR_H
#define PNDTRANSPAR_H 1

#include "PndMapPar.h"

class PndTransPar : public PndMapPar {
 public:
  /** Standard constructor  **/
  PndTransPar(const char *name, const char *title, const char *context);

  /** default constructor  **/
  PndTransPar();

  /** Destructor **/
  ~PndTransPar();

  ClassDef(PndTransPar, 1);
};

#endif
