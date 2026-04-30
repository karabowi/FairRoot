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

#ifndef BSEMCCORRECTION_HH
#define BSEMCCORRECTION_HH

#include <memory>

#include "TObject.h"
class BSEmcRecoHit;

/**
 * @class BSEmcCorrection
 * @brief Energy correction factor/function interface
 * @details
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcCorrection : public TObject {
 public:
  /**
   * @brief Apply corrections
   *
   * @param t_recoHit to be corrected RecoHit
   */
  virtual void Correct(BSEmcRecoHit *t_recoHit) const = 0;

 private:
  ClassDef(BSEmcCorrection, 1);
};

#endif /*BSEMCCORRECTION_HH*/
