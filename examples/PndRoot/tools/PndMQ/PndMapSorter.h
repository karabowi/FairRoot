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
 * PndMapSorter.h
 *
 *  Created on: Jul 15, 2010
 *      Author: stockman
 */

#ifndef PndMapSorter_H_
#define PndMapSorter_H_

//#include "TObject.h"                    // for TObject

#include <iosfwd>   // for ostream
#include "Rtypes.h" // for PndMapSorter::Class, etc

#include <iostream> // for operator<<, ostream, etc
#include <map>      // for multimap
#include <utility>  // for pair
#include <vector>   // for vector

#include "FairTimeStamp.h"

class PndMapSorter //: public TObject
{
 public:
  PndMapSorter(double timeOffset = 1000000) : fOutputData(), fVerbose(0), fTimeOffset(timeOffset), fOldTS(0) {}

  virtual ~PndMapSorter(){};

  virtual FairTimeStamp *CreateElement(FairTimeStamp *data);

  virtual void AddElement(FairTimeStamp *digi, double timestamp);
  virtual void AddElements(std::vector<FairTimeStamp *> dataArray)
  {
    for (auto dataItr : dataArray)
      AddElement(dataItr, dataItr->GetTimeStamp());
  }
  virtual void WriteOutAll();
  virtual void WriteOutData(double time);
  virtual std::vector<FairTimeStamp *> GetOutputData() { return fOutputData; }

  virtual void DeleteOutputData()
  {
    //    	std::cout << "DeleteOutputData size: " << fOutputData.size() << std::endl;
    for (int itr = 0; itr < fOutputData.size(); itr++) {
      //  		std::cout << itr << " " << fOutputData[itr] << std::endl;
      //   		delete(fOutputData[itr]);
    }
    fOutputData.clear();
  }

  virtual void print(std::ostream &out = std::cout) {}

  virtual void PrintMap(std::ostream &out = std::cout)
  {
    int i = 0;
    out << "PndMapsSorter Print Map: " << std::endl;
    for (std::multimap<double, FairTimeStamp *>::iterator itr = fMapBuffer.begin(); itr != fMapBuffer.end(); itr++) {
      out << i++ << " : " << itr->first << std::endl;
    }
    out << std::endl;
  }

 private:
  std::multimap<double, FairTimeStamp *> fMapBuffer;
  std::vector<FairTimeStamp *> fOutputData;
  double fOldTS;
  double fTimeOffset;
  int fVerbose;

  //    ClassDef(PndMapSorter,1)
};

#endif /* PndMapSorter_H_ */
