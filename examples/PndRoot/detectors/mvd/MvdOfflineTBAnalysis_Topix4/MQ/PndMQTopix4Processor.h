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

#ifndef PndMQTopix4Processor_H_
#define PndMQTopix4Processor_H_

//#include "FairMQDevice.h"
#include "FairMQDevice.h"
#include "PndMvdReadInToPix4TBData.h"

#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>

class PndMQTopix4Processor : public FairMQDevice {
 public:
  enum { FE = FairMQDevice::Last, TimeCorr, Last };

  PndMQTopix4Processor();
  virtual ~PndMQTopix4Processor();

  virtual void SetProperty(const int key, const std::string &value);
  virtual std::string GetProperty(const int key, const std::string &default_ = "");
  virtual void SetProperty(const int key, const int value);
  virtual int GetProperty(const int key, const int default_ = 0);

  void SetFilter(bool val) { fTopixDataReader.SetFilter(val); }

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version)
  {
    ar &fPndSdsDigiTopix4Vector;
    ar &fStatusValues;
  }

 protected:
  virtual void Run();

 private:
#ifndef __CINT__ // for BOOST serialization
  friend class boost::serialization::access;
#endif // for BOOST serialization
  std::vector<PndSdsDigiTopix4> fPndSdsDigiTopix4Vector;
  std::vector<int> fStatusValues;

  bool fHasBoostSerialization;
  int fFE;
  double fTimeStampCorrection;
  std::string fTimeCorrStr;

  PndMvdReadInToPix4TBData fTopixDataReader;
};

#endif /* FAIRMQEXAMPLE1SINK_H_ */
