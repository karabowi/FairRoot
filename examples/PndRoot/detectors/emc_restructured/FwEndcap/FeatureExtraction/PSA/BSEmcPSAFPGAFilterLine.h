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

#ifndef BSEMCPSAFPGAFILTERLINE_HH
#define BSEMCPSAFPGAFILTERLINE_HH
#include "RtypesCore.h"

class BSEmcPSAFPGAFilterLine {
 public:
  BSEmcPSAFPGAFilterLine(){};
  virtual ~BSEmcPSAFPGAFilterLine(){};
  virtual Float_t put(Float_t t_valueToAdd) = 0;
};

#endif /*BSEMCPSAFPGAFILTERLINE_HH*/
