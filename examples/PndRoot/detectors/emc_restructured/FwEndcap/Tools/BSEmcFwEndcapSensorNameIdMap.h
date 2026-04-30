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

#ifndef FWENDCAPSENSORNAMEIDMAP_H
#define FWENDCAPSENSORNAMEIDMAP_H

#include "Rtypes.h"
#include "RtypesCore.h"

#include "PndSensorNameIdMap.h"

class TBuffer;
class TClass;
class TMemberInspector;
class TString;

/**
 * @class BSEmcFwEndcapSensorNameIdMap
 * @brief FwEndcap specific crystal-geometry-name decoding into detector Id used by EmcDetector
 * @details this version is there for the "emc_module3_2012_new_EmcDetector.root" geometry
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-01
 * @ingroup FwEndcap
 */
class BSEmcFwEndcapSensorNameIdMap : public PndSensorNameIdMap {
 public:
  BSEmcFwEndcapSensorNameIdMap();
  virtual ~BSEmcFwEndcapSensorNameIdMap();

 private:
  virtual Int_t CreateId(TString &t_sensName) const /*override*/;

  ClassDef(BSEmcFwEndcapSensorNameIdMap, 1);
};

#endif /*FWENDCAPSENSORNAMEIDMAP_H*/
