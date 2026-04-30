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
 * PndMQBurstProcessor.h
 *
 * @since 2013-06-05
 * @author A. Rybalchenko
 */

#ifndef PndMQBurstProcessor_H_
#define PndMQBurstProcessor_H_

#include <iostream>

#include "TList.h"
#include "TObjString.h"

#include "FairMQDevice.h"
#include "FairMQLogger.h"
#include "FairParGenericSet.h"
#include "FairGeoParSet.h"

#include "PndMvdMQFileSamplerBursts.h"

#include "baseMQtools.h"

#include "TMessage.h"

#ifndef __CINT__
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/unique_ptr.hpp>
#endif //__CINT__

using namespace std;

class PndMQBurstProcessor : public FairMQDevice {
 public:
  PndMQBurstProcessor() : fHasBoostSerialization(false), fCurrentRunId(0), fNewRunId(0)
  {
    gSystem->ResetSignal(kSigInterrupt);
    gSystem->ResetSignal(kSigTermination);

    // Check if boost serialization is available if it is chosen
    using namespace baseMQ::tools::resolve;
    // coverity[pointless_expression]: suppress coverity warnings on apparant if(const).
    if (is_same<boost::archive::binary_iarchive, boost::archive::binary_iarchive>::value || is_same<boost::archive::binary_iarchive, boost::archive::text_iarchive>::value) {
      if (has_BoostSerialization<FairTimeStamp, void(boost::archive::binary_iarchive &, const unsigned int)>::value == 1) {
        fHasBoostSerialization = true;
      }
    }
  }

  virtual ~PndMQBurstProcessor() { fParCList->Clear(); }

  virtual void UpdateParameters();
  virtual FairParGenericSet *UpdateParameter(FairParGenericSet *thisPar);
  virtual void SetParameters(){};
  static void CustomCleanupParameters(void *data, void *hint);
  static void free_string(void *data, void *hint);

  virtual void ProcessData() = 0;

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version)
  {
    ar &fBurstDataIn;
    ar &fBurstDataOut;
  }

 protected:
  virtual void Run();
  BurstData fBurstDataIn;
  BurstData fBurstDataOut;
  int fCurrentRunId;
  int fNewRunId;
  TList *fParCList;

 private:
#ifndef __CINT__ // for BOOST serialization
  friend class boost::serialization::access;
  bool fHasBoostSerialization;
#endif // for BOOST serialization

  /// Copy Constructor
  //    PndMQBurstProcessor(const PndMQBurstProcessor&);
  //    PndMQBurstProcessor operator=(const PndMQBurstProcessor&);
};

#endif /* PndMQBurstProcessor_H_ */
