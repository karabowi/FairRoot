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

#ifndef BSEMCSHASHLIKECF_HH
#define BSEMCSHASHLIKECF_HH

#include "Rtypes.h"

#include "BSEmcThetaEnergyECF.h"

class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcShashlikECF
 * @brief Shashlik specific implementation of BSEmcThetaEnergyECF
 * @details based on and using PndEmcClusterHistCalibrator code and date.
 * Uses lookup histogram for energy correction from /macro/params/emc_correction_hist_gamma_1.root
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-01
 * @ingroup Shashlik
 */
class BSEmcShashlikECF : public BSEmcThetaEnergyECF {
 public:
  BSEmcShashlikECF();
  virtual ~BSEmcShashlikECF();

  ClassDef(BSEmcShashlikECF, 1);
};

#endif /*BSEMCSHASHLIKECF_HH*/
