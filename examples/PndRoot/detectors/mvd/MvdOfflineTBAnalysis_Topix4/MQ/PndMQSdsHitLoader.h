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
 * File:   PndMQSdsHitLoader.h
 * @since 2014-02-08
 * @author: A. Rybalchenko, N. Winckler
 */

#ifndef PNDMQSDSHITLOADER_H
#define PNDMQSDSHITLOADER_H

#include <iostream>
#include <type_traits>

#include <boost/timer/timer.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>

#include "TMessage.h"

#include "PndSdsHit.h"

#include "FairMQSamplerTask.h"
#include "FairMQLogger.h"

#include "baseMQtools.h"

using namespace std;

class PndMQSdsHitLoader : public FairMQSamplerTask {
 public:
  PndMQSdsHitLoader() : FairMQSamplerTask("Load class PndSdsHit"), fDigiVector(), fHasBoostSerialization(false)
  {
    using namespace baseMQ::tools::resolve;
    // coverity[pointless_expression]: suppress coverity warnings on apparant if(const).
    if (is_same<boost::archive::binary_oarchive, boost::archive::binary_oarchive>::value || is_same<boost::archive::binary_oarchive, boost::archive::text_oarchive>::value) {
      if (has_BoostSerialization<PndSdsHit, void(boost::archive::binary_oarchive &, const unsigned int)>::value == 1) {
        fHasBoostSerialization = true;
      }
    }
  }

  virtual ~PndMQSdsHitLoader()
  {
    if (fDigiVector.size() > 0) {
      fDigiVector.clear();
    }
  }

  virtual void Exec(Option_t *opt);

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version)
  {
    ar &fDigiVector;
  }

 private:
  friend class boost::serialization::access;
  vector<PndSdsHit> fDigiVector;
  bool fHasBoostSerialization;
};

#endif /* FAIRTESTDETECTORDIGILOADER_H */
