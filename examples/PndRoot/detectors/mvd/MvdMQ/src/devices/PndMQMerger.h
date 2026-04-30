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
 * PndMQMerger.h
 *
 * @since 2012-12-06
 * @author D. Klein, A. Rybalchenko
 */

#ifndef PndMQMerger_H_
#define PndMQMerger_H_

#include "FairMQDevice.h"

#include <PndMvdMQFileSamplerBursts.h>

#include <boost/serialization/access.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>

class PndMQMerger : public FairMQDevice {
 public:
  PndMQMerger();
  virtual ~PndMQMerger();

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version)
  {
    ar &fInputData;
    ar &fOutputData;
  }

 protected:
  virtual void Run();
  BurstData *fOutputData;
  BurstData fInputData;
  std::map<int, std::map<std::string, BurstData>> fInputMap; //< map <BurstID, map<BranchName, Data> >

  virtual void ProcessData(std::map<std::string, BurstData> &dataToProcess) = 0;

#ifndef __CINT__ // for BOOST serialization
  friend class boost::serialization::access;
  bool fHasBoostSerialization;
#endif // for BOOST serialization

  std::vector<bool> fRunningStatus;
};

#endif /* PndMQMerger_H_ */
