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
 * PndMQSorterMerger.h
 *
 * @since 2012-12-06
 * @author D. Klein, A. Rybalchenko
 */

#ifndef PndMQSorterMerger_H_
#define PndMQSorterMerger_H_

#include "FairMQDevice.h"

#include "PndSdsDigiTopix4.h"

#include <boost/serialization/access.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>

class PndMQSorterMerger : public FairMQDevice {
 public:
  PndMQSorterMerger();
  virtual ~PndMQSorterMerger();

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version)
  {
    ar &fInputData;
    ar &fOutputData;
  }

 protected:
  virtual void Run();
  std::vector<std::vector<PndSdsDigiTopix4>> fData;
  std::vector<PndSdsDigiTopix4> fOutputData;
  std::vector<PndSdsDigiTopix4> fInputData;

#ifndef __CINT__ // for BOOST serialization
  friend class boost::serialization::access;
  bool fHasBoostSerialization;
#endif // for BOOST serialization
  std::vector<PndSdsDigiTopix4> fTopixData;
  std::vector<PndSdsDigiTopix4> fCurrentOutput;
  std::vector<PndSdsDigiTopix4> fNextOutput;
  std::vector<bool> fRunningStatus;
};

#endif /* PndMQSorterMerger_H_ */
