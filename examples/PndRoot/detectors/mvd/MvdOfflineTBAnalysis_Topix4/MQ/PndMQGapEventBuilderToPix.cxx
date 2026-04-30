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
 * PndMQGapEventBuilderToPix.cxx
 *
 *  Created on: 14.10.2015
 *      Author: Stockmanns
 */

#include <PndMQGapEventBuilderToPix.h>

PndMQGapEventBuilderToPix::PndMQGapEventBuilderToPix() : fTimeGap(0) {}

PndMQGapEventBuilderToPix::PndMQGapEventBuilderToPix(double timeGap) : fTimeGap(timeGap) {}

PndMQGapEventBuilderToPix::~PndMQGapEventBuilderToPix()
{
  // TODO Auto-generated destructor stub
}

void PndMQGapEventBuilderToPix::FillData(std::vector<PndSdsDigiTopix4> data)
{
  fData.insert(fData.end(), data.begin(), data.end());
}

std::vector<std::vector<PndSdsDigiTopix4>> PndMQGapEventBuilderToPix::GetSeparatedData()
{
  std::vector<std::vector<PndSdsDigiTopix4>> separatedData;
  PndSdsDigiTopix4 oldData;
  std::vector<PndSdsDigiTopix4> tempData;
  for (auto itr : fData) {
    if (itr.GetTimeStamp() - oldData.GetTimeStamp() > fTimeGap) {
      if (tempData.size() > 0) {
        separatedData.push_back(tempData);
        tempData.clear();
      }
    }
    tempData.push_back(itr);
    oldData = itr;
  }
  fPreviousData = tempData;
  fData = fPreviousData;
  return separatedData;
}

std::vector<std::vector<PndSdsDigiTopix4>> PndMQGapEventBuilderToPix::GetLastData()
{
  std::vector<std::vector<PndSdsDigiTopix4>> separatedData;
  separatedData.push_back(fPreviousData);
  fPreviousData.clear();
  return separatedData;
}
