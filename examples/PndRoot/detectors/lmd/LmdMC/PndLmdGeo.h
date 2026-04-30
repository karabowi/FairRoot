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

/*
 * PndLmdGeo.h
 * *  Created on: Jun 30, 2010
 *      Author: huagen
 */
/*
 * PndLmdGeo class will define the detector name by user. It was inherited from
 * Sds base class PndSdsGeo.
 */

#ifndef PNDLMDGEO_H_
#define PNDLMDGEO_H_

#include "PndSdsGeo.h"

class PndLmdGeo : public PndSdsGeo {
 public:
  PndLmdGeo();
  ~PndLmdGeo();

  const char *getModuleName(Int_t m);
  const char *getEleName(Int_t m);

  ClassDef(PndLmdGeo, 1);
};

#endif /* PNDLMDGEO_H_ */
