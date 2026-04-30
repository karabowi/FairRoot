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
 * PndMQTopix4Sorter.h
 *
 * @since 2014-10-10
 * @author A. Rybalchenko
 */

#ifndef PndMQTopix4Sorter_H_
#define PndMQTopix4Sorter_H_

#include "FairMQDevice.h"

#include "PndSdsDigiTopix4.h"

#include <boost/serialization/access.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>

class PndMQTopix4Sorter : public FairMQDevice {
 public:
  PndMQTopix4Sorter();
  virtual ~PndMQTopix4Sorter();

  //    static void CustomCleanup(void *data, void *object);

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version)
  {
    ar &fTopixData;
    ar &fOutputData;
  }

 protected:
  virtual void Run();

#ifndef __CINT__ // for BOOST serialization
  friend class boost::serialization::access;
  bool fHasBoostSerialization;
#endif // for BOOST serialization

  std::vector<PndSdsDigiTopix4> fTopixData;
  std::vector<PndSdsDigiTopix4> fOutputData;
};

#endif /* FAIRMQEXAMPLE1SINK_H_ */
