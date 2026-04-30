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

#ifndef L1MaterialInfo_h
#define L1MaterialInfo_h

class L1MaterialInfo {

  //* information about material

 public:
  L1MaterialInfo() : thick(0), RL(0), RadThick(0), logRadThick(0){};

  float thick, RL, RadThick, logRadThick;
};

#endif
