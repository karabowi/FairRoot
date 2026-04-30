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
 * PndSttHitWriteoutBuffer.h
 *
 *  Created on: May 10, 2011
 *      Author: stockman
 */

#ifndef PndSttHitWRITEOUTBUFFER_H_
#define PndSttHitWRITEOUTBUFFER_H_

#include "FairWriteoutBuffer.h"
#include "PndSttHit.h"
#include "FairLogger.h"

class FairTimeStamp;

class PndSttHitWriteoutBuffer : public FairWriteoutBuffer {

 public:
  PndSttHitWriteoutBuffer();
  PndSttHitWriteoutBuffer(TString branchName, TString folderName, Bool_t persistance);
  void AddNewDataToTClonesArray(FairTimeStamp *);

  virtual ~PndSttHitWriteoutBuffer();

  virtual double FindTimeForData(FairTimeStamp *data);
  virtual void FillDataMap(FairTimeStamp *data, double activeTime);
  virtual void EraseDataFromDataMap(FairTimeStamp *data);

 protected:
  std::map<PndSttHit, double> fData_map;

  ClassDef(PndSttHitWriteoutBuffer, 1);
};

#endif /* PndSttHitWRITEOUTBUFFER_H_ */
