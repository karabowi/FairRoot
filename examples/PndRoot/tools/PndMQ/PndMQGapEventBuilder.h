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
 * PndMQGapEventBuilder.h
 *
 *  Created on: 14.10.2015
 *      Author: Stockmanns
 */

#ifndef PndMQGapEventBuilder_H_
#define PndMQGapEventBuilder_H_

#include "FairTimeStamp.h"

class PndMQGapEventBuilder {
 public:
  PndMQGapEventBuilder();
  PndMQGapEventBuilder(double timeGap);
  virtual ~PndMQGapEventBuilder();

  void FillData(std::vector<FairTimeStamp *> data);
  std::vector<std::vector<FairTimeStamp *>> GetSeparatedData();
  std::vector<std::vector<FairTimeStamp *>> GetLastData();

 private:
  double fTimeGap;
  std::vector<FairTimeStamp *> fData;
  std::vector<FairTimeStamp *> fPreviousData;
};

#endif /* PndMQGapEventBuilder_H_ */
