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
 * PndSdsDigiStripWriteoutBuffer.h
 *
 *  Created on: May 10, 2011
 *      Author: stockman
 */

#ifndef PNDSdsDigiStripWRITEOUTBUFFER_H_
#define PNDSdsDigiStripWRITEOUTBUFFER_H_

#include "FairWriteoutBuffer.h"
#include "PndSdsDigiStrip.h"

class PndSdsDigiStripWriteoutBuffer : public FairWriteoutBuffer {
 public:
  PndSdsDigiStripWriteoutBuffer();
  PndSdsDigiStripWriteoutBuffer(TString branchName, TString folderName, Bool_t persistance);
  void AddNewDataToTClonesArray(FairTimeStamp *);

  virtual ~PndSdsDigiStripWriteoutBuffer();

  virtual std::vector<std::pair<double, FairTimeStamp *>> Modify(std::pair<double, FairTimeStamp *> oldData, std::pair<double, FairTimeStamp *> newData);

  virtual double FindTimeForData(FairTimeStamp *data);
  virtual void FillDataMap(FairTimeStamp *data, double activeTime);
  virtual void EraseDataFromDataMap(FairTimeStamp *data);

 protected:
  std::map<PndSdsDigiStrip, double> fData_map;

  ClassDef(PndSdsDigiStripWriteoutBuffer, 1);
};

#endif /* PNDSdsDigiStripWRITEOUTBUFFER_H_ */
