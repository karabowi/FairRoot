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

/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
// -------------------------------------------------------------------------
//  PndMapSorter.cxx
//   Created on: Jul 30, 2010
//       Author: stockman
// -------------------------------------------------------------------------

#include "PndMapSorter.h"
#include "FairTimeStamp.h" // for FairTimeStamp

FairTimeStamp *PndMapSorter::CreateElement(FairTimeStamp *data)
{
  return (FairTimeStamp *)data->Clone();
}

void PndMapSorter::AddElement(FairTimeStamp *digi, double timestamp)
{
  if (fVerbose > 0)
    std::cout << "PndMapSorter::AddElement " << timestamp << std::endl;
  fMapBuffer.insert(std::pair<double, FairTimeStamp *>(timestamp, CreateElement(digi)));
}

void PndMapSorter::WriteOutData(double time)
{
  if (time > fTimeOffset) {
    if (fVerbose > 0)
      std::cout << "PndMapSorter::WriteOutData for time " << time << std::endl;
    //	std::cout << "PrintMap before!" << std::endl;
    //	PrintMap();
    for (std::multimap<double, FairTimeStamp *>::iterator itr = fMapBuffer.begin(); itr != fMapBuffer.lower_bound(time - fTimeOffset); itr++) {
      fOutputData.push_back(itr->second);
      if (fVerbose > 1) {
        std::cout << "TS: " << itr->first << " " << itr->second->GetTimeStamp() << " diff " << itr->first - fOldTS << std::endl;
      }
      if (itr->first - fOldTS < 0)
        std::cout << "*** Error *** PndMapSorter::WriteOutData negative difference " << itr->first << " - " << fOldTS << std::endl;
      fOldTS = itr->first;
    }
    fMapBuffer.erase(fMapBuffer.begin(), fMapBuffer.lower_bound(time - fTimeOffset));
    //		std::cout << "PrintMap after!" << std::endl;
    //		PrintMap();
  }
}

void PndMapSorter::WriteOutAll()
{
  if (fVerbose > 0)
    std::cout << "PndMapSorter::WriteOutAll called!" << std::endl;
  for (std::multimap<double, FairTimeStamp *>::iterator itr = fMapBuffer.begin(); itr != fMapBuffer.end(); itr++)
    fOutputData.push_back(itr->second);
}

// ClassImp(PndMapSorter);
