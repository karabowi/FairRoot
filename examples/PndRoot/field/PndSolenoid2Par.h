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


/** PndSolenoid2Par.h
 ** @author M.Al-Turany
 ** @since 30.01.2007
 ** @version 1.0
 **
 ** Parameter set for the Panda Solenoid. For the runtime database.
 **/

#ifndef PNDSOLENOID2PAR_H
#define PNDSOLENOID2PAR_H 1
#include "PndMapPar.h"

class PndSolenoid2Par : public PndMapPar {
 public:
  /** Standard constructor  **/
  PndSolenoid2Par(const char *name, const char *title, const char *context);

  /** default constructor  **/
  PndSolenoid2Par();
  /** Destructor **/
  ~PndSolenoid2Par();

  ClassDef(PndSolenoid2Par, 1);
};

#endif
