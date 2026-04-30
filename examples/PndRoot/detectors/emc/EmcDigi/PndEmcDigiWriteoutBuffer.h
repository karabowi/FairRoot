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
 * PndEmcDigiWriteoutBuffer.h
 */

#ifndef PNDEMCDIGIWRITEOUTBUFFER_H_
#define PNDEMCDIGIWRITEOUTBUFFER_H_

#include "FairWriteoutBuffer.h"
#include "PndEmcDigi.h"

class FairTimeStamp;

class PndEmcDigiWriteoutBuffer : public FairWriteoutBuffer {

 public:
  PndEmcDigiWriteoutBuffer();
  PndEmcDigiWriteoutBuffer(TString branchName, TString folderName, Bool_t persistance);
  void AddNewDataToTClonesArray(FairTimeStamp *);

  virtual ~PndEmcDigiWriteoutBuffer();

  virtual double FindTimeForData(FairTimeStamp *data);
  virtual void FillDataMap(FairTimeStamp *data, double activeTime);
  virtual void EraseDataFromDataMap(FairTimeStamp *data);

 protected:
  std::map<PndEmcDigi, double> fData_map;

  ClassDef(PndEmcDigiWriteoutBuffer, 1);
};

#endif /* PNDEMCDIGIWRITEOUTBUFFER_H_ */
