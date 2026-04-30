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

#ifndef BSEMCFWENDCAP2DMAPPER_HH
#define BSEMCFWENDCAP2DMAPPER_HH

#include "BSEmc2DMapper.h"

/**
 * @class BSEmcFwEndcap2DMapper
 * @brief Forward Endcap specific DetectorId to x-y-index mapping
 * @details
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup FwEndcap
 */
class BSEmcFwEndcap2DMapper : public BSEmc2DMapper {
 public:
  BSEmcFwEndcap2DMapper() : BSEmc2DMapper() {}
  virtual ~BSEmcFwEndcap2DMapper() {}

  virtual std::pair<Int_t, Int_t> GetXYIndices(Int_t t_detectorId) const /*override*/;
 
 private:
  ClassDef(BSEmcFwEndcap2DMapper, 1);
};

#endif /*BSEMCFWENDCAP2DMAPPER_HH*/
