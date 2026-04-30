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


/** PndDipolePar.h
 ** @author M.Al-Turany
 ** @since 30.01.2007
 ** @version 1.0
 **
 ** Parameter set for the Panda Dipole. For the runtime database.
 **/

#ifndef PNDDIPOLEPAR_H
#define PNDDIPOLEPAR_H 1

#include "PndMapPar.h"

class PndDipolePar : public PndMapPar {
 public:
  /** Standard constructor  **/
  PndDipolePar(const char *name, const char *title, const char *context);

  /** default constructor  **/
  PndDipolePar();

  /** Destructor **/
  ~PndDipolePar();

  ClassDef(PndDipolePar, 1);
};

#endif
