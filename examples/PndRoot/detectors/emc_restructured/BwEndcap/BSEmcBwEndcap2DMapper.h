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

#ifndef BSEMCBWENDCAP2DMAPPER_HH
#define BSEMCBWENDCAP2DMAPPER_HH

#include "BSEmc2DMapper.h"
#include "BSEmcDetectorID.h"

/**
 * @class BSEmcBwEndcap2DMapper
 * @brief Backward Endcap specific DetectorId to x-y-index mapping
 * @details
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup BwEndcap
 */
class BSEmcBwEndcap2DMapper : public BSEmc2DMapper {
 public:
  BSEmcBwEndcap2DMapper() : BSEmc2DMapper() {}
  virtual ~BSEmcBwEndcap2DMapper() {}
  virtual std::pair<Int_t, Int_t> GetXYIndices(Int_t t_detectorId) const /*override*/;

 private:
  ClassDef(BSEmcBwEndcap2DMapper, 1);
};

#endif /*BSEMCBWENDCAP2DMAPPER_HH*/
