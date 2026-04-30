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

#ifndef PNDMQTOPIX4PROCESSORTASK_H_
#define PNDMQTOPIX4PROCESSORTASK_H_

//#include "FairMQDevice.h"
#include "FairMQProcessorTask.h"
#include "PndMvdReadInToPix4TBData.h"
#include "PndTopix4.h"

#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>

class PndMQTopix4ProcessorTask : public FairMQProcessorTask {
 public:
  PndMQTopix4ProcessorTask();
  virtual ~PndMQTopix4ProcessorTask();

  virtual InitStatus Init() { return kSUCCESS; };

  virtual void Exec(Option_t *opt = "0");

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version)
  {
    ar &fPndSdsDigiTopix4Vector;
  }

 private:
#ifndef __CINT__ // for BOOST serialization
  friend class boost::serialization::access;
#endif // for BOOST serialization
  std::vector<PndSdsDigiTopix4> fPndSdsDigiTopix4Vector;

  bool fHasBoostSerialization;

  PndMvdReadInToPix4TBData fTopixDataReader;
};

#endif /* FAIRMQEXAMPLE1SINK_H_ */
