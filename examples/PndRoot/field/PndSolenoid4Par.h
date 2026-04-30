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


/** PndSolenoidPar.h
 ** @author M.Al-Turany
 ** @since 30.01.2007
 ** @version 1.0
 **
 ** Parameter set for the Panda Solenoid. For the runtime database.
 **/

#ifndef PNDSOLENOID4PAR_H
#define PNDSOLENOID4PAR_H 1
#include "PndMapPar.h"

class PndSolenoid4Par : public PndMapPar {
 public:
  /** Standard constructor  **/
  PndSolenoid4Par(const char *name, const char *title, const char *context);

  /** default constructor  **/
  PndSolenoid4Par();
  /** Destructor **/
  ~PndSolenoid4Par();

  ClassDef(PndSolenoid4Par, 1);
};

#endif
