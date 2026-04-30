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
 * PndMQDataDuplicator.h
 *
 * @since 2014-10-10
 * @author A. Rybalchenko
 */

#ifndef PndMQDataDuplicator_H_
#define PndMQDataDuplicator_H_

#include <string>
#include <boost/serialization/access.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>

#include "FairMQDevice.h"

#include "PndSdsDigiTopix4.h"

class PndMQDataDuplicator : public FairMQDevice {
 public:
  PndMQDataDuplicator();
  virtual ~PndMQDataDuplicator();

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version)
  {
    ar &fTopixData;
  }

  void SetRateRatio(int channel, int ratio)
  {
    if (channel > fRates.size() - 1) {
      fRates.resize(channel + 1, 0);
    }
    fRates[channel] = ratio;
  }

 protected:
  virtual void Run();

  std::vector<int> fRates;

#ifndef __CINT__ // for BOOST serialization
  friend class boost::serialization::access;
  bool fHasBoostSerialization;
#endif // for BOOST serialization
  std::vector<PndSdsDigiTopix4> fTopixData;
};

#endif /* PndMQDataDuplicator_H_ */
