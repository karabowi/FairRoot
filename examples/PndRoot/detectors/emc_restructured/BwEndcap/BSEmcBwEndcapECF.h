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

#ifndef BSEMCBWENDCAPECF_HH
#define BSEMCBWENDCAPECF_HH

#include "Rtypes.h"

#include "BSEmcThetaEnergyECF.h"

class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcBwEndcapECF
 * @brief Theta and Energy dependent energy correction for the BwEndcap
 * @details based on PndEmcClusterHistCalibrator code and data
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup BwEndcap
 */
class BSEmcBwEndcapECF : public BSEmcThetaEnergyECF {
 public:
  BSEmcBwEndcapECF();
  virtual ~BSEmcBwEndcapECF();

  ClassDef(BSEmcBwEndcapECF, 1);
};

#endif /*BSEMCBWENDCAPECF_HH*/
