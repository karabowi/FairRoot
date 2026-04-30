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

// -------------------------------------------------------------------------
// -----					PndSensorNameIdMap
// -----
// -----	Created 25/03/2019 by B. Salisbury <salisbury@hiskp.uni-bonn.de>
// -------------------------------------------------------------------------
#ifndef PNDSENSORNAMEIDMAP_HH
#define PNDSENSORNAMEIDMAP_HH
#include "TString.h"
#include "TObject.h"
/**
 * @brief Base class for NameIdMaps used by the EmcGeoHandling singleton to
 * retrieve a detector id from a name string. Each detector component defines
 * its own map deriving from this class.
 *
 * @author B. Salisbury <salisbury@hiskp.uni-bonn.de>
 * @ingroup PndEmc
 */
class PndSensorNameIdMap : public TObject {
 public:
  PndSensorNameIdMap();
  virtual ~PndSensorNameIdMap();

  Int_t GetId(TString &sensName) const;

 private:
  virtual Int_t CreateId(TString &sensName) const = 0;

  ClassDef(PndSensorNameIdMap, 1);
};

#endif /*PNDSENSORNAMEIDMAP_HH*/
