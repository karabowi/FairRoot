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

#ifndef BSEMCFWENDCAPECF_HH
#define BSEMCFWENDCAPECF_HH
#include "Rtypes.h"

#include "BSEmcThetaEnergyECF.h"

class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcFwEndcapECF
 * @brief Theta and Energy dependent energy correction for the FwEndcap
 * @details based on PndEmcClusterHistCalibrator code and data
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup FwEndcap
 */
class BSEmcFwEndcapECF : public BSEmcThetaEnergyECF {
 public:
  BSEmcFwEndcapECF();
  virtual ~BSEmcFwEndcapECF();

  ClassDef(BSEmcFwEndcapECF, 1);
};

#endif /*BSEMCFWENDCAPECF_HH*/
