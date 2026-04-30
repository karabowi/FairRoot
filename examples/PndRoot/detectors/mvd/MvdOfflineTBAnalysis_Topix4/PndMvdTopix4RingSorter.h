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
 * PndMvdTopix4RingSorter.h
 *
 *  Created on: Sep 9, 2011
 *      Author: stockman
 */

#ifndef PndMvdTopix4RingSorter_H_
#define PndMvdTopix4RingSorter_H_

#include <FairRingSorter.h>

class PndMvdTopix4RingSorter : public FairRingSorter {
 public:
  PndMvdTopix4RingSorter(int size = 100, double width = 10) : FairRingSorter(size, width){};
  virtual ~PndMvdTopix4RingSorter();

  virtual FairTimeStamp *CreateElement(FairTimeStamp *data);

  ClassDef(PndMvdTopix4RingSorter, 1);
};

#endif /* PndMvdTopix4RingSorter_H_ */
