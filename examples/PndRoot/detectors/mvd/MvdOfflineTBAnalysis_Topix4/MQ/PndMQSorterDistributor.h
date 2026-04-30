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
 * PndMQSorterDistributor.h
 *
 * @since 2012-12-06
 * @author D. Klein, A. Rybalchenko
 */

#ifndef PndMQSorterDistributor_H_
#define PndMQSorterDistributor_H_

#include "FairMQDevice.h"

#include "PndSdsDigiTopix4.h"

#include <boost/serialization/access.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>

class PndMQSorterDistributor : public FairMQDevice {
 public:
  PndMQSorterDistributor();
  virtual ~PndMQSorterDistributor();

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version)
  {
    ar &fTopixData;
    ar &fCurrentOutput;
    ar &fNextOutput;
  }

 protected:
  virtual void Run();
  double fThreshold;
  double fOffset;

#ifndef __CINT__ // for BOOST serialization
  friend class boost::serialization::access;
  bool fHasBoostSerialization;
#endif // for BOOST serialization
  std::vector<PndSdsDigiTopix4> fTopixData;
  std::vector<PndSdsDigiTopix4> fCurrentOutput;
  std::vector<PndSdsDigiTopix4> fNextOutput;
};

#endif /* PndMQSorterDistributor_H_ */
