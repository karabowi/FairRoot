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

#ifndef BSEMCFWENDCAP_AGTHOMABONNVERSION_SENSORNAMEIDMAP_HH
#define BSEMCFWENDCAP_AGTHOMABONNVERSION_SENSORNAMEIDMAP_HH

#include <utility>
#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TString.h"

#include "PndSensorNameIdMap.h"

class TBuffer;
class TClass;
class TMemberInspector;
class TString;

/**
 * @class BSEmcFwEndcap_AgThomaBonnVersion_SensorNameIdMap
 * @brief FwEndcap specific crystal-geometry-name decoding into detector Id used by EmcDetector
 * @details this version is there for the "New_FwEndcap_All_Oct5.root" geometry
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup FwEndcap
 */
class BSEmcFwEndcap_AgThomaBonnVersion_SensorNameIdMap : public PndSensorNameIdMap {
 public:
  BSEmcFwEndcap_AgThomaBonnVersion_SensorNameIdMap();
  virtual ~BSEmcFwEndcap_AgThomaBonnVersion_SensorNameIdMap();
  virtual Int_t CreateId(TString &t_sensName) const /*override*/;

 private:
  std::pair<Int_t, Int_t> ConvertIntoAlveole(Int_t t_section, Int_t t_alveolex, Int_t t_alveoley) const;
  Int_t RemapCrystal(Int_t t_crystal, Int_t t_alveolex, Int_t t_alveoley) const;
  Bool_t IsIncludedIn(Int_t t_xIndex, Int_t t_yIndex, const std::vector<std::pair<Int_t, Int_t>> &t_indices) const;
  Int_t CalculateRow(Int_t t_crystal, Int_t t_xAlveoleIndex, Int_t t_yAlveoleIndex) const;
  Int_t CalculateColumn(Int_t t_crystal, Int_t t_xAlveoleIndex, Int_t t_yAlveoleIndex) const;

  std::vector<std::pair<Int_t, Int_t>> f0DegRot{};
  std::vector<std::pair<Int_t, Int_t>> f90DegRot{};
  std::vector<std::pair<Int_t, Int_t>> f180DegRot{};
  std::vector<std::pair<Int_t, Int_t>> f270DegRot{};
  std::vector<std::pair<Int_t, Int_t>> fRequireOffset{};

  ClassDef(BSEmcFwEndcap_AgThomaBonnVersion_SensorNameIdMap, 1);
};

#endif /*BSEMCFWENDCAP_AGTHOMABONNVERSION_SENSORNAMEIDMAP_HH*/
