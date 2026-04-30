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

#ifndef BSEMCBARRELECF_HH
#define BSEMCBARRELECF_HH
#include "Rtypes.h"

#include "BSEmcThetaEnergyECF.h"

class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcBarrelECF
 * @brief Theta and Energy dependent energy correction for the Barrel
 * @details based on PndEmcClusterHistCalibrator code and data
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-01
 * @ingroup Barrel
 */
class BSEmcBarrelECF : public BSEmcThetaEnergyECF {
 public:
  BSEmcBarrelECF();
  virtual ~BSEmcBarrelECF();

  ClassDef(BSEmcBarrelECF, 1);
};

#endif /*BSEMCBARRELECF_HH*/
