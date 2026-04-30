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
/*
 * PndMapSorterTpl.h
 *
 *  Created on: Jul 15, 2010
 *      Author: stockman
 */

#ifndef PndMapSorterTpl_H_
#define PndMapSorterTpl_H_

//#include <iosfwd>                     // for ostream
//#include "Rtypes.h"                     // for PndMapSorterTpl::Class, etc

#include <iostream> // for operator<<, ostream, etc
#include <map>      // for multimap
#include <utility>  // for pair
#include <vector>   // for vector

#include "FairTimeStamp.h"
#include "PndSdsDigiTopix4.h"

template <typename TData>
class PndMapSorterTpl {
 public:
  PndMapSorterTpl(double timeOffset = 1000000) : fOutputData(), fVerbose(0), fTimeOffset(timeOffset), fOldTS(0) {}

  virtual ~PndMapSorterTpl(){};

  //   virtual FairTimeStamp* CreateElement(FairTimeStamp* data);

  virtual void AddElement(TData digi, double timestamp);
  virtual void WriteOutAll();
  virtual void WriteOutData(double time);
  virtual std::vector<TData> GetOutputData() { return fOutputData; }

  virtual void DeleteOutputData() { fOutputData.clear(); }

  virtual void print(std::ostream &out = std::cout) {}

  virtual void PrintMap(std::ostream &out = std::cout)
  {
    int i = 0;
    out << "PndMapSorter Print Map: " << std::endl;
    for (auto itr : fMapBuffer) {
      out << i++ << " : " << itr.first << std::endl;
    }
    out << std::endl;
  }

 private:
  std::multimap<double, TData> fMapBuffer;
  std::vector<TData> fOutputData;
  double fOldTS;
  double fTimeOffset;
  int fVerbose;
};

#include "PndMapSorterTpl.tpl"

#endif /* PndMapSorterTpl_H_ */
