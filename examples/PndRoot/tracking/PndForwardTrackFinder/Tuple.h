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
 * Tuple.h
 *
 *  Created on: Jun 2, 2016
 *      Author: kibellus
 */
#include <iostream>
#include "PndLineApproximation.h"

#ifndef PNDTOOLS_PNDFORWARDTRACKFINDER_TUPLE_H_
#define PNDTOOLS_PNDFORWARDTRACKFINDER_TUPLE_H_

class Tuple {
 public:
  Tuple(PndLineApproximation xp, Int_t yp) : x(xp), y(yp) {}
  virtual ~Tuple();

  PndLineApproximation x;
  Int_t y;
};

#endif /* PNDTOOLS_PNDFORWARDTRACKFINDER_TUPLE_H_ */
