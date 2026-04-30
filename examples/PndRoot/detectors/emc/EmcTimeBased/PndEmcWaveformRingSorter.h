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
 * PndEmcWaveformRingSorter.h
 */

#ifndef PNDEMCWAVEFORMRINGSORTER_H_
#define PNDEMCWAVEFORMRINGSORTER_H_

#include <FairRingSorter.h>

class PndEmcWaveformRingSorter : public FairRingSorter {
 public:
  PndEmcWaveformRingSorter(int size = 100, double width = 10) : FairRingSorter(size, width){};
  virtual ~PndEmcWaveformRingSorter();

  virtual FairTimeStamp *CreateElement(FairTimeStamp *data);

  ClassDef(PndEmcWaveformRingSorter, 1);
};

#endif /* PNDEMCWAVEFORMRINGSORTER_H_ */
