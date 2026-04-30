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

#ifndef BSEMCBARRELSENSORNAMEPAR_HH
#define BSEMCBARRELSENSORNAMEPAR_HH

#include "Rtypes.h"
#include "RtypesCore.h"

#include "PndSensorNameIdMap.h"

class TBuffer;
class TClass;
class TMemberInspector;
class TString;

/**
 * @class BSEmcBarrelSensorNameIdMap
 * @brief Barrel specific crystal-geometry-name decoding into detector Id used by EmcDetector
 * @details this version is there for the "emc_module12_2018v1_EmcDetector.root" geometry
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-01
 * @ingroup Barrel
 */
class BSEmcBarrelSensorNameIdMap : public PndSensorNameIdMap {
 public:
  BSEmcBarrelSensorNameIdMap();
  virtual ~BSEmcBarrelSensorNameIdMap();

 private:
  virtual Int_t CreateId(TString &t_sensName) const /*override*/;

  ClassDef(BSEmcBarrelSensorNameIdMap, 1);
};

#endif /*BSEMCBARRELSENSORNAMEPAR_HH*/
