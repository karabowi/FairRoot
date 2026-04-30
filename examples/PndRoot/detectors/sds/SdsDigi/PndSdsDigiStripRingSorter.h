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
 * PndSdsDigiStripRingSorter.h
 *
 *  Created on: Sep 9, 2011
 *      Author: stockman
 */

#ifndef PNDSDSDigiStripRINGSORTER_H_
#define PNDSDSDigiStripRINGSORTER_H_

#include <FairRingSorter.h>

class PndSdsDigiStripRingSorter : public FairRingSorter {
 public:
  PndSdsDigiStripRingSorter(int size = 100, double width = 10) : FairRingSorter(size, width){};
  virtual ~PndSdsDigiStripRingSorter();

  virtual FairTimeStamp *CreateElement(FairTimeStamp *data);

  ClassDef(PndSdsDigiStripRingSorter, 1);
};

#endif /* PNDSDSDigiStripRINGSORTER_H_ */
