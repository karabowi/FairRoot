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
/**
 * PndBurstVectorBuilder.h
 *
 * @since 2012-12-06
 * @author D. Klein, A. Rybalchenko
 */

#ifndef PndBurstVectorBuilder_H_
#define PndBurstVectorBuilder_H_

#include "FairTimeStamp.h"

#include "PndBurstContainer.h"

#include "TClonesArray.h"

#include <boost/serialization/access.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>

using namespace std;

class PndBurstVectorBuilderBase {
 public:
  PndBurstVectorBuilderBase() //: fThreshold(2E3), fOffset(2E3), fHasBoostSerialization(false), fCurrentThreshold(fThreshold), fCurrentOffset(fOffset + fThreshold)
  {
  }

  virtual ~PndBurstVectorBuilderBase() {}

  virtual std::vector<std::vector<FairTimeStamp *>> ProcessData(TClonesArray *data) = 0;

  virtual std::vector<std::vector<FairTimeStamp *>> GetLastData() = 0;

  //	virtual void SetThreshold(double val){ fThreshold = val;}
  //	virtual void SetOffset(double val){ fOffset = val;}
  //
  //	virtual double GetThreshold(){return fThreshold;}
  //	virtual double GetOffset(){return fOffset;}

  virtual int GetBurstId(FairTimeStamp *data)
  {
    int result = data->GetTimeStamp() / fBurstContainer.GetThreshold();
    return result;
  }

  //	template <class Archive>
  //	void serialize(Archive& ar, const unsigned int version)
  //	{
  //		ar& fData;
  ////		ar& fCurrentOutput;
  ////		ar& fNextOutputs;
  //	}

 protected:
  //	double fThreshold;
  //	double fOffset;
  //	double fCurrentThreshold;
  //	double fCurrentOffset;

  //	#ifndef __CINT__ // for BOOST serialization
  //		friend class boost::serialization::access;
  //		bool fHasBoostSerialization;
  //	#endif // for BOOST serialization
  std::vector<FairTimeStamp *> fData;
  //		std::vector<FairTimeStamp* > fCurrentOutput;
  //		std::vector<std::vector<FairTimeStamp* > > fNextOutputs;
  PndBurstContainer fBurstContainer;
};

template <typename T>
class PndBurstVectorBuilderT : public PndBurstVectorBuilderBase {
 public:
  PndBurstVectorBuilderT() : PndBurstVectorBuilderBase() {}

  virtual ~PndBurstVectorBuilderT() {}

  std::vector<std::vector<FairTimeStamp *>> ProcessData(TClonesArray *data);

  std::vector<std::vector<FairTimeStamp *>> GetLastData();
};

#include "PndBurstVectorBuilder.tpl"

#endif /* PndBurstVectorBuilder_H_ */
