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
 * PndBurstContainer.cxx
 *
 *  Created on: 23.08.2016
 *      Author: Stockmanns
 */

#include "PndBurstContainer.h"
#include "FairLogger.h"

std::vector<std::vector<FairTimeStamp *>> PndBurstContainer::ProcessData(std::vector<FairTimeStamp *> data)
{
  std::vector<std::vector<FairTimeStamp *>> result;
  for (auto dataItr : data) {
    //		LOG(info) << dataItr->GetTimeStamp();
    if ((dataItr->GetTimeStamp() - fCurrentThreshold) / fThreshold > 2.0)
      continue;
    if (IsAboveOffset(dataItr)) {
      result.push_back(*fContainer.begin());
      fContainer.erase(fContainer.begin());
      fCurrentThreshold += fThreshold;
      fCurrentOffset += fThreshold;
    }
    int pos = CalcContainerPos(dataItr);
    //		LOG(info) << "Pos: " << pos << " Thresh: " << fCurrentThreshold << " Offset: " << fCurrentOffset << " container.size " << fContainer.size();
    if (pos < 0) {
      //			LOG(info) << "PndBurstContainer: negative Pos: TimeStamp " << dataItr->GetTimeStamp()
      //					<< " fCurrenThreshold " << fCurrentThreshold << " fThreshold " << fThreshold
      //					<< " fCurrentOffset " << fCurrentOffset << " fOffset " << fOffset;
      continue;
    }
    if (pos > fContainer.size() - 1) {
      //			LOG(info) << "Resize container: " << pos + 1;
      fContainer.resize(pos + 1);
    }
    fContainer[pos].push_back(dataItr);
  }
  return result;
}
