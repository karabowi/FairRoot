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
 * PndMQMvdPixelDigiProcessorBursts.h
 *
 * @since 2013-06-05
 * @author A. Rybalchenko
 */

#ifndef PndMQMvdPixelDigiProcessorBursts_H_
#define PndMQMvdPixelDigiProcessorBursts_H_

#include "FairMQDevice.h"
#include "FairMQLogger.h"
#include "FairParGenericSet.h"
#include "FairGeoParSet.h"

#include "PndMQBurstProcessor.h"
#include "PndMQGapEventBuilder.h"
#include "PndMvdPixelClusterFinder.h"
#include "PndMQMvdChargeWeightedPixelMapping.h"
#include "PndMapSorter.h"
#include "PndGeoHandling.h"

#include "PndMvdMQFileSamplerBursts.h"
#include "PndSdsPixelDigiPar.h"
#include "PndSdsTotDigiPar.h"
#include "PndSensorNamePar.h"

#include "PndSdsHit.h"

#include "baseMQtools.h"

#include "TMessage.h"

#ifndef __CINT__
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/unique_ptr.hpp>
#include <boost/serialization/export.hpp>
#endif //__CINT__

BOOST_CLASS_EXPORT_GUID(PndSdsHit, "PndSdsHit");

using namespace std;

class PndMQMvdPixelDigiProcessorBursts : public PndMQBurstProcessor {
 public:
  PndMQMvdPixelDigiProcessorBursts() : PndMQBurstProcessor(), fPixelMapping(0), fGeoHandler(0)
  {
    fGeoPar = new FairGeoParSet("FairGeoParSet");
    fParCList = new TList();
    fParCList->Add(fGeoPar);

    fDigiPar = new PndSdsPixelDigiPar("MVDPixelDigiPar");
    fParCList->Add(fDigiPar);

    fTotPar = new PndSdsTotDigiPar("MVDPixelTotDigiPar");
    fParCList->Add(fTotPar);

    fSensorPar = new PndSensorNamePar("PndSensorNamePar");
    fParCList->Add(fSensorPar);
  }

  virtual ~PndMQMvdPixelDigiProcessorBursts()
  {
    if (fGeoPar != 0)
      delete fGeoPar;
    if (fDigiPar != 0)
      delete fDigiPar;
    if (fTotPar != 0)
      delete fTotPar;
    if (fSensorPar != 0)
      delete fSensorPar;
    if (fPixelMapping != 0)
      delete fPixelMapping;
    if (fGeoHandler != 0)
      delete fGeoHandler;
  }

  virtual void SetParameters();

  virtual void ProcessData();

 private:
  FairGeoParSet *fGeoPar;
  PndSdsPixelDigiPar *fDigiPar;
  PndSdsTotDigiPar *fTotPar;
  PndSensorNamePar *fSensorPar;

  PndMQGapEventBuilder fEventBuilder;
  PndMapSorter fMapSorter;
  PndMvdPixelClusterFinder fClusterFinder;

  PndMQMvdChargeWeightedPixelMapping *fPixelMapping;
  PndGeoHandling *fGeoHandler;
};

#endif /* PndMQMvdPixelDigiProcessorBursts_H_ */
