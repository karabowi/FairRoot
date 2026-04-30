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
 * PndRichHitWriteoutBuffer.h
 *
 *  Created on: Mar 12, 2017
 *      Author: K. Beloborodov
 */

#ifndef PndRichHitWRITEOUTBUFFER_H_
#define PndRichHitWRITEOUTBUFFER_H_

#include "FairWriteoutBuffer.h"
#include "PndRichDigi.h"

class FairTimeStamp;

class PndRichHitWriteoutBuffer : public FairWriteoutBuffer {

 public:
  PndRichHitWriteoutBuffer();
  PndRichHitWriteoutBuffer(TString branchName, TString folderName, Bool_t persistance);
  void AddNewDataToTClonesArray(FairTimeStamp *);

  virtual ~PndRichHitWriteoutBuffer();

  virtual double FindTimeForData(FairTimeStamp *data);
  virtual void FillDataMap(FairTimeStamp *data, double activeTime);
  virtual void EraseDataFromDataMap(FairTimeStamp *data);

 protected:
  std::map<PndRichDigi, double> fData_map;

  ClassDef(PndRichHitWriteoutBuffer, 1);
};

#endif /* PndRichHitWRITEOUTBUFFER_H_ */
