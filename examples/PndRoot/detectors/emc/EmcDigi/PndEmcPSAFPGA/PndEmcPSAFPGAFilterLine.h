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

#ifndef PNDEMCFPGAFILTERLINE_HH
#define PNDEMCFPGAFILTERLINE_HH

class PndEmcPSAFPGAFilterLine {
 public:
  PndEmcPSAFPGAFilterLine(){};
  virtual ~PndEmcPSAFPGAFilterLine(){};
  virtual float put(float valueToAdd) = 0;
};

#endif
