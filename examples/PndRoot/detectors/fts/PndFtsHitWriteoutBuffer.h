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
 * PndFtsHitWriteoutBuffer.h
 *
 *  Created on: May 10, 2011
 *      Author: stockman
 */

#ifndef PndFtsHitWRITEOUTBUFFER_H_
#define PndFtsHitWRITEOUTBUFFER_H_

#include "FairWriteoutBuffer.h"
#include "PndFtsHit.h"

class FairTimeStamp;

class PndFtsHitWriteoutBuffer : public FairWriteoutBuffer {

 public:
  PndFtsHitWriteoutBuffer();
  PndFtsHitWriteoutBuffer(TString branchName, TString folderName, Bool_t persistance);
  void AddNewDataToTClonesArray(FairTimeStamp *);

  virtual ~PndFtsHitWriteoutBuffer();

  virtual double FindTimeForData(FairTimeStamp *data);
  virtual void FillDataMap(FairTimeStamp *data, double activeTime);
  virtual void EraseDataFromDataMap(FairTimeStamp *data);

 protected:
  std::map<PndFtsHit, double> fData_map;

  ClassDef(PndFtsHitWriteoutBuffer, 1);
};

#endif /* PndFtsHitWRITEOUTBUFFER_H_ */
