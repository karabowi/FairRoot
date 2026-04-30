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

#ifndef BSEMC2DMAPPER_HH
#define BSEMC2DMAPPER_HH

#include <utility>

#include "TObject.h"

/**
 * @class BSEmc2DMapper
 * @brief Interface class to map DetectorIDs (crystal Ids) to x-y-indices
 * @details Has to be implemented by each Emc detector
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmc2DMapper : public TObject {
 public:
  virtual ~BSEmc2DMapper(){};
  /**
   * @brief maps detector Id to x-y indices
   *
   * @param t_detectorId
   * @return std::pair<Int_t, Int_t>
   */
  virtual std::pair<Int_t, Int_t> GetXYIndices(Int_t t_detectorId) const = 0;

 private:
  ClassDef(BSEmc2DMapper, 1);
};

#endif /*BSEMC2DMAPPER_HH*/
