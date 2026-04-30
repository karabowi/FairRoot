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

#ifndef BSEMCSHASHLIKDETECTOR_HH
#define BSEMCSHASHLIKDETECTOR_HH

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TString.h"

#include "BSEmcDetector.h"

class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcShashlikDetector
 * @brief Shashlik specific implementation of BSEmcDetector
 * @details
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-01
 * @ingroup Shashlik
 */
class BSEmcShashlikDetector : public BSEmcDetector {
 public:
  BSEmcShashlikDetector(const char *t_name, Bool_t t_active) : BSEmcDetector(t_name, t_active) {}
  BSEmcShashlikDetector() : BSEmcDetector() {}
  virtual void SetSpecialPhysicsCuts() /*override*/;

 protected:
  virtual Int_t GetDetectorId() /*override*/;
  TString ProcessSensitiveName(TString t_sensName) const;
  ClassDef(BSEmcShashlikDetector, 1);
};

#endif /*BSEMCSHASHLIKDETECTOR_HH*/
