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
 * PndSciTHitRingSorter.h
 *
 *  Created on: May 13, 2015
 *      Author: Steinschaden Dominik
 *  Last update: 6.2015
 */

#ifndef PNDSCITHITRINGSORTER_H
#define PNDSCITHITRINGSORTER_H

#include "FairRingSorter.h"

class PndSciTHitRingSorter : public FairRingSorter {
 public:
  PndSciTHitRingSorter(int size = 100, double width = 10) : FairRingSorter(size, width){};
  virtual ~PndSciTHitRingSorter();

  virtual FairTimeStamp *CreateElement(FairTimeStamp *data);

  ClassDef(PndSciTHitRingSorter, 1);
};

#endif /* PndSciTHitRingSorter_H */
