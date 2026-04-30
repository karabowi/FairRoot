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
 * PndSttHitRingSorter.h
 *
 *  Created on: Sep 9, 2011
 *      Author: stockman
 */

#ifndef PndSttHitRINGSORTER_H_
#define PndSttHitRINGSORTER_H_

#include <FairRingSorter.h>

class PndSttHitRingSorter : public FairRingSorter {
 public:
  PndSttHitRingSorter(int size = 100, double width = 10) : FairRingSorter(size, width){};
  virtual ~PndSttHitRingSorter();

  virtual FairTimeStamp *CreateElement(FairTimeStamp *data);

  ClassDef(PndSttHitRingSorter, 1);
};

#endif /* PndSttHitRINGSORTER_H_ */
