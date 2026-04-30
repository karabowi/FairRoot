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

#ifndef BSEMCBASELINEADDER_HH
#define BSEMCBASELINEADDER_HH

#include "Rtypes.h"
#include "RtypesCore.h"

#include "BSEmcAbsWaveformModifier.h"

class BSEmcWaveform;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcBaselineAdder
 * @brief Adds a Baseline to a waveform
 * @details
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-03
 * @ingroup FwEndcap
 */
class BSEmcBaselineAdder : public BSEmcAbsWaveformModifier {
 public:
  BSEmcBaselineAdder();
  BSEmcBaselineAdder(Double_t t_distributionMean, Double_t t_distributionWidth);

  virtual void Modify(BSEmcWaveform *t_wf) /*override*/;
  void SetDistributionMean(Double_t t_mean) { fBaselineDistributionMean = t_mean; }
  void SetDistributionWidth(Double_t t_width) { fBaselineDistributionWidth = t_width; }

 private:
  Double_t fBaselineDistributionMean{0};
  Double_t fBaselineDistributionWidth{0};
  ClassDef(BSEmcBaselineAdder, 0)
};

#endif /*BSEMCBASELINEADDER_HH*/
