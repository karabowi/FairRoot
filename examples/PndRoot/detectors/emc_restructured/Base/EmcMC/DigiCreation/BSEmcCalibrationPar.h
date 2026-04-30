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

#ifndef BSEMCCALIBRATIONPAR_HH
#define BSEMCCALIBRATIONPAR_HH
#include <string>

#include "Rtypes.h"
#include "RtypesCore.h"

#include "FairParGenericSet.h"
#include "FairParamList.h"

class FairParamList;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcCalibrationPar
 * @brief Very basic start for CalibrationPar (each Detector gets its own)
 * @details  Will have to change, once we have more than just on calibration value per gain, but one per crystal
 * Look at BSEmcCrystalPositionPar or BSEmcGeoNeighbouringRelationPar for that
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcCalibrationPar : public FairParGenericSet {
 public:
  static const std::string fgParameterName;
  BSEmcCalibrationPar(const char *t_name = "EmcCalibrationPar", const char *t_title = "Calibration Values for emc detector", const char *t_context = "TestDefaultContext");
  virtual ~BSEmcCalibrationPar(void);

  void clear(void) /*override*/ {}
  void putParams(FairParamList * /*unused*/) /*override*/;
  Bool_t getParams(FairParamList * /*unused*/) /*override*/;

  Double_t GetHighGain() const { return fHighGain; }
  Double_t GetLowGain() const { return fLowGain; }
  Double_t GetDefaultGain() const { return fDefault; }

 private:
  Double_t fLowGain{1};
  Double_t fHighGain{1};
  Double_t fDefault{1};
  ClassDef(BSEmcCalibrationPar, 1)
};

#endif /*BSEMCCALIBRATIONPAR_HH*/
