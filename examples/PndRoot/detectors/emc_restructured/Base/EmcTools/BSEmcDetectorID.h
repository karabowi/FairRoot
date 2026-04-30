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
// -----					BSEmcDetectorID
// -----DetectorID class to bundle relative positioning info contained in an Integer
// -----	Created 25/03/2019 by B. Salisbury <salisbury@hiskp.uni-bonn.de>
// -------------------------------------------------------------------------
#ifndef BSEMCDETECTORID_HH
#define BSEMCDETECTORID_HH

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TObject.h"

class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmc2DMapper
 * @brief Represents a unique detectorid for a EMC crystal. It requires appropriate encoding of the module, row and column of the crystal.
 * @details  An appropriate Id would be: 329000027: a Forward-endcap crystal in row 29 and column 27.
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcDetectorID {
 public:
  BSEmcDetectorID(Int_t t_detectorId = 0);
  virtual ~BSEmcDetectorID();

  Short_t GetModule() const { return (fDetectorId / 100000000); };
  Short_t GetRow() const { return ((fDetectorId / 1000000) % 100); };
  Short_t GetColumn() const { return (fDetectorId % 10000); };
  Short_t GetCopy() const { return ((fDetectorId / 10000) % 100); };
  Short_t GetXPad() const;
  Short_t GetYPad() const;
  Int_t GetDetectorId() const { return fDetectorId; };

 private:
  Int_t fDetectorId{-1};

  ClassDef(BSEmcDetectorID, 1);
};

#endif /*BSEMCDETECTORID_HH*/
