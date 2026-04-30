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

#ifndef BSEMCSHASHLIK2DMAPPER_HH
#define BSEMCSHASHLIK2DMAPPER_HH

#include "BSEmc2DMapper.h"

/**
 * @class BSEmcShashlik2DMapper
 * @brief Shashlik specific detectorID to X-Y-pair mapping
 * @details Based on PndEmc.cxx
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-01
 * @ingroup Shashlik
 */
class BSEmcShashlik2DMapper : public BSEmc2DMapper {
 public:
  BSEmcShashlik2DMapper() : BSEmc2DMapper() {}
  virtual ~BSEmcShashlik2DMapper() {}

  virtual std::pair<Int_t, Int_t> GetXYIndices(Int_t t_detectorId) const /*override*/;

 private:
  ClassDef(BSEmcShashlik2DMapper, 1);
};

#endif /*BSEMCSHASHLIK2DMAPPER_HH*/
