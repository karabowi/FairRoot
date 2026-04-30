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
 * PndSciTHitWriteoutBuffer.h
 *
 *  Created on: May 13, 2015
 *      Author: Steinschaden Dominik
 *  Last update: 6.2015
 */

#ifndef PNDSCITHITWRITEOUTBUFFER_H
#define PNDSCITHITWRITEOUTBUFFER_H

#include "FairWriteoutBuffer.h"

#include "PndSciTHit.h"

class FairTimeStamp;

class PndSciTHitWriteoutBuffer : public FairWriteoutBuffer {

 public:
  PndSciTHitWriteoutBuffer();
  PndSciTHitWriteoutBuffer(TString branchName, TString folderName, Bool_t persistance);

  virtual ~PndSciTHitWriteoutBuffer();

  void AddNewDataToTClonesArray(FairTimeStamp *);

  virtual double FindTimeForData(FairTimeStamp *data);

  virtual void FillDataMap(FairTimeStamp *data, double activeTime);

  virtual void EraseDataFromDataMap(FairTimeStamp *data);

  virtual std::vector<std::pair<double, FairTimeStamp *>> Modify(std::pair<double, FairTimeStamp *> oldData, std::pair<double, FairTimeStamp *> newData);

 protected:
  std::map<PndSciTHit, double> fData_map;

  ClassDef(PndSciTHitWriteoutBuffer, 1);
};

#endif /* PNDSCITHITWRITEOUTBUFFER_H */
