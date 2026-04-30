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
 *      Author: stockman
 * PndRichHitRingSorter.h
 *
 */
// -------------------------------------------------------------------------
// -----                   PndRichHitRingSorter header file              -----
// -----         HARPHOOL KUMAWAT h.kumawat@gsi.de                     -----
// -----                                                               -----
// -------------------------------------------------------------------------

#ifndef PNDRICHHITRINGSORTER_H_
#define PNDRICHHITRINGSORTER_H_

#include <FairRingSorter.h>

class PndRichHitRingSorter : public FairRingSorter {
 public:
  PndRichHitRingSorter(int size = 100, double width = 10) : FairRingSorter(size, width){};
  virtual ~PndRichHitRingSorter();

  virtual FairTimeStamp *CreateElement(FairTimeStamp *data);

  ClassDef(PndRichHitRingSorter, 1);
};

#endif /* PNDRICHHITRINGSORTER_H_ */
