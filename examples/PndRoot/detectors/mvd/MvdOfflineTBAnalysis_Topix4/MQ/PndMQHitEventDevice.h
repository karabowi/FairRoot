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
 * PndMQTopix4Sink.h
 *
 * @since 2014-10-10
 * @author A. Rybalchenko
 */

#ifndef PndMQHitEventDevice_H_
#define PndMQHitEventDevice_H_

//#include "FairMQDevice.h"
#include "FairMQDevice.h"
#include "PndMvdReadInToPix4TBData.h"
#include "PndMQHitsEventBuilder.h"

#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/deque.hpp>

class PndMQHitEventDevice : public FairMQDevice {
 public:
  PndMQHitEventDevice();
  virtual ~PndMQHitEventDevice();

  virtual void SetProperty(const int key, const std::string &value);
  virtual std::string GetProperty(const int key, const std::string &default_ = "");
  virtual void SetProperty(const int key, const int value);
  virtual int GetProperty(const int key, const int default_ = 0);

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version)
  {
    ar &fHitData;
    ar &fEventData;
    ar &fSensorsInEvent;
  }

 protected:
  virtual void Run();

 private:
#ifndef __CINT__ // for BOOST serialization
  friend class boost::serialization::access;
#endif // for BOOST serialization
  std::vector<std::vector<PndSdsHit>> fHitData;
  // std::vector<PndSdsHit> fHitData;
  std::vector<std::vector<PndSdsHit>> fEventData;
  std::vector<std::vector<std::vector<PndSdsHit>>> fDataFromChannels;
  std::vector<bool> fRunningStatus;
  bool fGlobalRunningStatus;
  std::vector<int> fSensorsInEvent;

  bool fHasBoostSerialization;
  PndMQHitsEventBuilder *fBuilder;
};

#endif /* FAIRMQEXAMPLE1SINK_H_ */
