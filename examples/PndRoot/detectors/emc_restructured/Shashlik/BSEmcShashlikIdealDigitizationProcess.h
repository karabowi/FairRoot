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

#ifndef BSEMCSHASHLIKIDEALDIGITIZATIONPROCESS_HH
#define BSEMCSHASHLIKIDEALDIGITIZATIONPROCESS_HH

#include "Rtypes.h"
#include "RtypesCore.h"

#include "BSEmcIdealDigitizationProcess.h"

class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcShashlikIdealDigitizationProcess
 * @brief IdealDigitizationProcess (non waveform) for Shashlik requires special smearing of energy
 * @details
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-01
 * @ingroup Shashlik
 */
class BSEmcShashlikIdealDigitizationProcess : public BSEmcIdealDigitizationProcess {
 public:
  BSEmcShashlikIdealDigitizationProcess();
  virtual ~BSEmcShashlikIdealDigitizationProcess();

 protected:
  virtual Double_t GausSmearing(Double_t t_energy) const /*override*/;

 private:
  ClassDef(BSEmcShashlikIdealDigitizationProcess, 1);
};

#endif /*BSEMCSHASHLIKIDEALDIGITIZATIONPROCESS_HH*/
