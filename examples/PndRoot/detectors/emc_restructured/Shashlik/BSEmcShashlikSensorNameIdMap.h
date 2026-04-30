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

#ifndef BSEMCSHASHLIKSENSORNAMEPAR_HH
#define BSEMCSHASHLIKSENSORNAMEPAR_HH

#include "Rtypes.h"
#include "RtypesCore.h"

#include "PndSensorNameIdMap.h"

class TBuffer;
class TClass;
class TMemberInspector;
class TString;

/**
 * @class BSEmcShashlikSensorNameIdMap
 * @brief Maps Shashlik geometry name (for emc_module5_fsc_EmcDetector.root) to Detector IDs
 * @details based on PndEmc.cxx code
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-01
 * @ingroup Shashlik
 */
class BSEmcShashlikSensorNameIdMap : public PndSensorNameIdMap {
 public:
  BSEmcShashlikSensorNameIdMap();
  virtual ~BSEmcShashlikSensorNameIdMap();

 private:
  virtual Int_t CreateId(TString &t_sensName) const /*override*/;

  ClassDef(BSEmcShashlikSensorNameIdMap, 1);
};

#endif /*BSEMCSHASHLIKSENSORNAMEPAR_HH*/
