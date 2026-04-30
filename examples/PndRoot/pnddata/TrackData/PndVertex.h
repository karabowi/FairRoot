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
 * PndVertex.h
 *
 *  Created on: Aug 25, 2009
 *      Author: stockman
 */

#ifndef PNDVERTEX_H_
#define PNDVERTEX_H_

#include "FairHit.h"

class PndVertex : public FairHit {
 public:
  PndVertex();
  PndVertex(double x, double y, double z)
  {
    SetX(x);
    SetY(y);
    SetZ(z);
  }
  virtual ~PndVertex();

  ClassDef(PndVertex, 0);
};

#endif /* PNDVERTEX_H_ */
